// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <wpi/array.h>

#include "Eigen/Core"
#include "Eigen/src/Cholesky/LDLT.h"
#include "drake/math/discrete_algebraic_riccati_equation.h"
#include "frc/StateSpaceUtil.h"
#include "frc/system/Discretization.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/NumericalJacobian.h"
#include "units/time.h"

namespace frc {

template <int States, int Inputs, int Outputs>
class ExtendedKalmanFilter {
 public:
  /**
   * Constructs an Extended Kalman filter.
   *
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dt                 Nominal discretization timestep.
   */
  ExtendedKalmanFilter(std::function<Eigen::Matrix<double, States, 1>(
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
    m_dt = dt;

    Reset();

    Eigen::Matrix<double, States, States> contA =
        NumericalJacobianX<States, States, Inputs>(
            m_f, m_xHat, Eigen::Matrix<double, Inputs, 1>::Zero());
    Eigen::Matrix<double, Outputs, States> C =
        NumericalJacobianX<Outputs, States, Inputs>(
            m_h, m_xHat, Eigen::Matrix<double, Inputs, 1>::Zero());

    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, States> discQ;
    DiscretizeAQTaylor<States>(contA, m_contQ, dt, &discA, &discQ);

    Eigen::Matrix<double, Outputs, Outputs> discR =
        DiscretizeR<Outputs>(m_contR, dt);

    // IsStabilizable(A^T, C^T) will tell us if the system is observable.
    bool isObservable =
        IsStabilizable<States, Outputs>(discA.transpose(), C.transpose());
    if (isObservable && Outputs <= States) {
      m_initP = drake::math::DiscreteAlgebraicRiccatiEquation(
          discA.transpose(), C.transpose(), discQ, discR);
    } else {
      m_initP = Eigen::Matrix<double, States, States>::Zero();
    }
    m_P = m_initP;
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
    m_P = m_initP;
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u  New control input from controller.
   * @param dt Timestep for prediction.
   */
  void Predict(const Eigen::Matrix<double, Inputs, 1>& u, units::second_t dt) {
    m_dt = dt;

    // Find continuous A
    Eigen::Matrix<double, States, States> contA =
        NumericalJacobianX<States, States, Inputs>(m_f, m_xHat, u);

    // Find discrete A and Q
    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, States> discQ;
    DiscretizeAQTaylor<States>(contA, m_contQ, dt, &discA, &discQ);

    m_xHat = RK4(m_f, m_xHat, u, dt);
    m_P = discA * m_P * discA.transpose() + discQ;
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  void Correct(const Eigen::Matrix<double, Inputs, 1>& u,
               const Eigen::Matrix<double, Outputs, 1>& y) {
    Correct<Outputs>(u, y, m_h, m_contR);
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
   * @param R Discrete measurement noise covariance matrix.
   */
  template <int Rows>
  void Correct(const Eigen::Matrix<double, Inputs, 1>& u,
               const Eigen::Matrix<double, Rows, 1>& y,
               std::function<Eigen::Matrix<double, Rows, 1>(
                   const Eigen::Matrix<double, States, 1>&,
                   const Eigen::Matrix<double, Inputs, 1>&)>
                   h,
               const Eigen::Matrix<double, Rows, Rows>& R) {
    const Eigen::Matrix<double, Rows, States> C =
        NumericalJacobianX<Rows, States, Inputs>(h, m_xHat, u);
    const Eigen::Matrix<double, Rows, Rows> discR = DiscretizeR<Rows>(R, m_dt);

    Eigen::Matrix<double, Rows, Rows> S = C * m_P * C.transpose() + discR;

    // We want to put K = PC^T S^-1 into Ax = b form so we can solve it more
    // efficiently.
    //
    // K = PC^T S^-1
    // KS = PC^T
    // (KS)^T = (PC^T)^T
    // S^T K^T = CP^T
    //
    // The solution of Ax = b can be found via x = A.solve(b).
    //
    // K^T = S^T.solve(CP^T)
    // K = (S^T.solve(CP^T))^T
    Eigen::Matrix<double, States, Rows> K =
        S.transpose().ldlt().solve(C * m_P.transpose()).transpose();

    m_xHat += K * (y - h(m_xHat, u));
    m_P = (Eigen::Matrix<double, States, States>::Identity() - K * C) * m_P;
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
  Eigen::Matrix<double, States, 1> m_xHat;
  Eigen::Matrix<double, States, States> m_P;
  Eigen::Matrix<double, States, States> m_contQ;
  Eigen::Matrix<double, Outputs, Outputs> m_contR;
  units::second_t m_dt;

  Eigen::Matrix<double, States, States> m_initP;
};

}  // namespace frc
