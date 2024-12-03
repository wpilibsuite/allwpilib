// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <utility>

#include <Eigen/Cholesky>
#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/EigenCore.h"
#include "frc/StateSpaceUtil.h"
#include "frc/estimator/MerweScaledSigmaPoints.h"
#include "frc/estimator/UnscentedTransform.h"
#include "frc/system/Discretization.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/NumericalJacobian.h"
#include "units/time.h"

namespace frc {

/**
 * A Kalman filter combines predictions from a model and measurements to give an
 * estimate of the true system state. This is useful because many states cannot
 * be measured directly as a result of sensor noise, or because the state is
 * "hidden".
 *
 * Kalman filters use a K gain matrix to determine whether to trust the model or
 * measurements more. Kalman filter theory uses statistics to compute an optimal
 * K gain which minimizes the sum of squares error in the state estimate. This K
 * gain is used to correct the state estimate by some amount of the difference
 * between the actual measurements and the measurements predicted by the model.
 *
 * An unscented Kalman filter uses nonlinear state and measurement models. It
 * propagates the error covariance using sigma points chosen to approximate the
 * true probability distribution.
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf chapter 9
 * "Stochastic control theory".
 *
 * <p> This class implements a square-root-form unscented Kalman filter
 * (SR-UKF). For more information about the SR-UKF, see
 * https://www.researchgate.net/publication/3908304.
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 * @tparam Outputs Number of outputs.
 */
template <int States, int Inputs, int Outputs>
class UnscentedKalmanFilter {
 public:
  using StateVector = Vectord<States>;
  using InputVector = Vectord<Inputs>;
  using OutputVector = Vectord<Outputs>;

  using StateArray = wpi::array<double, States>;
  using OutputArray = wpi::array<double, Outputs>;

  using StateMatrix = Matrixd<States, States>;

  /**
   * Constructs an unscented Kalman filter.
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
   * @param dt                 Nominal discretization timestep.
   */
  UnscentedKalmanFilter(
      std::function<StateVector(const StateVector&, const InputVector&)> f,
      std::function<OutputVector(const StateVector&, const InputVector&)> h,
      const StateArray& stateStdDevs, const OutputArray& measurementStdDevs,
      units::second_t dt)
      : m_f(std::move(f)), m_h(std::move(h)) {
    m_contQ = MakeCovMatrix(stateStdDevs);
    m_contR = MakeCovMatrix(measurementStdDevs);
    m_meanFuncX = [](const Matrixd<States, 2 * States + 1>& sigmas,
                     const Vectord<2 * States + 1>& Wm) -> StateVector {
      return sigmas * Wm;
    };
    m_meanFuncY = [](const Matrixd<Outputs, 2 * States + 1>& sigmas,
                     const Vectord<2 * States + 1>& Wc) -> OutputVector {
      return sigmas * Wc;
    };
    m_residualFuncX = [](const StateVector& a,
                         const StateVector& b) -> StateVector { return a - b; };
    m_residualFuncY = [](const OutputVector& a,
                         const OutputVector& b) -> OutputVector {
      return a - b;
    };
    m_addFuncX = [](const StateVector& a, const StateVector& b) -> StateVector {
      return a + b;
    };
    m_dt = dt;

    Reset();
  }

  /**
   * Constructs an unscented Kalman filter with custom mean, residual, and
   * addition functions. Using custom functions for arithmetic can be useful if
   * you have angles in the state or measurements, because they allow you to
   * correctly account for the modular nature of angle arithmetic.
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
   * @param meanFuncX          A function that computes the mean of 2 * States +
   *                           1 state vectors using a given set of weights.
   * @param meanFuncY          A function that computes the mean of 2 * States +
   *                           1 measurement vectors using a given set of
   *                           weights.
   * @param residualFuncX      A function that computes the residual of two
   *                           state vectors (i.e. it subtracts them.)
   * @param residualFuncY      A function that computes the residual of two
   *                           measurement vectors (i.e. it subtracts them.)
   * @param addFuncX           A function that adds two state vectors.
   * @param dt                 Nominal discretization timestep.
   */
  UnscentedKalmanFilter(
      std::function<StateVector(const StateVector&, const InputVector&)> f,
      std::function<OutputVector(const StateVector&, const InputVector&)> h,
      const StateArray& stateStdDevs, const OutputArray& measurementStdDevs,
      std::function<StateVector(const Matrixd<States, 2 * States + 1>&,
                                const Vectord<2 * States + 1>&)>
          meanFuncX,
      std::function<OutputVector(const Matrixd<Outputs, 2 * States + 1>&,
                                 const Vectord<2 * States + 1>&)>
          meanFuncY,
      std::function<StateVector(const StateVector&, const StateVector&)>
          residualFuncX,
      std::function<OutputVector(const OutputVector&, const OutputVector&)>
          residualFuncY,
      std::function<StateVector(const StateVector&, const StateVector&)>
          addFuncX,
      units::second_t dt)
      : m_f(std::move(f)),
        m_h(std::move(h)),
        m_meanFuncX(std::move(meanFuncX)),
        m_meanFuncY(std::move(meanFuncY)),
        m_residualFuncX(std::move(residualFuncX)),
        m_residualFuncY(std::move(residualFuncY)),
        m_addFuncX(std::move(addFuncX)) {
    m_contQ = MakeCovMatrix(stateStdDevs);
    m_contR = MakeCovMatrix(measurementStdDevs);
    m_dt = dt;

    Reset();
  }

