// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>

#include <Eigen/Cholesky>
#include <Eigen/Core>

#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/system/Discretization.hpp"
#include "wpi/math/system/NumericalIntegration.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/UsageReporting.hpp"
#include "wpi/util/array.hpp"

namespace wpi::math {

/**
 * A particle filter estimates the state of a system by representing the state
 * distribution with a set of weighted samples called particles. Unlike Kalman
 * filters, it doesn't assume the state distribution is Gaussian, so it can
 * handle strongly nonlinear models and multimodal distributions.
 *
 * Each prediction step propagates every particle through the dynamics model and
 * adds sampled process noise. Each correction step reweights the particles by
 * the likelihood of the measurement given each particle, then resamples the
 * particles if too few of them carry most of the weight.
 *
 * The state estimate x-hat is the weighted mean of the particles, and the error
 * covariance P is their weighted sample covariance.
 *
 * The filter's computational cost is proportional to the number of particles.
 * More particles give a more accurate approximation of the state distribution.
 *
 * This class implements the generic particle filter in
 * [2, Algorithm 3, p. 180] with the dynamics model as the importance density
 * [2, eq. (62), p. 179]. That importance density comes from the bootstrap
 * filter [1, Section 3.1, p. 108], also known as the sampling importance
 * resampling (SIR) filter [2, Section V-B1, p. 181]. Unlike those filters,
 * this one only resamples when the effective sample size is low instead of
 * every timestep.
 *
 * The N particles xᵢ with weights Wᵢ approximate the posterior state
 * distribution as [2, eq. (49), p. 178]
 *
 *   p(xₖ | y₁, …, yₖ) ≈ Σᵢ Wᵢ δ(xₖ − xᵢ)
 *
 * where δ is the Dirac delta and Σᵢ Wᵢ = 1.
 *
 * References:
 *
 * [1] N. J. Gordon, D. J. Salmond, and A. F. M. Smith, "Novel approach to
 *     nonlinear/non-Gaussian Bayesian state estimation," IEE Proceedings F,
 *     vol. 140, no. 2, pp. 107–113, 1993. DOI: 10.1049/ip-f-2.1993.0015
 *     https://www.ece.iastate.edu/~namrata/EE520/Gordonnovelapproach.pdf
 *
 * [2] M. S. Arulampalam, S. Maskell, N. Gordon, and T. Clapp, "A tutorial on
 *     particle filters for online nonlinear/non-Gaussian Bayesian tracking,"
 *     IEEE Transactions on Signal Processing, vol. 50, no. 2, pp. 174–188,
 *     2002. DOI: 10.1109/78.978374
 *     https://people.eecs.berkeley.edu/~pabbeel/cs287-fa12/optreadings/Arulampalam_etal_2002.pdf
 *
 * [3] R. Douc, O. Cappé, and E. Moulines, "Comparison of resampling schemes for
 *     particle filtering," Proceedings of the 4th International Symposium on
 *     Image and Signal Processing and Analysis (ISPA), pp. 64–69, 2005.
 *     https://arxiv.org/abs/cs/0507025
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 * @tparam Outputs Number of outputs.
 */
template <int States, int Inputs, int Outputs>
class ParticleFilter {
 public:
  using StateVector = Vectord<States>;
  using InputVector = Vectord<Inputs>;
  using OutputVector = Vectord<Outputs>;

  using StateArray = wpi::util::array<double, States>;
  using OutputArray = wpi::util::array<double, Outputs>;

  using StateMatrix = Matrixd<States, States>;

  using ParticleMatrix = Matrixd<States, Eigen::Dynamic>;
  using WeightVector = Eigen::VectorXd;

  // [1] uses our notation except their particle weights qᵢ are our Wᵢ.
  // [2]'s z, v, n, and wₖⁱ are our y, w, v, and Wᵢ respectively
  // (measurements, process noise, measurement noise, and particle weights).

