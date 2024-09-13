// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <Eigen/Cholesky>

#include "frc/DARE.h"
#include "frc/StateSpaceUtil.h"
#include "frc/estimator/ExtendedKalmanFilter.h"
#include "frc/system/Discretization.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/NumericalJacobian.h"

namespace frc {

template <int States, int Inputs, int Outputs>
ExtendedKalmanFilter<States, Inputs, Outputs>::ExtendedKalmanFilter(
    std::function<StateVector(const StateVector&, const InputVector&)> f,
    std::function<OutputVector(const StateVector&, const InputVector&)> h,
    const StateArray& stateStdDevs, const OutputArray& measurementStdDevs,
    units::second_t dt)
    : m_f(f), m_h(h) {
  m_contQ = MakeCovMatrix(stateStdDevs);
  m_contR = MakeCovMatrix(measurementStdDevs);
  m_residualFuncY = [](auto a, auto b) -> OutputVector { return a - b; };
  m_addFuncX = [](auto a, auto b) -> StateVector { return a + b; };
  m_dt = dt;

  Reset();

  StateMatrix contA = NumericalJacobianX<States, States, Inputs>(
      m_f, m_xHat, InputVector::Zero());
  Matrixd<Outputs, States> C = NumericalJacobianX<Outputs, States, Inputs>(
      m_h, m_xHat, InputVector::Zero());

  StateMatrix discA;
  StateMatrix discQ;
  DiscretizeAQ<States>(contA, m_contQ, dt, &discA, &discQ);

  Matrixd<Outputs, Outputs> discR = DiscretizeR<Outputs>(m_contR, dt);

  if (IsDetectable<States, Outputs>(discA, C) && Outputs <= States) {
    m_initP =
        DARE<States, Outputs>(discA.transpose(), C.transpose(), discQ, discR);
  } else {
    m_initP = StateMatrix::Zero();
  }
  m_P = m_initP;
}

template <int States, int Inputs, int Outputs>
ExtendedKalmanFilter<States, Inputs, Outputs>::ExtendedKalmanFilter(
    std::function<StateVector(const StateVector&, const InputVector&)> f,
    std::function<OutputVector(const StateVector&, const InputVector&)> h,
    const StateArray& stateStdDevs, const OutputArray& measurementStdDevs,
    std::function<OutputVector(const OutputVector&, const OutputVector&)>
        residualFuncY,
    std::function<StateVector(const StateVector&, const StateVector&)> addFuncX,
    units::second_t dt)
    : m_f(f), m_h(h), m_residualFuncY(residualFuncY), m_addFuncX(addFuncX) {
  m_contQ = MakeCovMatrix(stateStdDevs);
  m_contR = MakeCovMatrix(measurementStdDevs);
  m_dt = dt;

  Reset();

  StateMatrix contA = NumericalJacobianX<States, States, Inputs>(
      m_f, m_xHat, InputVector::Zero());
  Matrixd<Outputs, States> C = NumericalJacobianX<Outputs, States, Inputs>(
      m_h, m_xHat, InputVector::Zero());

  StateMatrix discA;
  StateMatrix discQ;
  DiscretizeAQ<States>(contA, m_contQ, dt, &discA, &discQ);

  Matrixd<Outputs, Outputs> discR = DiscretizeR<Outputs>(m_contR, dt);

  if (IsDetectable<States, Outputs>(discA, C) && Outputs <= States) {
    m_initP =
        DARE<States, Outputs>(discA.transpose(), C.transpose(), discQ, discR);
  } else {
    m_initP = StateMatrix::Zero();
  }
  m_P = m_initP;
}

template <int States, int Inputs, int Outputs>
void ExtendedKalmanFilter<States, Inputs, Outputs>::Predict(
    const InputVector& u, units::second_t dt) {
  // Find continuous A
  StateMatrix contA =
      NumericalJacobianX<States, States, Inputs>(m_f, m_xHat, u);

  // Find discrete A and Q
  StateMatrix discA;
  StateMatrix discQ;
  DiscretizeAQ<States>(contA, m_contQ, dt, &discA, &discQ);

  m_xHat = RK4(m_f, m_xHat, u, dt);

  // Pₖ₊₁⁻ = APₖ⁻Aᵀ + Q
  m_P = discA * m_P * discA.transpose() + discQ;

  m_dt = dt;
}

template <int States, int Inputs, int Outputs>
template <int Rows>
void ExtendedKalmanFilter<States, Inputs, Outputs>::Correct(
    const InputVector& u, const Vectord<Rows>& y,
    std::function<Vectord<Rows>(const StateVector&, const InputVector&)> h,
    const Matrixd<Rows, Rows>& R) {
  auto residualFuncY = [](auto a, auto b) -> Vectord<Rows> { return a - b; };
  auto addFuncX = [](auto a, auto b) -> StateVector { return a + b; };
  Correct<Rows>(u, y, h, R, residualFuncY, addFuncX);
}

template <int States, int Inputs, int Outputs>
template <int Rows>
void ExtendedKalmanFilter<States, Inputs, Outputs>::Correct(
    const InputVector& u, const Vectord<Rows>& y,
    std::function<Vectord<Rows>(const StateVector&, const InputVector&)> h,
    const Matrixd<Rows, Rows>& R,
    std::function<Vectord<Rows>(const Vectord<Rows>&, const Vectord<Rows>&)>
        residualFuncY,
    std::function<StateVector(const StateVector&, const StateVector&)>
        addFuncX) {
  const Matrixd<Rows, States> C =
      NumericalJacobianX<Rows, States, Inputs>(h, m_xHat, u);
  const Matrixd<Rows, Rows> discR = DiscretizeR<Rows>(R, m_dt);

  Matrixd<Rows, Rows> S = C * m_P * C.transpose() + discR;

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
  Matrixd<States, Rows> K =
      S.transpose().ldlt().solve(C * m_P.transpose()).transpose();

  // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + Kₖ₊₁(y − h(x̂ₖ₊₁⁻, uₖ₊₁))
  m_xHat = addFuncX(m_xHat, K * residualFuncY(y, h(m_xHat, u)));

  // Pₖ₊₁⁺ = (I−Kₖ₊₁C)Pₖ₊₁⁻(I−Kₖ₊₁C)ᵀ + Kₖ₊₁RKₖ₊₁ᵀ
  // Use Joseph form for numerical stability
  m_P = (StateMatrix::Identity() - K * C) * m_P *
            (StateMatrix::Identity() - K * C).transpose() +
        K * discR * K.transpose();
}

}  // namespace frc