  /**
   * Returns the square-root error covariance matrix S.
   */
  const StateMatrix& S() const { return m_S; }

  /**
   * Returns an element of the square-root error covariance matrix S.
   *
   * @param i Row of S.
   * @param j Column of S.
   */
  double S(int i, int j) const { return m_S(i, j); }

  /**
   * Set the current square-root error covariance matrix S.
   *
   * @param S The square-root error covariance matrix S.
   */
  void SetS(const StateMatrix& S) { m_S = S; }

  /**
   * Returns the reconstructed error covariance matrix P.
   */
  StateMatrix P() const { return m_S.transpose() * m_S; }

  /**
   * Set the current square-root error covariance matrix S by taking the square
   * root of P.
   *
   * @param P The error covariance matrix P.
   */
  void SetP(const StateMatrix& P) { m_S = P.llt().matrixU(); }

  /**
   * Returns the state estimate x-hat.
   */
  const StateVector& Xhat() const { return m_xHat; }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  double Xhat(int i) const { return m_xHat(i); }

  /**
   * Set initial state estimate x-hat.
   *
   * @param xHat The state estimate x-hat.
   */
  void SetXhat(const StateVector& xHat) { m_xHat = xHat; }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param i     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  void SetXhat(int i, double value) { m_xHat(i) = value; }