  /**
   * Constructs a particle filter.
   *
   * All particles start at the zero state, so the initial error covariance is
   * zero. Call SetXhat() and SetP() to set the initial state distribution.
   *
   * See
   * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices
   * for how to select the standard deviations.
   *
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param numParticles       Number of particles. Must be positive.
   * @param dt                 Nominal discretization timestep.
   * @throws std::invalid_argument if numParticles isn't positive.
   */
  ParticleFilter(
      std::function<StateVector(const StateVector&, const InputVector&)> f,
      std::function<OutputVector(const StateVector&, const InputVector&)> h,
      const StateArray& stateStdDevs, const OutputArray& measurementStdDevs,
      int numParticles, wpi::units::second_t dt)
      : ParticleFilter(
            std::move(f), std::move(h), stateStdDevs, measurementStdDevs,
            [](const ParticleMatrix& particles, const WeightVector& weights)
                -> StateVector { return particles * weights; },
            [](const StateVector& a, const StateVector& b) -> StateVector {
              return a - b;
            },
            [](const OutputVector& a, const OutputVector& b) -> OutputVector {
              return a - b;
            },
            [](const StateVector& a, const StateVector& b) -> StateVector {
              return a + b;
            },
            numParticles, dt) {}

  /**
   * Constructs a particle filter with custom mean, residual, and addition
   * functions. Using custom functions for arithmetic can be useful if you have
   * angles in the state or measurements, because they allow you to correctly
   * account for the modular nature of angle arithmetic (see AngleMean(),
   * AngleResidual(), and AngleAdd()).
   *
   * All particles start at the zero state, so the initial error covariance is
   * zero. Call SetXhat() and SetP() to set the initial state distribution.
   *
   * See
   * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices
   * for how to select the standard deviations.
   *
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param meanFuncX          A function that computes the weighted mean of the
   *                           particles (one particle per column) given a
   *                           column vector of weights that sum to one.
   * @param residualFuncX      A function that computes the residual of two
   *                           state vectors (i.e. it subtracts them.)
   * @param residualFuncY      A function that computes the residual of two
   *                           measurement vectors (i.e. it subtracts them.)
   * @param addFuncX           A function that adds two state vectors.
   * @param numParticles       Number of particles. Must be positive.
   * @param dt                 Nominal discretization timestep.
   * @throws std::invalid_argument if numParticles isn't positive.
   */
  ParticleFilter(
      std::function<StateVector(const StateVector&, const InputVector&)> f,
      std::function<OutputVector(const StateVector&, const InputVector&)> h,
      const StateArray& stateStdDevs, const OutputArray& measurementStdDevs,
      std::function<StateVector(const ParticleMatrix&, const WeightVector&)>
          meanFuncX,
      std::function<StateVector(const StateVector&, const StateVector&)>
          residualFuncX,
      std::function<OutputVector(const OutputVector&, const OutputVector&)>
          residualFuncY,
      std::function<StateVector(const StateVector&, const StateVector&)>
          addFuncX,
      int numParticles, wpi::units::second_t dt)
      : m_f(std::move(f)),
        m_h(std::move(h)),
        m_meanFuncX(std::move(meanFuncX)),
        m_residualFuncX(std::move(residualFuncX)),
        m_residualFuncY(std::move(residualFuncY)),
        m_addFuncX(std::move(addFuncX)),
        m_dt(dt) {
    if (numParticles <= 0) {
      std::string msg = "ParticleFilter numParticles must be positive";
      wpi::math::MathSharedStore::ReportError(msg);
      throw std::invalid_argument(msg);
    }

    for (int i = 0; i < States; ++i) {
      m_contStateStdDevs(i) = stateStdDevs[i];
    }
    m_contR = CovarianceMatrix(measurementStdDevs);

    m_particles = ParticleMatrix::Zero(States, numParticles);
    m_weights = WeightVector::Constant(numParticles, 1.0 / numParticles);
    m_resampled = ParticleMatrix{States, numParticles};

    UpdateStatistics();

    wpi::util::ReportUsage("ParticleFilter", "");
  }

  /**
   * Returns the error covariance matrix P, which is the weighted sample
   * covariance of the particles.
   */
  const StateMatrix& P() const { return m_P; }

