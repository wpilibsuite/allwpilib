// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import java.util.Random;
import java.util.function.BiFunction;
import org.ejml.data.DMatrixRMaj;
import org.ejml.dense.row.CommonOps_DDRM;
import org.ejml.dense.row.decomposition.TriangularSolver_DDRM;
import org.ejml.dense.row.factory.DecompositionFactory_DDRM;
import org.ejml.interfaces.decomposition.EigenDecomposition_F64;
import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.system.Discretization;
import org.wpilib.math.system.NumericalIntegration;
import org.wpilib.math.util.Nat;
import org.wpilib.math.util.Num;
import org.wpilib.math.util.StateSpaceUtil;
import org.wpilib.util.UsageReporting;

/**
 * A particle filter estimates the state of a system by representing the state distribution with a
 * set of weighted samples called particles. Unlike Kalman filters, it doesn't assume the state
 * distribution is Gaussian, so it can handle strongly nonlinear models and multimodal
 * distributions.
 *
 * <p>Each prediction step propagates every particle through the dynamics model and adds sampled
 * process noise. Each correction step reweights the particles by the likelihood of the measurement
 * given each particle, then resamples the particles if too few of them carry most of the weight.
 *
 * <p>The state estimate x-hat is the weighted mean of the particles, and the error covariance P is
 * their weighted sample covariance.
 *
 * <p>The filter's computational cost is proportional to the number of particles. More particles
 * give a more accurate approximation of the state distribution.
 *
 * <p>This class implements the generic particle filter in [2, Algorithm 3, p. 180] with the
 * dynamics model as the importance density [2, eq. (62), p. 179]. That importance density comes
 * from the bootstrap filter [1, Section 3.1, p. 108], also known as the sampling importance
 * resampling (SIR) filter [2, Section V-B1, p. 181]. Unlike those filters, this one only resamples
 * when the effective sample size is low instead of every timestep.
 *
 * <p>As shown in [2, eq. (49), p. 178], the N particles xᵢ with weights Wᵢ approximate the
 * posterior state distribution as
 *
 * <pre>
 *   p(xₖ | y₁, …, yₖ) ≈ Σᵢ Wᵢ δ(xₖ − xᵢ)
 * </pre>
 *
 * <p>where δ is the Dirac delta and Σᵢ Wᵢ = 1.
 *
 * <p>References:
 *
 * <p>[1] N. J. Gordon, D. J. Salmond, and A. F. M. Smith, "Novel approach to nonlinear/non-Gaussian
 * Bayesian state estimation," IEE Proceedings F, vol. 140, no. 2, pp. 107–113, 1993. DOI:
 * 10.1049/ip-f-2.1993.0015 <a
 * href="https://www.ece.iastate.edu/~namrata/EE520/Gordonnovelapproach.pdf">https://www.ece.iastate.edu/~namrata/EE520/Gordonnovelapproach.pdf</a>
 *
 * <p>[2] M. S. Arulampalam, S. Maskell, N. Gordon, and T. Clapp, "A tutorial on particle filters
 * for online nonlinear/non-Gaussian Bayesian tracking," IEEE Transactions on Signal Processing,
 * vol. 50, no. 2, pp. 174–188, 2002. DOI: 10.1109/78.978374 <a
 * href="https://people.eecs.berkeley.edu/~pabbeel/cs287-fa12/optreadings/Arulampalam_etal_2002.pdf">https://people.eecs.berkeley.edu/~pabbeel/cs287-fa12/optreadings/Arulampalam_etal_2002.pdf</a>
 *
 * <p>[3] R. Douc, O. Cappé, and E. Moulines, "Comparison of resampling schemes for particle
 * filtering," Proceedings of the 4th International Symposium on Image and Signal Processing and
 * Analysis (ISPA), pp. 64–69, 2005. <a
 * href="https://arxiv.org/abs/cs/0507025">https://arxiv.org/abs/cs/0507025</a>
 *
 * @param <States> Number of states.
 * @param <Inputs> Number of inputs.
 * @param <Outputs> Number of outputs.
 */
