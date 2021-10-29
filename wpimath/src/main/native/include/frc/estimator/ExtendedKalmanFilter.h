// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <wpi/array.h>

#include "Eigen/Cholesky"
#include "Eigen/Core"
#include "drake/math/discrete_algebraic_riccati_equation.h"
#include "frc/StateSpaceUtil.h"
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
 * An extended Kalman filter supports nonlinear state and measurement models. It
 * propagates the error covariance by linearizing the models around the state
 * estimate, then applying the linear Kalman filter equations.
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf chapter 9
 * "Stochastic control theory".
 *
 * @tparam States The number of states.
 * @tparam Inputs The number of inputs.
 * @tparam Outputs The number of outputs.
 */
template <int States, int Inputs, int Outputs>
class ExtendedKalmanFilter {
 public:
  /**
   * Constructs an extended Kalman filter.
   *
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dt                 Nominal discretization timestep.
   */
  ExtendedKalmanFilter(std::function<Eigen::Vector<double, States>(
                           const Eigen::Vector<double, States>&,
                           const Eigen::Vector<double, Inputs>&)>
                           f,
                       std::function<Eigen::Vector<double, Outputs>(
                           const Eigen::Vector<double, States>&,
                           const Eigen::Vector<double, Inputs>&)>
                           h,
                       const wpi::array<double, States>& stateStdDevs,
                       const wpi::array<double, Outputs>& measurementStdDevs,
                       units::second_t dt)
      : m_f(f), m_h(h) {
    m_contQ = MakeCovMatrix(stateStdDevs);
    m_contR = MakeCovMatrix(measurementStdDevs);
    m_residualFuncY = [](auto a, auto b) -> Eigen::Vector<double, Outputs> {
      return a - b;
    };
    m_addFuncX = [](auto a, auto b) -> Eigen::Vector<double, States> {
      return a + b;
    };
    m_dt = dt;

    Reset();

    Eigen::Matrix<double, States, States> contA =
        NumericalJacobianX<States, States, Inputs>(
            m_f, m_xHat, Eigen::Vector<double, Inputs>::Zero());
    Eigen::Matrix<double, Outputs, States> C =
        NumericalJacobianX<Outputs, States, Inputs>(
            m_h, m_xHat, Eigen::Vector<double, Inputs>::Zero());

    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, States> discQ;
    DiscretizeAQTaylor<States>(contA, m_contQ, dt, &discA, &discQ);

    Eigen::Matrix<double, Outputs, Outputs> discR =
        DiscretizeR<Outputs>(m_contR, dt);

    if (IsDetectable<States, Outputs>(discA, C) && Outputs <= States) {
      m_initP = drake::math::DiscreteAlgebraicRiccatiEquation(
          discA.transpose(), C.transpose(), discQ, discR);
    } else {
      m_initP = Eigen::Matrix<double, States, States>::Zero();
    }
    m_P = m_initP;
  }