  /**
   * Returns an element of the error covariance matrix P.
   *
   * @param i Row of P.
   * @param j Column of P.
   */
  double P(int i, int j) const { return m_P(i, j); }

  /**
   * Sets the error covariance matrix P by redrawing the particles from a
   * Gaussian distribution centered on the current state estimate x-hat with
   * covariance P.
   *
   * @param P The error covariance matrix P. Must be symmetric positive
   *          semidefinite.
   */
  void SetP(const StateMatrix& P) {
    // Draw each particle as
    //
    //   xᵢ = x̂ + Sεᵢ
    //
    // where εᵢ ~ N(0, I) and SSᵀ = P. Then
    //
    //   Cov(Sεᵢ) = S E[εᵢεᵢᵀ] Sᵀ = SISᵀ = P
    //
    // so xᵢ ~ N(x̂, P).
    const StateVector xHat = m_xHat;
    const StateMatrix S = CovarianceSqrt(P);
    for (int i = 0; i < NumParticles(); ++i) {
      m_particles.col(i) = m_addFuncX(xHat, S * SampleStandardNormal());
    }
    m_weights.setConstant(1.0 / NumParticles());
    UpdateStatistics();
  }

  /**
   * Returns the state estimate x-hat, which is the weighted mean of the
   * particles.
   */
  const StateVector& Xhat() const { return m_xHat; }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  double Xhat(int i) const { return m_xHat(i); }

  /**
   * Sets the state estimate x-hat by shifting every particle by the same
   * amount. This preserves the shape of the state distribution.
   *
   * @param xHat The state estimate x-hat.
   */
  void SetXhat(const StateVector& xHat) {
    // Shift every particle by δ = x̂′ − x̂, where x̂′ is the new state estimate:
    //
    //   xᵢ ← xᵢ + δ
    //
    // With the default arithmetic, the new mean is Σᵢ Wᵢ(xᵢ + δ) = x̂ + δ = x̂′,
    // and each residual (xᵢ + δ) − (x̂ + δ) = xᵢ − x̂ is unchanged, so P is
    // unchanged.
    const StateVector delta = m_residualFuncX(xHat, m_xHat);
    for (int i = 0; i < NumParticles(); ++i) {
      m_particles.col(i) = m_addFuncX(m_particles.col(i), delta);
    }
    UpdateStatistics();
  }

  /**
   * Sets an element of the state estimate x-hat by shifting every particle by
   * the same amount. This preserves the shape of the state distribution.
   *
   * @param i     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  void SetXhat(int i, double value) {
    StateVector xHat = m_xHat;
    xHat(i) = value;
    SetXhat(xHat);
  }

  /**
   * Resets the observer. All particles are moved to the zero state and given
   * equal weights.
   */
  void Reset() {
    m_particles.setZero();
    m_weights.setConstant(1.0 / NumParticles());
    UpdateStatistics();
  }

  /**
   * Returns the number of particles.
   */
  int NumParticles() const { return static_cast<int>(m_particles.cols()); }

  /**
   * Returns the particles. Each column is one particle.
   */
  const ParticleMatrix& Particles() const { return m_particles; }

  /**
   * Returns the particle weights. They're nonnegative and sum to one.
   */
  const WeightVector& Weights() const { return m_weights; }