public class ParticleFilter<States extends Num, Inputs extends Num, Outputs extends Num>
    implements KalmanTypeFilter<States, Inputs, Outputs> {
  // [1] uses our notation except their particle weights qᵢ are our Wᵢ.
  // [2]'s z, v, n, and wₖⁱ are our y, w, v, and Wᵢ respectively
  // (measurements, process noise, measurement noise, and particle weights).

  private final Nat<States> m_states;
  private final Nat<Outputs> m_outputs;

  private final BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> m_f;
  private final BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> m_h;
  private final BiFunction<Matrix<States, ?>, Matrix<?, N1>, Matrix<States, N1>> m_meanFuncX;
  private final BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>>
      m_residualFuncX;
  private final BiFunction<Matrix<Outputs, N1>, Matrix<Outputs, N1>, Matrix<Outputs, N1>>
      m_residualFuncY;
  private final BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> m_addFuncX;

  // Each column is one particle
  private SimpleMatrix m_particles;

  // Column vector of particle weights
  private final SimpleMatrix m_weights;

  // Scratch storage for resampling, preallocated to reduce garbage
  private SimpleMatrix m_resampled;

  private Matrix<States, N1> m_xHat;
  private Matrix<States, States> m_P;

  private final Matrix<States, N1> m_contStateStdDevs;
  private final Matrix<Outputs, Outputs> m_contR;
  private double m_dt;

  private final Random m_random = new Random();

  /**
   * Constructs a particle filter.
   *
   * <p>All particles start at the zero state, so the initial error covariance is zero. Call
   * setXhat() and setP() to set the initial state distribution.
   *
   * <p>See <a
   * href="https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices">https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices</a>
   * for how to select the standard deviations.
   *
   * @param states A Nat representing the number of states.
   * @param outputs A Nat representing the number of outputs.
   * @param f A vector-valued function of x and u that returns the derivative of the state vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param stateStdDevs Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param numParticles Number of particles. Must be positive.
   * @param dt Nominal discretization timestep in seconds.
   * @throws IllegalArgumentException if numParticles isn't positive.
   */
  public ParticleFilter(
      Nat<States> states,
      Nat<Outputs> outputs,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> h,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> measurementStdDevs,
      int numParticles,
      double dt) {
    this(
        states,
        outputs,
        f,
        h,
        stateStdDevs,
        measurementStdDevs,
        (particles, weights) -> particles.times(Matrix.changeBoundsUnchecked(weights)),
        Matrix::minus,
        Matrix::minus,
        Matrix::plus,
        numParticles,
        dt);
  }

  /**
   * Constructs a particle filter with custom mean, residual, and addition functions. Using custom
   * functions for arithmetic can be useful if you have angles in the state or measurements, because
   * they allow you to correctly account for the modular nature of angle arithmetic (see
   * AngleStatistics.angleMean(), AngleStatistics.angleResidual(), and AngleStatistics.angleAdd()).
   *
   * <p>All particles start at the zero state, so the initial error covariance is zero. Call
   * setXhat() and setP() to set the initial state distribution.
   *
   * <p>See <a
   * href="https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices">https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices</a>
   * for how to select the standard deviations.
   *
   * @param states A Nat representing the number of states.
   * @param outputs A Nat representing the number of outputs.
   * @param f A vector-valued function of x and u that returns the derivative of the state vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param stateStdDevs Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param meanFuncX A function that computes the weighted mean of the particles (one particle per
   *     column) given a column vector of weights that sum to one.
   * @param residualFuncX A function that computes the residual of two state vectors (i.e. it
   *     subtracts them.)
   * @param residualFuncY A function that computes the residual of two measurement vectors (i.e. it
   *     subtracts them.)
   * @param addFuncX A function that adds two state vectors.
   * @param numParticles Number of particles. Must be positive.
   * @param dt Nominal discretization timestep in seconds.
   * @throws IllegalArgumentException if numParticles isn't positive.
   */
  public ParticleFilter(
      Nat<States> states,
      Nat<Outputs> outputs,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> h,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> measurementStdDevs,
      BiFunction<Matrix<States, ?>, Matrix<?, N1>, Matrix<States, N1>> meanFuncX,
      BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> residualFuncX,
      BiFunction<Matrix<Outputs, N1>, Matrix<Outputs, N1>, Matrix<Outputs, N1>> residualFuncY,
      BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> addFuncX,
      int numParticles,
      double dt) {
    if (numParticles <= 0) {
      throw new IllegalArgumentException("ParticleFilter numParticles must be positive");
    }

    m_states = states;
    m_outputs = outputs;

    m_f = f;
    m_h = h;
    m_meanFuncX = meanFuncX;
    m_residualFuncX = residualFuncX;
    m_residualFuncY = residualFuncY;
    m_addFuncX = addFuncX;

    m_contStateStdDevs = stateStdDevs.copy();
    m_contR = StateSpaceUtil.covarianceMatrix(outputs, measurementStdDevs);
    m_dt = dt;

    m_particles = new SimpleMatrix(states.getNum(), numParticles);
    m_weights = new SimpleMatrix(numParticles, 1);
    m_weights.fill(1.0 / numParticles);
    m_resampled = new SimpleMatrix(states.getNum(), numParticles);

    updateStatistics();

    UsageReporting.reportUsage("ParticleFilter", "");
  }

  /**
   * Returns the error covariance matrix P, which is the weighted sample covariance of the
   * particles.
   *
   * @return The error covariance matrix P.
   */
  @Override
  public Matrix<States, States> getP() {
    return m_P;
  }

  /**
   * Returns an element of the error covariance matrix P.
   *
   * @param row Row of P.
   * @param col Column of P.
   * @return The value of the error covariance matrix P at (i, j).
   */
  @Override
  public double getP(int row, int col) {
    return m_P.get(row, col);
  }

  /**
   * Sets the error covariance matrix P by redrawing the particles from a Gaussian distribution
   * centered on the current state estimate x-hat with covariance P.
   *
   * @param newP The error covariance matrix P. Must be symmetric positive semidefinite.
   */
  @Override
  public void setP(Matrix<States, States> newP) {
    // Draw each particle as
    //
    //   xᵢ = x̂ + Sεᵢ
    //
    // where εᵢ ~ N(0, I) and SSᵀ = P. Then
    //
    //   Cov(Sεᵢ) = S E[εᵢεᵢᵀ] Sᵀ = SISᵀ = P
    //
    // so xᵢ ~ N(x̂, P).
    final var xHat = m_xHat;
    final var S = covarianceSqrt(newP);
    for (int i = 0; i < getNumParticles(); ++i) {
      setParticle(i, m_addFuncX.apply(xHat, S.times(sampleStandardNormal())));
    }
    m_weights.fill(1.0 / getNumParticles());
    updateStatistics();
  }

  /**
   * Returns the state estimate x-hat, which is the weighted mean of the particles.
   *
   * @return The state estimate x-hat.
   */
  @Override
  public Matrix<States, N1> getXhat() {
    return m_xHat;
  }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param row Row of x-hat.
   * @return The value of the state estimate x-hat at that row.
   */
  @Override
  public double getXhat(int row) {
    return m_xHat.get(row, 0);
  }

  /**
   * Sets the state estimate x-hat by shifting every particle by the same amount. This preserves the
   * shape of the state distribution.
   *
   * @param xHat The state estimate x-hat.
   */
  @Override
  public void setXhat(Matrix<States, N1> xHat) {
    // Shift every particle by δ = x̂′ − x̂, where x̂′ is the new state estimate:
    //
    //   xᵢ ← xᵢ + δ
    //
    // With the default arithmetic, the new mean is Σᵢ Wᵢ(xᵢ + δ) = x̂ + δ = x̂′,
    // and each residual (xᵢ + δ) − (x̂ + δ) = xᵢ − x̂ is unchanged, so P is
    // unchanged.
    final var delta = m_residualFuncX.apply(xHat, m_xHat);
    for (int i = 0; i < getNumParticles(); ++i) {
      setParticle(i, m_addFuncX.apply(getParticle(i), delta));
    }
    updateStatistics();
  }

  /**
   * Sets an element of the state estimate x-hat by shifting every particle by the same amount. This
   * preserves the shape of the state distribution.
   *
   * @param row Row of x-hat.
   * @param value Value for element of x-hat.
   */
  @Override
  public void setXhat(int row, double value) {
    final var xHat = m_xHat.copy();
    xHat.set(row, 0, value);
    setXhat(xHat);
  }

  /** Resets the observer. All particles are moved to the zero state and given equal weights. */
  @Override
  public final void reset() {
    m_particles.zero();
    m_weights.fill(1.0 / getNumParticles());
    updateStatistics();
  }

  /**
   * Returns the number of particles.
   *
   * @return The number of particles.
   */
  public final int getNumParticles() {
    return m_particles.getNumCols();
  }

  /**
   * Returns a copy of the particles. Each column is one particle.
   *
   * @return A copy of the particles.
   */
  public Matrix<States, ?> getParticles() {
    return new Matrix<>(m_particles.copy());
  }

  /**
   * Returns a copy of the particle weights. They're nonnegative and sum to one.
   *
   * @return A copy of the particle weights.
   */
  public Matrix<?, N1> getWeights() {
    return new Matrix<>(m_weights.copy());
  }

  /**
   * Seeds the random number generator used for process noise and resampling. This makes the
   * filter's output reproducible.
   *
   * @param seed The seed.
   */
  public void setSeed(long seed) {
    m_random.setSeed(seed);
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u New control input from controller.
   * @param dt Timestep for prediction in seconds.
   */
  @Override
  public void predict(Matrix<Inputs, N1> u, double dt) {
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
    // which Discretization.discretizeAQ() computes exactly. Each particle would
    // need its own linearization A to use that, so the first-order term is used
    // instead:
    //
    //   Q_d = QT + O(T²)
    //
    // Q = diag(σ₁², …, σₙ²) is diagonal, so wᵢ = √T diag(σ) εᵢ with
    // εᵢ ~ N(0, I) gives Cov(wᵢ) = T diag(σ)² = QT.
    final var discStdDevs = m_contStateStdDevs.times(Math.sqrt(dt));

    for (int i = 0; i < getNumParticles(); ++i) {
      final var x = NumericalIntegration.rk4(m_f, getParticle(i), u, dt);
      setParticle(i, m_addFuncX.apply(x, discStdDevs.elementTimes(sampleStandardNormal())));
    }

    m_dt = dt;

    updateStatistics();
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  @Override
  public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y) {
    correct(m_outputs, u, y, m_h, m_contR, m_residualFuncY);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurement noise covariances vary.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param R Continuous measurement noise covariance matrix.
   */
  public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y, Matrix<Outputs, Outputs> R) {
    correct(m_outputs, u, y, m_h, R, m_residualFuncY);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurements available during a timestep's correct() call vary.
   * The h(x, u) passed to the constructor is used if one is not provided (the two-argument version
   * of this function).
   *
   * @param <Rows> Number of rows in the result of h(x, u).
   * @param rows Number of rows in the result of h(x, u).
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param R Continuous measurement noise covariance matrix.
   */
  public <Rows extends Num> void correct(
      Nat<Rows> rows,
      Matrix<Inputs, N1> u,
      Matrix<Rows, N1> y,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Rows, N1>> h,
      Matrix<Rows, Rows> R) {
    correct(rows, u, y, h, R, Matrix::minus);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurements available during a timestep's correct() call vary.
   * The h(x, u) passed to the constructor is used if one is not provided (the two-argument version
   * of this function).
   *
   * @param <Rows> Number of rows in the result of h(x, u).
   * @param rows Number of rows in the result of h(x, u).
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param R Continuous measurement noise covariance matrix. Must be symmetric positive definite.
   * @param residualFuncY A function that computes the residual of two measurement vectors (i.e. it
   *     subtracts them.)
   */
  public <Rows extends Num> void correct(
      Nat<Rows> rows,
      Matrix<Inputs, N1> u,
      Matrix<Rows, N1> y,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Rows, N1>> h,
      Matrix<Rows, Rows> R,
      BiFunction<Matrix<Rows, N1>, Matrix<Rows, N1>, Matrix<Rows, N1>> residualFuncY) {
    // The general sequential importance sampling weight update is
    // [2, eq. (48), p. 178]
    //
    //   Wᵢ ∝ Wᵢ⁻ p(y | xᵢ) p(xᵢ | xᵢ⁻) / q(xᵢ | xᵢ⁻, y)
    //
    // where q is the importance density. predict() samples from the dynamics
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
    // Discretization.discretizeR(). The leading constant is the same for every
    // particle, so it cancels during normalization and only
    // exp(−½ rᵢᵀR_d⁻¹rᵢ) is needed.
    //
    // With the Cholesky decomposition R_d = LLᵀ,
    //
    //   rᵢᵀR_d⁻¹rᵢ = rᵢᵀL⁻ᵀL⁻¹rᵢ = (L⁻¹rᵢ)ᵀ(L⁻¹rᵢ) = ‖L⁻¹rᵢ‖²
    //
    // so a triangular solve replaces the matrix inverse.
    final double[] L =
        Discretization.discretizeR(R, m_dt).lltDecompose(true).getStorage().getDDRM().data;

    // Weights are updated in log space to avoid underflow when the likelihoods
    // are tiny:
    //
    //   ℓᵢ = ln Wᵢ⁻ − ½‖L⁻¹rᵢ‖²
    final int N = getNumParticles();
    final double[] logWeights = new double[N];
    for (int i = 0; i < N; ++i) {
      final var r = residualFuncY.apply(y, h.apply(getParticle(i), u));

      // Solve Lz = r in place for z = L⁻¹r
      final double[] z = r.getData().clone();
      TriangularSolver_DDRM.solveL(L, z, rows.getNum());
      double squaredNorm = 0.0;
      for (double elem : z) {
        squaredNorm += elem * elem;
      }

      logWeights[i] = Math.log(m_weights.get(i, 0)) - 0.5 * squaredNorm;

      // Treat particles with invalid measurements as impossible
      if (Double.isNaN(logWeights[i])) {
        logWeights[i] = Double.NEGATIVE_INFINITY;
      }
    }

    double maxLogWeight = Double.NEGATIVE_INFINITY;
    for (double logWeight : logWeights) {
      maxLogWeight = Math.max(maxLogWeight, logWeight);
    }

    if (Double.isFinite(maxLogWeight)) {
      // Normalize with the log-sum-exp trick:
      //
      //   Wᵢ = exp(ℓᵢ − ℓₘₐₓ) / Σⱼ exp(ℓⱼ − ℓₘₐₓ)
      //
      // The factor exp(−ℓₘₐₓ) cancels between the numerator and denominator,
      // so this equals exp(ℓᵢ) / Σⱼ exp(ℓⱼ). Subtracting ℓₘₐₓ makes the
      // largest term one, so the sum is at least one and normalization can't
      // divide by zero.
      double sum = 0.0;
      for (int i = 0; i < N; ++i) {
        final double weight = Math.exp(logWeights[i] - maxLogWeight);
        m_weights.set(i, 0, weight);
        sum += weight;
      }
      CommonOps_DDRM.divide(m_weights.getDDRM(), sum);
    } else {
      // No particle is consistent with the measurement, so the measurement
      // carries no usable information. Fall back to equal weights rather than
      // dividing by zero.
      m_weights.fill(1.0 / N);
    }

    // Resample when the estimated effective sample size [2, eq. (51), p. 179]
    //
    //   N̂ₑ = 1 / Σᵢ Wᵢ²
    //
    // drops below half the particle count [2, Algorithm 3, p. 180]. N̂ₑ ranges
    // from 1 (all weight on one particle) to N (equal weights). [2] leaves the
    // threshold as a design choice, and N/2 is a common heuristic.
    double sumSquaredWeights = 0.0;
    for (int i = 0; i < N; ++i) {
      sumSquaredWeights += m_weights.get(i, 0) * m_weights.get(i, 0);
    }
    final double effectiveSampleSize = 1.0 / sumSquaredWeights;
    if (effectiveSampleSize < 0.5 * N) {
      resample();
    }

    updateStatistics();
  }

  /**
   * Returns particle i.
   *
   * @param i The particle index.
   * @return Particle i.
   */
  private Matrix<States, N1> getParticle(int i) {
    return new Matrix<>(m_particles.extractVector(false, i));
  }

  /**
   * Sets particle i.
   *
   * @param i The particle index.
   * @param x The particle's new state.
   */
  private void setParticle(int i, Matrix<States, N1> x) {
    m_particles.insertIntoThis(0, i, x.getStorage());
  }

  /**
   * Returns a vector of independent standard normal samples.
   *
   * @return A vector of independent standard normal samples.
   */
  private Matrix<States, N1> sampleStandardNormal() {
    final var epsilon = new Matrix<>(m_states, Nat.N1());
    for (int i = 0; i < m_states.getNum(); ++i) {
      epsilon.set(i, 0, m_random.nextGaussian());
    }
    return epsilon;
  }

  /**
   * Returns S such that SSᵀ = P for a symmetric positive semidefinite P.
   *
   * @param P A symmetric positive semidefinite matrix.
   * @return S such that SSᵀ = P.
   */
  private Matrix<States, States> covarianceSqrt(Matrix<States, States> P) {
    // EJML doesn't have a pivoted LDLT decomposition, and its Cholesky
    // decomposition fails for singular covariances, so use the symmetric
    // eigendecomposition instead.
    //
    // P = SSᵀ = VΛVᵀ
    // S = V√(Λ)
    //
    // where V's columns are P's orthonormal eigenvectors and Λ is the diagonal
    // matrix of P's eigenvalues. Λ is clamped to be nonnegative to absorb
    // roundoff in nearly singular covariances.
    final int n = m_states.getNum();
    final EigenDecomposition_F64<DMatrixRMaj> eig = DecompositionFactory_DDRM.eig(n, true, true);
    if (!eig.decompose(P.getStorage().getDDRM().copy())) {
      throw new IllegalArgumentException("Eigendecomposition of P failed:\n" + P);
    }

    final var S = new Matrix<>(m_states, m_states);
    for (int j = 0; j < n; ++j) {
      final double sqrtLambda = Math.sqrt(Math.max(eig.getEigenvalue(j).getReal(), 0.0));
      final DMatrixRMaj v = eig.getEigenVector(j);
      for (int i = 0; i < n; ++i) {
        S.set(i, j, v.get(i, 0) * sqrtLambda);
      }
    }
    return S;
  }

  /**
   * Resamples the particles in proportion to their weights, then resets the weights to be equal.
   */
  private void resample() {
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
    final int N = getNumParticles();
    final double step = 1.0 / N;
    double position = m_random.nextDouble() * step;

    int i = 0;
    double cumulativeWeight = m_weights.get(0, 0);
    for (int j = 0; j < N; ++j) {
      // Roundoff can leave the total cumulative weight slightly below one, so
      // stop at the last particle
      while (position > cumulativeWeight && i < N - 1) {
        ++i;
        cumulativeWeight += m_weights.get(i, 0);
      }
      m_resampled.insertIntoThis(0, j, m_particles.extractVector(false, i));
      position += step;
    }

    final var temp = m_particles;
    m_particles = m_resampled;
    m_resampled = temp;
    m_weights.fill(step);
  }

  /** Recomputes the state estimate and error covariance from the particles. */
  private void updateStatistics() {
    // With the default arithmetic, these are the mean and covariance of the
    // weighted particle approximation of the posterior:
    //
    //   x̂ = Σᵢ Wᵢxᵢ
    //   P = Σᵢ Wᵢ(xᵢ − x̂)(xᵢ − x̂)ᵀ
    //
    // P has no small-sample bias correction. With equal weights, P is
    // (N − 1)/N times the unbiased sample covariance, which is negligible for
    // typical particle counts.
    m_xHat = m_meanFuncX.apply(new Matrix<>(m_particles), new Matrix<>(m_weights));

    final var P = new Matrix<>(m_states, m_states);
    for (int i = 0; i < getNumParticles(); ++i) {
      final var r = m_residualFuncX.apply(getParticle(i), m_xHat);
      final double weight = m_weights.get(i, 0);
      for (int row = 0; row < m_states.getNum(); ++row) {
        for (int col = 0; col < m_states.getNum(); ++col) {
          P.set(row, col, P.get(row, col) + weight * r.get(row, 0) * r.get(col, 0));
        }
      }
    }
    m_P = P;
  }
}