  /**
   * Constructs an extended Kalman filter.
   *
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param residualFuncY      A function that computes the residual of two
   *                           measurement vectors (i.e. it subtracts them.)
   * @param addFuncX           A function that adds two state vectors.
   * @param dt                 Nominal discretization timestep.
   */
  ExtendedKalmanFilter(std::function<Eigen::Vector<double, States>(
                           const Eigen::Vector<double, States>&,
                           const Eigen::Vector<double, Inputs>&)>
                           f,
                       std::function<Eigen::Vector<double, Outputs>(
                           const Eigen::Vector<double, States>&,
                           const Eigen::Vector<double, Inputs>&)>
                           h,
                       const wpi::array<double, States>& stateStdDevs,
                       const wpi::array<double, Outputs>& measurementStdDevs,
                       std::function<Eigen::Vector<double, Outputs>(
                           const Eigen::Vector<double, Outputs>&,
                           const Eigen::Vector<double, Outputs>&)>
                           residualFuncY,
                       std::function<Eigen::Vector<double, States>(
                           const Eigen::Vector<double, States>&,
                           const Eigen::Vector<double, States>&)>
                           addFuncX,
                       units::second_t dt)
      : m_f(f), m_h(h), m_residualFuncY(residualFuncY), m_addFuncX(addFuncX) {
    m_contQ = MakeCovMatrix(stateStdDevs);
    m_contR = MakeCovMatrix(measurementStdDevs);
    m_dt = dt;

    Reset();

    Eigen::Matrix<double, States, States> contA =
        NumericalJacobianX<States, States, Inputs>(
            m_f, m_xHat, Eigen::Vector<double, Inputs>::Zero());
    Eigen::Matrix<double, Outputs, States> C =
        NumericalJacobianX<Outputs, States, Inputs>(
            m_h, m_xHat, Eigen::Vector<double, Inputs>::Zero());

    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, States> discQ;
    DiscretizeAQTaylor<States>(contA, m_contQ, dt, &discA, &discQ);

    Eigen::Matrix<double, Outputs, Outputs> discR =
        DiscretizeR<Outputs>(m_contR, dt);

    if (IsDetectable<States, Outputs>(discA, C) && Outputs <= States) {
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
  const Eigen::Vector<double, States>& Xhat() const { return m_xHat; }

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
  void SetXhat(const Eigen::Vector<double, States>& xHat) { m_xHat = xHat; }

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
  void Predict(const Eigen::Vector<double, Inputs>& u, units::second_t dt) {
    // Find continuous A
    Eigen::Matrix<double, States, States> contA =
        NumericalJacobianX<States, States, Inputs>(m_f, m_xHat, u);

    // Find discrete A and Q
    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, States> discQ;
    DiscretizeAQTaylor<States>(contA, m_contQ, dt, &discA, &discQ);

    m_xHat = RK4(m_f, m_xHat, u, dt);

    // Pₖ₊₁⁻ = APₖ⁻Aᵀ + Q
    m_P = discA * m_P * discA.transpose() + discQ;

    m_dt = dt;
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  void Correct(const Eigen::Vector<double, Inputs>& u,
               const Eigen::Vector<double, Outputs>& y) {
    Correct<Outputs>(u, y, m_h, m_contR, m_residualFuncY, m_addFuncX);
  }

  template <int Rows>
  void Correct(const Eigen::Vector<double, Inputs>& u,
               const Eigen::Vector<double, Rows>& y,
               std::function<Eigen::Vector<double, Rows>(
                   const Eigen::Vector<double, States>&,
                   const Eigen::Vector<double, Inputs>&)>
                   h,
               const Eigen::Matrix<double, Rows, Rows>& R) {
    auto residualFuncY = [](auto a, auto b) -> Eigen::Vector<double, Rows> {
      return a - b;
    };
    auto addFuncX = [](auto a, auto b) -> Eigen::Vector<double, States> {
      return a + b;
    };
    Correct<Rows>(u, y, h, R, residualFuncY, addFuncX);
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
   * @param R             Discrete measurement noise covariance matrix.
   * @param residualFuncY A function that computes the residual of two
   *                      measurement vectors (i.e. it subtracts them.)
   * @param addFuncX      A function that adds two state vectors.
   */
  template <int Rows>
  void Correct(const Eigen::Vector<double, Inputs>& u,
               const Eigen::Vector<double, Rows>& y,
               std::function<Eigen::Vector<double, Rows>(
                   const Eigen::Vector<double, States>&,
                   const Eigen::Vector<double, Inputs>&)>
                   h,
               const Eigen::Matrix<double, Rows, Rows>& R,
               std::function<Eigen::Vector<double, Rows>(
                   const Eigen::Vector<double, Rows>&,
                   const Eigen::Vector<double, Rows>&)>
                   residualFuncY,
               std::function<Eigen::Vector<double, States>(
                   const Eigen::Vector<double, States>&,
                   const Eigen::Vector<double, States>)>
                   addFuncX) {
    const Eigen::Matrix<double, Rows, States> C =
        NumericalJacobianX<Rows, States, Inputs>(h, m_xHat, u);
    const Eigen::Matrix<double, Rows, Rows> discR = DiscretizeR<Rows>(R, m_dt);

    Eigen::Matrix<double, Rows, Rows> S = C * m_P * C.transpose() + discR;

    // We want to put K = PCᵀS⁻¹ into Ax = b form so we can solve it more
    // efficiently.
    //
    // K = PCᵀS⁻¹
    // KS = PCᵀ
    // (KS)ᵀ = (PCᵀ)ᵀ
    // SᵀKᵀ = CPᵀ
    //
    // The solution of Ax = b can be found via x = A.solve(b).
    //
    // Kᵀ = Sᵀ.solve(CPᵀ)
    // K = (Sᵀ.solve(CPᵀ))ᵀ
    Eigen::Matrix<double, States, Rows> K =
        S.transpose().ldlt().solve(C * m_P.transpose()).transpose();

    // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + K(y − h(x̂ₖ₊₁⁻, uₖ₊₁))
    m_xHat = addFuncX(m_xHat, K * residualFuncY(y, h(m_xHat, u)));

    // Pₖ₊₁⁺ = (I − KC)Pₖ₊₁⁻
    m_P = (Eigen::Matrix<double, States, States>::Identity() - K * C) * m_P;
  }

 private:
  std::function<Eigen::Vector<double, States>(
      const Eigen::Vector<double, States>&,
      const Eigen::Vector<double, Inputs>&)>
      m_f;
  std::function<Eigen::Vector<double, Outputs>(
      const Eigen::Vector<double, States>&,
      const Eigen::Vector<double, Inputs>&)>
      m_h;
  std::function<Eigen::Vector<double, Outputs>(
      const Eigen::Vector<double, Outputs>&,
      const Eigen::Vector<double, Outputs>)>
      m_residualFuncY;
  std::function<Eigen::Vector<double, States>(
      const Eigen::Vector<double, States>&,
      const Eigen::Vector<double, States>)>
      m_addFuncX;
  Eigen::Vector<double, States> m_xHat;
  Eigen::Matrix<double, States, States> m_P;
  Eigen::Matrix<double, States, States> m_contQ;
  Eigen::Matrix<double, Outputs, Outputs> m_contR;
  units::second_t m_dt;

  Eigen::Matrix<double, States, States> m_initP;
};

}  // namespace frc