  /**
   * Seeds the random number generator used for process noise and resampling.
   * This makes the filter's output reproducible.
   *
   * @param seed The seed.
   */
  void SetSeed(std::mt19937::result_type seed) { m_generator.seed(seed); }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u  New control input from controller.
   * @param dt Timestep for prediction.
   */
  void Predict(const InputVector& u, wpi::units::second_t dt) {
    // Sample each particle from the dynamics model [1, Section 3.1, p. 108]
    // [2, Section V-B1, p. 181]:
    //
    //   xᵢ ← RK4(f, xᵢ, u, dt) + wᵢ,  wᵢ ~ N(0, Q_d)
    //
    // Because the importance density is the dynamics model itself, the weight
    // update only depends on the likelihood [2, eq. (63), p. 179], so the
    // weights don't change during prediction.
    //
    // For dynamics linearized as ẋ = Ax + Bu + w, where w has continuous
    // process noise covariance Q, the discrete process noise covariance over a
    // timestep T = dt is
    //
    //   Q_d = ∫₀ᵀ e^(Aτ) Q e^(Aᵀτ) dτ
    //
    // which DiscretizeAQ() computes exactly. Each particle would need its own
    // linearization A to use that, so the first-order term is used instead:
    //
    //   Q_d = QT + O(T²)
    //
    // Q = diag(σ₁², …, σₙ²) is diagonal, so wᵢ = √T diag(σ) εᵢ with
    // εᵢ ~ N(0, I) gives Cov(wᵢ) = T diag(σ)² = QT.
    const StateVector discStdDevs = m_contStateStdDevs * std::sqrt(dt.value());

    for (int i = 0; i < NumParticles(); ++i) {
      StateVector x = RK4(m_f, StateVector{m_particles.col(i)}, u, dt);
      m_particles.col(i) =
          m_addFuncX(x, discStdDevs.cwiseProduct(SampleStandardNormal()));
    }

    m_dt = dt;

    UpdateStatistics();
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  void Correct(const InputVector& u, const OutputVector& y) {
    Correct<Outputs>(u, y, m_h, m_contR, m_residualFuncY);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * This is useful for when the measurement noise covariances vary.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param R Continuous measurement noise covariance matrix.
   */
  void Correct(const InputVector& u, const OutputVector& y,
               const Matrixd<Outputs, Outputs>& R) {
    Correct<Outputs>(u, y, m_h, R, m_residualFuncY);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * This is useful for when the measurements available during a timestep's
   * Correct() call vary. The h(x, u) passed to the constructor is used if one
   * is not provided (the two-argument version of this function).
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param h A vector-valued function of x and u that returns the measurement
   *          vector.
   * @param R Continuous measurement noise covariance matrix.
   */
  template <int Rows>
  void Correct(
      const InputVector& u, const Vectord<Rows>& y,
      std::function<Vectord<Rows>(const StateVector&, const InputVector&)> h,
      const Matrixd<Rows, Rows>& R) {
    auto residualFuncY = [](const Vectord<Rows>& a,
                            const Vectord<Rows>& b) -> Vectord<Rows> {
      return a - b;
    };
    Correct<Rows>(u, y, std::move(h), R, std::move(residualFuncY));
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * This is useful for when the measurements available during a timestep's
   * Correct() call vary. The h(x, u) passed to the constructor is used if one
   * is not provided (the two-argument version of this function).
   *
   * @param u             Same control input used in the predict step.
   * @param y             Measurement vector.
   * @param h             A vector-valued function of x and u that returns
   *                      the measurement vector.
   * @param R             Continuous measurement noise covariance matrix. Must
   *                      be symmetric positive definite.
   * @param residualFuncY A function that computes the residual of two
   *                      measurement vectors (i.e. it subtracts them.)
   */
  template <int Rows>
  void Correct(
      const InputVector& u, const Vectord<Rows>& y,
      std::function<Vectord<Rows>(const StateVector&, const InputVector&)> h,
      const Matrixd<Rows, Rows>& R,
      std::function<Vectord<Rows>(const Vectord<Rows>&, const Vectord<Rows>&)>
          residualFuncY) {
    // The general sequential importance sampling weight update is
    // [2, eq. (48), p. 178]
    //
    //   Wᵢ ∝ Wᵢ⁻ p(y | xᵢ) p(xᵢ | xᵢ⁻) / q(xᵢ | xᵢ⁻, y)
    //
    // where q is the importance density. Predict() samples from the dynamics
    // model, so q(xᵢ | xᵢ⁻, y) = p(xᵢ | xᵢ⁻) and the update reduces to
    // [2, eqs. (62)–(63), p. 179]
    //
    //   Wᵢ ∝ Wᵢ⁻ p(y | xᵢ)
    //
    // With Gaussian measurement noise, the likelihood is
    //
    //   p(y | xᵢ) = (2π)^(−m/2) |R_d|^(−½) exp(−½ rᵢᵀR_d⁻¹rᵢ)
    //
    // where rᵢ = y − h(xᵢ, u), m is the number of measurements, and
    // R_d = R/T is the discrete measurement noise covariance from
    // DiscretizeR(). The leading constant is the same for every particle, so it
    // cancels during normalization and only exp(−½ rᵢᵀR_d⁻¹rᵢ) is needed.
    //
    // With the Cholesky decomposition R_d = LLᵀ,
    //
    //   rᵢᵀR_d⁻¹rᵢ = rᵢᵀL⁻ᵀL⁻¹rᵢ = (L⁻¹rᵢ)ᵀ(L⁻¹rᵢ) = ‖L⁻¹rᵢ‖²
    //
    // so a triangular solve replaces the matrix inverse.
    const Eigen::LLT<Matrixd<Rows, Rows>> discRllt{DiscretizeR<Rows>(R, m_dt)};

    // Weights are updated in log space to avoid underflow when the likelihoods
    // are tiny:
    //
    //   ℓᵢ = ln Wᵢ⁻ − ½‖L⁻¹rᵢ‖²
    WeightVector logWeights{NumParticles()};
    for (int i = 0; i < NumParticles(); ++i) {
      const Vectord<Rows> r = residualFuncY(y, h(m_particles.col(i), u));
      const double logLikelihood =
          -0.5 * discRllt.matrixL().solve(r).squaredNorm();
      logWeights(i) = std::log(m_weights(i)) + logLikelihood;

      // Treat particles with invalid measurements as impossible
      if (std::isnan(logWeights(i))) {
        logWeights(i) = -std::numeric_limits<double>::infinity();
      }
    }

    const double maxLogWeight = logWeights.maxCoeff();
    if (std::isfinite(maxLogWeight)) {
      // Normalize with the log-sum-exp trick:
      //
      //   Wᵢ = exp(ℓᵢ − ℓₘₐₓ) / Σⱼ exp(ℓⱼ − ℓₘₐₓ)
      //
      // The factor exp(−ℓₘₐₓ) cancels between the numerator and denominator,
      // so this equals exp(ℓᵢ) / Σⱼ exp(ℓⱼ). Subtracting ℓₘₐₓ makes the
      // largest term one, so the sum is at least one and normalization can't
      // divide by zero.
      m_weights = (logWeights.array() - maxLogWeight).exp();
      m_weights /= m_weights.sum();
    } else {
      // No particle is consistent with the measurement, so the measurement
      // carries no usable information. Fall back to equal weights rather than
      // dividing by zero.
      m_weights.setConstant(1.0 / NumParticles());
    }

    // Resample when the estimated effective sample size [2, eq. (51), p. 179]
    //
    //   N̂ₑ = 1 / Σᵢ Wᵢ²
    //
    // drops below half the particle count [2, Algorithm 3, p. 180]. N̂ₑ ranges
    // from 1 (all weight on one particle) to N (equal weights). [2] leaves the
    // threshold as a design choice, and N/2 is a common heuristic.
    const double effectiveSampleSize = 1.0 / m_weights.squaredNorm();
    if (effectiveSampleSize < 0.5 * NumParticles()) {
      Resample();
    }

    UpdateStatistics();
  }

 private:
  std::function<StateVector(const StateVector&, const InputVector&)> m_f;
  std::function<OutputVector(const StateVector&, const InputVector&)> m_h;
  std::function<StateVector(const ParticleMatrix&, const WeightVector&)>
      m_meanFuncX;
  std::function<StateVector(const StateVector&, const StateVector&)>
      m_residualFuncX;
  std::function<OutputVector(const OutputVector&, const OutputVector&)>
      m_residualFuncY;
  std::function<StateVector(const StateVector&, const StateVector&)> m_addFuncX;

  ParticleMatrix m_particles;
  WeightVector m_weights;

  // Scratch storage for resampling, preallocated so steady-state operation
  // doesn't allocate
  ParticleMatrix m_resampled;

  StateVector m_xHat = StateVector::Zero();
  StateMatrix m_P = StateMatrix::Zero();

  StateVector m_contStateStdDevs;
  Matrixd<Outputs, Outputs> m_contR;
  wpi::units::second_t m_dt;

  std::mt19937 m_generator{std::random_device{}()};
  std::normal_distribution<double> m_normal{0.0, 1.0};
  std::uniform_real_distribution<double> m_uniform{0.0, 1.0};

  /**
   * Returns a vector of independent standard normal samples.
   */
  StateVector SampleStandardNormal() {
    StateVector epsilon;
    for (int i = 0; i < States; ++i) {
      epsilon(i) = m_normal(m_generator);
    }
    return epsilon;
  }

  /**
   * Returns S such that SSᵀ = P for a symmetric positive semidefinite P.
   */
  static StateMatrix CovarianceSqrt(const StateMatrix& P) {
    // P = SSᵀ = ΠᵀLDLᵀΠ
    // S = ΠᵀL√(D)
    //
    // where Π is the LDLT permutation (Eigen's P). D is clamped to be
    // nonnegative to absorb roundoff in nearly singular covariances.
    const Eigen::LDLT<StateMatrix> ldlt{P};
    StateMatrix S = StateMatrix{ldlt.matrixL()} *
                    ldlt.vectorD().cwiseMax(0.0).cwiseSqrt().asDiagonal();
    return ldlt.transpositionsP().transpose() * S;
  }

  /**
   * Resamples the particles in proportion to their weights, then resets the
   * weights to be equal.
   */
  void Resample() {
    // Use systematic resampling [2, Algorithm 2, p. 180] [3, Section 2.4],
    // which draws one uniform random offset, then selects particles at evenly
    // spaced points along the cumulative weight distribution. It runs in O(N)
    // time and needs only one random number.
    //
    // Residual and stratified resampling are proven to have lower conditional
    // variance than multinomial resampling [3, Sections 3.2–3.3], but
    // systematic resampling isn't in general [3, Section 3.4]. Systematic
    // resampling is still widely used because it's simple and its empirical
    // performance is generally close to residual and stratified resampling
    // [3, Section 1].
    //
    // With the cumulative weights Cᵢ = W₀ + ⋯ + Wᵢ and C₋₁ = 0, draw
    //
    //   U ~ Uniform[0, 1/N)
    //   uⱼ = U + j/N,  j = 0, …, N − 1
    //
    // and select particle i for each uⱼ where C₍ᵢ₋₁₎ < uⱼ ≤ Cᵢ. Each uⱼ is
    // uniform over a width-1/N interval, so particle i is selected NWᵢ times in
    // expectation and the resampled set is an unbiased estimate of the weighted
    // one.
    const int N = NumParticles();
    const double step = 1.0 / N;
    double position = m_uniform(m_generator) * step;

    int i = 0;
    double cumulativeWeight = m_weights(0);
    for (int j = 0; j < N; ++j) {
      // Roundoff can leave the total cumulative weight slightly below one, so
      // stop at the last particle
      while (position > cumulativeWeight && i < N - 1) {
        ++i;
        cumulativeWeight += m_weights(i);
      }
      m_resampled.col(j) = m_particles.col(i);
      position += step;
    }

    m_particles.swap(m_resampled);
    m_weights.setConstant(step);
  }

  /**
   * Recomputes the state estimate and error covariance from the particles.
   */
  void UpdateStatistics() {
    // With the default arithmetic, these are the mean and covariance of the
    // weighted particle approximation of the posterior:
    //
    //   x̂ = Σᵢ Wᵢxᵢ
    //   P = Σᵢ Wᵢ(xᵢ − x̂)(xᵢ − x̂)ᵀ
    //
    // P has no small-sample bias correction. With equal weights, P is
    // (N − 1)/N times the unbiased sample covariance, which is negligible for
    // typical particle counts.
    m_xHat = m_meanFuncX(m_particles, m_weights);

    m_P.setZero();
    for (int i = 0; i < NumParticles(); ++i) {
      const StateVector r = m_residualFuncX(m_particles.col(i), m_xHat);
      m_P += m_weights(i) * r * r.transpose();
    }
  }
};

}  // namespace wpi::math
