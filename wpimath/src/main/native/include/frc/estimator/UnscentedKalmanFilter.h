// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <wpi/array.h>

#include "Eigen/Core"
#include "Eigen/src/Cholesky/LDLT.h"
#include "frc/StateSpaceUtil.h"
#include "frc/estimator/MerweScaledSigmaPoints.h"
#include "frc/estimator/UnscentedTransform.h"
#include "frc/system/Discretization.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/NumericalJacobian.h"
#include "units/time.h"

namespace frc {

template <int States, int Inputs, int Outputs>
class UnscentedKalmanFilter {
 public:
  /**
   * Constructs an unscented Kalman filter.
   *
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dt                 Nominal discretization timestep.
   */
  UnscentedKalmanFilter(std::function<Eigen::Matrix<double, States, 1>(
                            const Eigen::Matrix<double, States, 1>&,
                            const Eigen::Matrix<double, Inputs, 1>&)>
                            f,
                        std::function<Eigen::Matrix<double, Outputs, 1>(
                            const Eigen::Matrix<double, States, 1>&,
                            const Eigen::Matrix<double, Inputs, 1>&)>
                            h,
                        const wpi::array<double, States>& stateStdDevs,
                        const wpi::array<double, Outputs>& measurementStdDevs,
                        units::second_t dt)
      : m_f(f), m_h(h) {
    m_contQ = MakeCovMatrix(stateStdDevs);
    m_contR = MakeCovMatrix(measurementStdDevs);
    m_meanFuncX = [](auto sigmas, auto Wm) -> Eigen::Matrix<double, States, 1> {
      return sigmas * Wm;
    };
    m_meanFuncY = [](auto sigmas,
                     auto Wc) -> Eigen::Matrix<double, Outputs, 1> {
      return sigmas * Wc;
    };
    m_residualFuncX = [](auto a, auto b) -> Eigen::Matrix<double, States, 1> {
      return a - b;
    };
    m_residualFuncY = [](auto a, auto b) -> Eigen::Matrix<double, Outputs, 1> {
      return a - b;
    };
    m_addFuncX = [](auto a, auto b) -> Eigen::Matrix<double, States, 1> {
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
      std::function<Eigen::Matrix<double, States, 1>(
          const Eigen::Matrix<double, States, 1>&,
          const Eigen::Matrix<double, Inputs, 1>&)>
          f,
      std::function<Eigen::Matrix<double, Outputs, 1>(
          const Eigen::Matrix<double, States, 1>&,
          const Eigen::Matrix<double, Inputs, 1>&)>
          h,
      const wpi::array<double, States>& stateStdDevs,
      const wpi::array<double, Outputs>& measurementStdDevs,
      std::function<Eigen::Matrix<double, States, 1>(
          const Eigen::Matrix<double, States, 2 * States + 1>&,
          const Eigen::Matrix<double, 2 * States + 1, 1>&)>
          meanFuncX,
      std::function<Eigen::Matrix<double, Outputs, 1>(
          const Eigen::Matrix<double, Outputs, 2 * States + 1>&,
          const Eigen::Matrix<double, 2 * States + 1, 1>&)>
          meanFuncY,
      std::function<Eigen::Matrix<double, States, 1>(
          const Eigen::Matrix<double, States, 1>&,
          const Eigen::Matrix<double, States, 1>&)>
          residualFuncX,
      std::function<Eigen::Matrix<double, Outputs, 1>(
          const Eigen::Matrix<double, Outputs, 1>&,
          const Eigen::Matrix<double, Outputs, 1>&)>
          residualFuncY,
      std::function<Eigen::Matrix<double, States, 1>(
          const Eigen::Matrix<double, States, 1>&,
          const Eigen::Matrix<double, States, 1>&)>
          addFuncX,
      units::second_t dt)
      : m_f(f),
        m_h(h),
        m_meanFuncX(meanFuncX),
        m_meanFuncY(meanFuncY),
        m_residualFuncX(residualFuncX),
        m_residualFuncY(residualFuncY),
        m_addFuncX(addFuncX) {
    m_contQ = MakeCovMatrix(stateStdDevs);
    m_contR = MakeCovMatrix(measurementStdDevs);
    m_dt = dt;

    Reset();
  }

  /**
   * Returns the error covariance matrix P.
   */
  const Eigen::Matrix<double, States, States>& P() const { return m_P; }

  /**
   * Returns an element of the error covariance matrix P.
   *
   * @param i Row of P.
   * @param j Column of P.
   */
  double P(int i, int j) const { return m_P(i, j); }

  /**
   * Set the current error covariance matrix P.
   *
   * @param P The error covariance matrix P.
   */
  void SetP(const Eigen::Matrix<double, States, States>& P) { m_P = P; }

  /**
   * Returns the state estimate x-hat.
   */
  const Eigen::Matrix<double, States, 1>& Xhat() const { return m_xHat; }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  double Xhat(int i) const { return m_xHat(i, 0); }

  /**
   * Set initial state estimate x-hat.
   *
   * @param xHat The state estimate x-hat.
   */
  void SetXhat(const Eigen::Matrix<double, States, 1>& xHat) { m_xHat = xHat; }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param i     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  void SetXhat(int i, double value) { m_xHat(i, 0) = value; }

  /**
   * Resets the observer.
   */
  void Reset() {
    m_xHat.setZero();
    m_P.setZero();
    m_sigmasF.setZero();
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u  New control input from controller.
   * @param dt Timestep for prediction.
   */
  void Predict(const Eigen::Matrix<double, Inputs, 1>& u, units::second_t dt) {
    m_dt = dt;

    // Discretize Q before projecting mean and covariance forward
    Eigen::Matrix<double, States, States> contA =
        NumericalJacobianX<States, States, Inputs>(m_f, m_xHat, u);
    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, States> discQ;
    DiscretizeAQTaylor<States>(contA, m_contQ, dt, &discA, &discQ);

    Eigen::Matrix<double, States, 2 * States + 1> sigmas =
        m_pts.SigmaPoints(m_xHat, m_P);

    for (int i = 0; i < m_pts.NumSigmas(); ++i) {
      Eigen::Matrix<double, States, 1> x =
          sigmas.template block<States, 1>(0, i);
      m_sigmasF.template block<States, 1>(0, i) = RK4(m_f, x, u, dt);
    }

    auto ret = UnscentedTransform<States, States>(
        m_sigmasF, m_pts.Wm(), m_pts.Wc(), m_meanFuncX, m_residualFuncX);
    m_xHat = std::get<0>(ret);
    m_P = std::get<1>(ret);

    m_P += discQ;
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  void Correct(const Eigen::Matrix<double, Inputs, 1>& u,
               const Eigen::Matrix<double, Outputs, 1>& y) {
    Correct<Outputs>(u, y, m_h, m_contR, m_meanFuncY, m_residualFuncY,
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
   * @param R Measurement noise covariance matrix.
   */
  template <int Rows>
  void Correct(const Eigen::Matrix<double, Inputs, 1>& u,
               const Eigen::Matrix<double, Rows, 1>& y,
               std::function<Eigen::Matrix<double, Rows, 1>(
                   const Eigen::Matrix<double, States, 1>&,
                   const Eigen::Matrix<double, Inputs, 1>&)>
                   h,
               const Eigen::Matrix<double, Rows, Rows>& R) {
    auto meanFuncY = [](auto sigmas,
                        auto Wc) -> Eigen::Matrix<double, Rows, 1> {
      return sigmas * Wc;
    };
    auto residualFuncX = [](auto a,
                            auto b) -> Eigen::Matrix<double, States, 1> {
      return a - b;
    };
    auto residualFuncY = [](auto a, auto b) -> Eigen::Matrix<double, Rows, 1> {
      return a - b;
    };
    auto addFuncX = [](auto a, auto b) -> Eigen::Matrix<double, States, 1> {
      return a + b;
    };
    Correct<Rows>(u, y, h, R, meanFuncY, residualFuncY, residualFuncX,
                  addFuncX);
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
   * @param R             Measurement noise covariance matrix.
   * @param meanFuncY     A function that computes the mean of 2 * States + 1
   *                      measurement vectors using a given set of weights.
   * @param residualFuncX A function that computes the residual of two state
   *                      vectors (i.e. it subtracts them.)
   * @param residualFuncY A function that computes the residual of two
   *                      measurement vectors (i.e. it subtracts them.)
   * @param addFuncX      A function that adds two state vectors.
   */
  template <int Rows>
  void Correct(const Eigen::Matrix<double, Inputs, 1>& u,
               const Eigen::Matrix<double, Rows, 1>& y,
               std::function<Eigen::Matrix<double, Rows, 1>(
                   const Eigen::Matrix<double, States, 1>&,
                   const Eigen::Matrix<double, Inputs, 1>&)>
                   h,
               const Eigen::Matrix<double, Rows, Rows>& R,
               std::function<Eigen::Matrix<double, Rows, 1>(
                   const Eigen::Matrix<double, Rows, 2 * States + 1>&,
                   const Eigen::Matrix<double, 2 * States + 1, 1>&)>
                   meanFuncY,
               std::function<Eigen::Matrix<double, Rows, 1>(
                   const Eigen::Matrix<double, Rows, 1>&,
                   const Eigen::Matrix<double, Rows, 1>&)>
                   residualFuncY,
               std::function<Eigen::Matrix<double, States, 1>(
                   const Eigen::Matrix<double, States, 1>&,
                   const Eigen::Matrix<double, States, 1>&)>
                   residualFuncX,
               std::function<Eigen::Matrix<double, States, 1>(
                   const Eigen::Matrix<double, States, 1>&,
                   const Eigen::Matrix<double, States, 1>)>
                   addFuncX) {
    const Eigen::Matrix<double, Rows, Rows> discR = DiscretizeR<Rows>(R, m_dt);

    // Transform sigma points into measurement space
    Eigen::Matrix<double, Rows, 2 * States + 1> sigmasH;
    Eigen::Matrix<double, States, 2 * States + 1> sigmas =
        m_pts.SigmaPoints(m_xHat, m_P);
    for (int i = 0; i < m_pts.NumSigmas(); ++i) {
      sigmasH.template block<Rows, 1>(0, i) =
          h(sigmas.template block<States, 1>(0, i), u);
    }

    // Mean and covariance of prediction passed through UT
    auto [yHat, Py] = UnscentedTransform<States, Rows>(
        sigmasH, m_pts.Wm(), m_pts.Wc(), meanFuncY, residualFuncY);
    Py += discR;

    // Compute cross covariance of the state and the measurements
    Eigen::Matrix<double, States, Rows> Pxy;
    Pxy.setZero();
    for (int i = 0; i < m_pts.NumSigmas(); ++i) {
      Pxy +=
          m_pts.Wc(i) *
          (residualFuncX(m_sigmasF.template block<States, 1>(0, i), m_xHat)) *
          (residualFuncY(sigmasH.template block<Rows, 1>(0, i), yHat))
              .transpose();
    }

    // K = P_{xy} Py^-1
    // K^T = P_y^T^-1 P_{xy}^T
    // P_y^T K^T = P_{xy}^T
    // K^T = P_y^T.solve(P_{xy}^T)
    // K = (P_y^T.solve(P_{xy}^T)^T
    Eigen::Matrix<double, States, Rows> K =
        Py.transpose().ldlt().solve(Pxy.transpose()).transpose();

    m_xHat = addFuncX(m_xHat, K * residualFuncY(y, yHat));
    m_P -= K * Py * K.transpose();
  }

 private:
  std::function<Eigen::Matrix<double, States, 1>(
      const Eigen::Matrix<double, States, 1>&,
      const Eigen::Matrix<double, Inputs, 1>&)>
      m_f;
  std::function<Eigen::Matrix<double, Outputs, 1>(
      const Eigen::Matrix<double, States, 1>&,
      const Eigen::Matrix<double, Inputs, 1>&)>
      m_h;
  std::function<Eigen::Matrix<double, States, 1>(
      const Eigen::Matrix<double, States, 2 * States + 1>&,
      const Eigen::Matrix<double, 2 * States + 1, 1>&)>
      m_meanFuncX;
  std::function<Eigen::Matrix<double, Outputs, 1>(
      const Eigen::Matrix<double, Outputs, 2 * States + 1>&,
      const Eigen::Matrix<double, 2 * States + 1, 1>&)>
      m_meanFuncY;
  std::function<Eigen::Matrix<double, States, 1>(
      const Eigen::Matrix<double, States, 1>&,
      const Eigen::Matrix<double, States, 1>&)>
      m_residualFuncX;
  std::function<Eigen::Matrix<double, Outputs, 1>(
      const Eigen::Matrix<double, Outputs, 1>&,
      const Eigen::Matrix<double, Outputs, 1>)>
      m_residualFuncY;
  std::function<Eigen::Matrix<double, States, 1>(
      const Eigen::Matrix<double, States, 1>&,
      const Eigen::Matrix<double, States, 1>)>
      m_addFuncX;
  Eigen::Matrix<double, States, 1> m_xHat;
  Eigen::Matrix<double, States, States> m_P;
  Eigen::Matrix<double, States, States> m_contQ;
  Eigen::Matrix<double, Outputs, Outputs> m_contR;
  Eigen::Matrix<double, States, 2 * States + 1> m_sigmasF;
  units::second_t m_dt;

  MerweScaledSigmaPoints<States> m_pts;
};

}  // namespace frc