  /**
   * Resets the observer.
   */
  void Reset() {
    m_xHat.setZero();
    m_S.setZero();
    m_sigmasF.setZero();
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u  New control input from controller.
   * @param dt Timestep for prediction.
   */
  void Predict(const InputVector& u, units::second_t dt) {
    m_dt = dt;

    // Discretize Q before projecting mean and covariance forward
    StateMatrix contA =
        NumericalJacobianX<States, States, Inputs>(m_f, m_xHat, u);
    StateMatrix discA;
    StateMatrix discQ;
    DiscretizeAQ<States>(contA, m_contQ, m_dt, &discA, &discQ);
    Eigen::internal::llt_inplace<double, Eigen::Lower>::blocked(discQ);

    Matrixd<States, 2 * States + 1> sigmas =
        m_pts.SquareRootSigmaPoints(m_xHat, m_S);

    for (int i = 0; i < m_pts.NumSigmas(); ++i) {
      StateVector x = sigmas.template block<States, 1>(0, i);
      m_sigmasF.template block<States, 1>(0, i) = RK4(m_f, x, u, dt);
    }

    auto [xHat, S] = SquareRootUnscentedTransform<States, States>(
        m_sigmasF, m_pts.Wm(), m_pts.Wc(), m_meanFuncX, m_residualFuncX,
        discQ.template triangularView<Eigen::Lower>());
    m_xHat = xHat;
    m_S = S;
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  void Correct(const InputVector& u, const OutputVector& y) {
    Correct<Outputs>(u, y, m_h, m_contR, m_meanFuncY, m_residualFuncY,
                     m_residualFuncX, m_addFuncX);
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
    Correct<Outputs>(u, y, m_h, R, m_meanFuncY, m_residualFuncY,
                     m_residualFuncX, m_addFuncX);
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
    auto meanFuncY = [](const Matrixd<Outputs, 2 * States + 1>& sigmas,
                        const Vectord<2 * States + 1>& Wc) -> Vectord<Rows> {
      return sigmas * Wc;
    };
    auto residualFuncX = [](const StateVector& a,
                            const StateVector& b) -> StateVector {
      return a - b;
    };
    auto residualFuncY = [](const Vectord<Rows>& a,
                            const Vectord<Rows>& b) -> Vectord<Rows> {
      return a - b;
    };
    auto addFuncX = [](const StateVector& a,
                       const StateVector& b) -> StateVector { return a + b; };
    Correct<Rows>(u, y, std::move(h), R, std::move(meanFuncY),
                  std::move(residualFuncY), std::move(residualFuncX),
                  std::move(addFuncX));
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
   * @param h             A vector-valued function of x and u that returns the
   *                      measurement vector.
   * @param R             Continuous measurement noise covariance matrix.
   * @param meanFuncY     A function that computes the mean of 2 * States + 1
   *                      measurement vectors using a given set of weights.
   * @param residualFuncY A function that computes the residual of two
   *                      measurement vectors (i.e. it subtracts them.)
   * @param residualFuncX A function that computes the residual of two state
   *                      vectors (i.e. it subtracts them.)
   * @param addFuncX      A function that adds two state vectors.
   */
  template <int Rows>
  void Correct(
      const InputVector& u, const Vectord<Rows>& y,
      std::function<Vectord<Rows>(const StateVector&, const InputVector&)> h,
      const Matrixd<Rows, Rows>& R,
      std::function<Vectord<Rows>(const Matrixd<Rows, 2 * States + 1>&,
                                  const Vectord<2 * States + 1>&)>
          meanFuncY,
      std::function<Vectord<Rows>(const Vectord<Rows>&, const Vectord<Rows>&)>
          residualFuncY,
      std::function<StateVector(const StateVector&, const StateVector&)>
          residualFuncX,
      std::function<StateVector(const StateVector&, const StateVector&)>
          addFuncX) {
    Matrixd<Rows, Rows> discR = DiscretizeR<Rows>(R, m_dt);
    Eigen::internal::llt_inplace<double, Eigen::Lower>::blocked(discR);

    // Transform sigma points into measurement space
    Matrixd<Rows, 2 * States + 1> sigmasH;
    Matrixd<States, 2 * States + 1> sigmas =
        m_pts.SquareRootSigmaPoints(m_xHat, m_S);
    for (int i = 0; i < m_pts.NumSigmas(); ++i) {
      sigmasH.template block<Rows, 1>(0, i) =
          h(sigmas.template block<States, 1>(0, i), u);
    }

    // Mean and covariance of prediction passed through UT
    auto [yHat, Sy] = SquareRootUnscentedTransform<Rows, States>(
        sigmasH, m_pts.Wm(), m_pts.Wc(), meanFuncY, residualFuncY,
        discR.template triangularView<Eigen::Lower>());

    // Compute cross covariance of the state and the measurements
    Matrixd<States, Rows> Pxy;
    Pxy.setZero();
    for (int i = 0; i < m_pts.NumSigmas(); ++i) {
      // Pxy += (sigmas_f[:, i] - x̂)(sigmas_h[:, i] - ŷ)ᵀ W_c[i]
      Pxy +=
          m_pts.Wc(i) *
          (residualFuncX(m_sigmasF.template block<States, 1>(0, i), m_xHat)) *
          (residualFuncY(sigmasH.template block<Rows, 1>(0, i), yHat))
              .transpose();
    }

    // K = (P_{xy} / S_yᵀ) / S_y
    // K = (S_y \ P_{xy}ᵀ)ᵀ / S_y
    // K = (S_yᵀ \ (S_y \ P_{xy}ᵀ))ᵀ
    Matrixd<States, Rows> K =
        Sy.transpose()
            .fullPivHouseholderQr()
            .solve(Sy.fullPivHouseholderQr().solve(Pxy.transpose()))
            .transpose();

    // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + K(y − ŷ)
    m_xHat = addFuncX(m_xHat, K * residualFuncY(y, yHat));

    Matrixd<States, Rows> U = K * Sy;
    for (int i = 0; i < Rows; i++) {
      Eigen::internal::llt_inplace<double, Eigen::Upper>::rankUpdate(
          m_S, U.template block<States, 1>(0, i), -1);
    }
  }

 private:
  std::function<StateVector(const StateVector&, const InputVector&)> m_f;
  std::function<OutputVector(const StateVector&, const InputVector&)> m_h;
  std::function<StateVector(const Matrixd<States, 2 * States + 1>&,
                            const Vectord<2 * States + 1>&)>
      m_meanFuncX;
  std::function<OutputVector(const Matrixd<Outputs, 2 * States + 1>&,
                             const Vectord<2 * States + 1>&)>
      m_meanFuncY;
  std::function<StateVector(const StateVector&, const StateVector&)>
      m_residualFuncX;
  std::function<OutputVector(const OutputVector&, const OutputVector&)>
      m_residualFuncY;
  std::function<StateVector(const StateVector&, const StateVector&)> m_addFuncX;
  StateVector m_xHat;
  StateMatrix m_S;
  StateMatrix m_contQ;
  Matrixd<Outputs, Outputs> m_contR;
  Matrixd<States, 2 * States + 1> m_sigmasF;
  units::second_t m_dt;

  MerweScaledSigmaPoints<States> m_pts;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    UnscentedKalmanFilter<3, 3, 1>;
extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    UnscentedKalmanFilter<5, 3, 3>;

}  // namespace frc
