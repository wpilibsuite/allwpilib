/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>
#include <functional>

#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <units/units.h>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/MerweScaledSigmaPoints.h"
#include "frc/estimator/UnscentedTransform.h"
#include "frc/system/RungeKutta.h"

namespace frc {

template <int N>
using Vector = Eigen::Matrix<double, N, 1>;

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
   */
  UnscentedKalmanFilter(std::function<Vector<States>(const Vector<States>&,
                                                     const Vector<Inputs>&)>
                            f,
                        std::function<Vector<Outputs>(const Vector<States>&,
                                                      const Vector<Inputs>&)>
                            h,
                        const std::array<double, States>& stateStdDevs,
                        const std::array<double, Outputs>& measurementStdDevs)
      : m_f(f), m_h(h) {
    m_Q = MakeCovMatrix(stateStdDevs);
    m_R = MakeCovMatrix(measurementStdDevs);

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
    auto sigmas = m_pts.SigmaPoints(m_xHat, m_P);

    for (int i = 0; i < m_pts.NumSigmas(); ++i) {
      Eigen::Matrix<double, States, 1> x =
          sigmas.template block<1, States>(i, 0).transpose();
      m_sigmasF.template block<1, States>(i, 0) =
          RungeKutta(m_f, x, u, dt).transpose();
    }

    auto ret = UnscentedTransform<States, States>(m_sigmasF, m_pts.Wm(),
                                                  m_pts.Wc(), m_Q);
    m_xHat = std::get<0>(ret);
    m_P = std::get<1>(ret);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  void Correct(const Eigen::Matrix<double, Inputs, 1>& u,
               const Eigen::Matrix<double, Outputs, 1>& y) {
    Correct(u, y, m_h, m_R);
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
   * @param h A vector-valued function of x and u that returns
   *          the measurement vector.
   * @param R Measurement noise covariance matrix.
   */
  template <int Rows>
  void Correct(
      const Eigen::Matrix<double, Inputs, 1>& u,
      const Eigen::Matrix<double, Rows, 1>& y,
      std::function<Vector<Rows>(const Vector<States>&, const Vector<Inputs>&)>
          h,
      const Eigen::Matrix<double, Rows, Rows>& R) {
    // Transform sigma points into measurement space
    Eigen::Matrix<double, 2 * States + 1, Rows> sigmasH;
    for (int i = 0; i < m_pts.NumSigmas(); ++i) {
      sigmasH.template block<1, Rows>(i, 0) =
          h(m_sigmasF.template block<1, States>(i, 0).transpose(), u);
    }

    // Mean and covariance of prediction passed through UT
    auto [yHat, Py] =
        UnscentedTransform<States, Rows>(sigmasH, m_pts.Wm(), m_pts.Wc(), R);

    // Compute cross covariance of the state and the measurements
    Eigen::Matrix<double, States, Rows> Pxy;
    Pxy.setZero();
    for (int i = 0; i < m_pts.NumSigmas(); ++i) {
      Pxy += m_pts.Wc(i) *
             (m_sigmasF.template block<1, States>(i, 0) - m_xHat.transpose())
                 .transpose() *
             (sigmasH.template block<1, Rows>(i, 0) - yHat.transpose());
    }

    // K = P_{xy} Py^-1
    // K^T = P_y^T^-1 P_{xy}^T
    // P_y^T K^T = P_{xy}^T
    // K^T = P_y^T.solve(P_{xy}^T)
    // K = (P_y^T.solve(P_{xy}^T)^T
    Eigen::Matrix<double, States, Rows> K =
        Py.transpose().ldlt().solve(Pxy.transpose()).transpose();

    m_xHat += K * (y - yHat);
    m_P -= K * Py * K.transpose();
  }

 private:
  std::function<Vector<States>(const Vector<States>&, const Vector<Inputs>&)>
      m_f;
  std::function<Vector<Outputs>(const Vector<States>&, const Vector<Inputs>&)>
      m_h;
  Eigen::Matrix<double, States, 1> m_xHat;
  Eigen::Matrix<double, States, States> m_P;
  Eigen::Matrix<double, States, States> m_Q;
  Eigen::Matrix<double, Outputs, Outputs> m_R;
  Eigen::Matrix<double, 2 * States + 1, States> m_sigmasF;

  MerweScaledSigmaPoints<States> m_pts;
};

}  // namespace frc
