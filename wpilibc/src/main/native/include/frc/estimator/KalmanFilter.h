/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>
#include <cmath>

#include <Eigen/Core>
#include <drake/math/discrete_algebraic_riccati_equation.h>
#include <units/units.h>

#include "frc/StateSpaceUtil.h"
#include "frc/system/Discretization.h"
#include "frc/system/LinearSystem.h"

namespace frc {

/**
 * Luenberger observers combine predictions from a model and measurements to
 * give an estimate of the true system state.
 *
 * Luenberger observers use an L gain matrix to determine whether to trust the
 * model or measurements more. Kalman filter theory uses statistics to compute
 * an optimal L gain (alternatively called the Kalman gain, K) which minimizes
 * the sum of squares error in the state estimate.
 *
 * Luenberger observers run the prediction and correction steps simultaneously
 * while Kalman filters run them sequentially. To implement a discrete-time
 * Kalman filter as a Luenberger observer, use the following mapping:
 * <pre>C = H, L = A * K</pre>
 * (H is the measurement matrix).
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
 */
template <int States, int Inputs, int Outputs>
class KalmanFilter {
 public:
  /**
   * Constructs a state-space observer with the given plant.
   *
   * @param plant              The plant used for the prediction step.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dt                 Nominal discretization timestep.
   */
  KalmanFilter(LinearSystem<States, Inputs, Outputs>& plant,
               const std::array<double, States>& stateStdDevs,
               const std::array<double, Outputs>& measurementStdDevs,
               units::second_t dt) {
    m_plant = &plant;

    m_contQ = MakeCovMatrix(stateStdDevs);
    m_contR = MakeCovMatrix(measurementStdDevs);

    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, States> discQ;
    DiscretizeAQTaylor<States>(plant.A(), m_contQ, dt, &discA, &discQ);

    m_discR = DiscretizeR<Outputs>(m_contR, dt);

    if (IsStabilizable<States, Outputs>(discA.transpose(),
                                        plant.C().transpose()) &&
        Outputs <= States) {
      m_P = drake::math::DiscreteAlgebraicRiccatiEquation(
          discA.transpose(), plant.C().transpose(), discQ, m_discR);
    } else {
      m_P = Eigen::Matrix<double, States, States>::Zero();
    }
  }

  KalmanFilter(KalmanFilter&&) = default;
  KalmanFilter& operator=(KalmanFilter&&) = default;

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
  const Eigen::Matrix<double, States, 1>& Xhat() const { return m_plant->X(); }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  double Xhat(int i) const { return m_plant->X(i); }

  /**
   * Set initial state estimate x-hat.
   *
   * @param xHat The state estimate x-hat.
   */
  void SetXhat(const Eigen::Matrix<double, States, 1>& xHat) {
    m_plant->SetX(xHat);
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param i     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  void SetXhat(int i, double value) { m_plant->SetX(i, value); }

  /**
   * Resets the observer.
   */
  void Reset() { m_plant->Reset(); }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u  New control input from controller.
   * @param dt Timestep for prediction.
   */
  void Predict(const Eigen::Matrix<double, Inputs, 1>& u, units::second_t dt) {
    m_plant->SetX(m_plant->CalculateX(m_plant->X(), u, dt));

    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, States> discQ;
    DiscretizeAQTaylor<States>(m_plant->A(), m_contQ, dt, &discA, &discQ);

    m_P = discA * m_P * discA.transpose() + discQ;
    m_discR = DiscretizeR<Outputs>(m_contR, dt);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the last predict step.
   * @param y Measurement vector.
   */
  void Correct(const Eigen::Matrix<double, Inputs, 1>& u,
               const Eigen::Matrix<double, Outputs, 1>& y) {
    Correct<Outputs>(u, y, m_plant->C(), m_discR);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * This is useful for when the measurements available during a timestep's
   * Correct() call vary. The C matrix passed to the constructor is used if one
   * is not provided (the two-argument version of this function).
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param C Output matrix.
   * @param R Measurement noise covariance matrix.
   */
  template <int Rows>
  void Correct(const Eigen::Matrix<double, Inputs, 1>& u,
               const Eigen::Matrix<double, Rows, 1>& y,
               const Eigen::Matrix<double, Rows, States>& C,
               const Eigen::Matrix<double, Rows, Rows>& R) {
    const auto& x = m_plant->X();
    Eigen::Matrix<double, Rows, Rows> S = C * m_P * C.transpose() + R;

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

    m_plant->SetX(x + K * (y - (C * x + m_plant->D() * u)));
    m_P = (Eigen::Matrix<double, States, States>::Identity() - K * C) * m_P;
  }

 private:
  LinearSystem<States, Inputs, Outputs>* m_plant;

  /**
   * Error covariance matrix.
   */
  Eigen::Matrix<double, States, States> m_P;

  /**
   * Continuous process noise covariance matrix.
   */
  Eigen::Matrix<double, States, States> m_contQ;

  /**
   * Continuous measurement noise covariance matrix.
   */
  Eigen::Matrix<double, Outputs, Outputs> m_contR;

  /**
   * Discrete measurement noise covariance matrix.
   */
  Eigen::Matrix<double, Outputs, Outputs> m_discR;
};

}  // namespace frc
