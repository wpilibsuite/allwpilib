// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Eigen/Cholesky"
#include "drake/math/discrete_algebraic_riccati_equation.h"
#include "frc/StateSpaceUtil.h"
#include "frc/estimator/ExtendedKalmanFilter.h"
#include "frc/system/Discretization.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/NumericalJacobian.h"

namespace frc {

template <int States, int Inputs, int Outputs>
ExtendedKalmanFilter<States, Inputs, Outputs>::ExtendedKalmanFilter(
    std::function<
        Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
                                      const Eigen::Vector<double, Inputs>&)>
        f,
    std::function<
        Eigen::Vector<double, Outputs>(const Eigen::Vector<double, States>&,
                                       const Eigen::Vector<double, Inputs>&)>
        h,
    const wpi::array<double, States>& stateStdDevs,
    const wpi::array<double, Outputs>& measurementStdDevs, units::second_t dt)
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

template <int States, int Inputs, int Outputs>
ExtendedKalmanFilter<States, Inputs, Outputs>::ExtendedKalmanFilter(
    std::function<
        Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
                                      const Eigen::Vector<double, Inputs>&)>
        f,
    std::function<
        Eigen::Vector<double, Outputs>(const Eigen::Vector<double, States>&,
                                       const Eigen::Vector<double, Inputs>&)>
        h,
    const wpi::array<double, States>& stateStdDevs,
    const wpi::array<double, Outputs>& measurementStdDevs,
    std::function<
        Eigen::Vector<double, Outputs>(const Eigen::Vector<double, Outputs>&,
                                       const Eigen::Vector<double, Outputs>&)>
        residualFuncY,
    std::function<
        Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
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

template <int States, int Inputs, int Outputs>
void ExtendedKalmanFilter<States, Inputs, Outputs>::Predict(
    const Eigen::Vector<double, Inputs>& u, units::second_t dt) {
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

template <int States, int Inputs, int Outputs>
template <int Rows>
void ExtendedKalmanFilter<States, Inputs, Outputs>::Correct(
    const Eigen::Vector<double, Inputs>& u,
    const Eigen::Vector<double, Rows>& y,
    std::function<
        Eigen::Vector<double, Rows>(const Eigen::Vector<double, States>&,
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

template <int States, int Inputs, int Outputs>
template <int Rows>
void ExtendedKalmanFilter<States, Inputs, Outputs>::Correct(
    const Eigen::Vector<double, Inputs>& u,
    const Eigen::Vector<double, Rows>& y,
    std::function<
        Eigen::Vector<double, Rows>(const Eigen::Vector<double, States>&,
                                    const Eigen::Vector<double, Inputs>&)>
        h,
    const Eigen::Matrix<double, Rows, Rows>& R,
    std::function<Eigen::Vector<double, Rows>(
        const Eigen::Vector<double, Rows>&, const Eigen::Vector<double, Rows>&)>
        residualFuncY,
    std::function<
        Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
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

  // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + Kₖ₊₁(y − h(x̂ₖ₊₁⁻, uₖ₊₁))
  m_xHat = addFuncX(m_xHat, K * residualFuncY(y, h(m_xHat, u)));

  // Pₖ₊₁⁺ = (I−Kₖ₊₁C)Pₖ₊₁⁻(I−Kₖ₊₁C)ᵀ + Kₖ₊₁RKₖ₊₁ᵀ
  // Use Joseph form for numerical stability
  m_P = (Eigen::Matrix<double, States, States>::Identity() - K * C) * m_P *
            (Eigen::Matrix<double, States, States>::Identity() - K * C)
                .transpose() +
        K * discR * K.transpose();
}

}  // namespace frc
