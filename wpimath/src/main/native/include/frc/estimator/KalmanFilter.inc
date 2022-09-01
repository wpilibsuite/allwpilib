// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/fmt/Eigen.h>

#include <cmath>
#include <stdexcept>
#include <string>

#include "Eigen/Cholesky"
#include "drake/math/discrete_algebraic_riccati_equation.h"
#include "frc/StateSpaceUtil.h"
#include "frc/estimator/KalmanFilter.h"
#include "frc/system/Discretization.h"
#include "wpimath/MathShared.h"

namespace frc {

template <int States, int Inputs, int Outputs>
KalmanFilter<States, Inputs, Outputs>::KalmanFilter(
    LinearSystem<States, Inputs, Outputs>& plant,
    const StateArray& stateStdDevs, const OutputArray& measurementStdDevs,
    units::second_t dt) {
  m_plant = &plant;

  auto contQ = MakeCovMatrix(stateStdDevs);
  auto contR = MakeCovMatrix(measurementStdDevs);

  Matrixd<States, States> discA;
  Matrixd<States, States> discQ;
  DiscretizeAQTaylor<States>(plant.A(), contQ, dt, &discA, &discQ);

  auto discR = DiscretizeR<Outputs>(contR, dt);

  const auto& C = plant.C();

  if (!IsDetectable<States, Outputs>(discA, C)) {
    std::string msg = fmt::format(
        "The system passed to the Kalman filter is "
        "unobservable!\n\nA =\n{}\nC =\n{}\n",
        discA, C);

    wpi::math::MathSharedStore::ReportError(msg);
    throw std::invalid_argument(msg);
  }

  Matrixd<States, States> P = drake::math::DiscreteAlgebraicRiccatiEquation(
      discA.transpose(), C.transpose(), discQ, discR);

  // S = CPCᵀ + R
  Matrixd<Outputs, Outputs> S = C * P * C.transpose() + discR;

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
  m_K = S.transpose().ldlt().solve(C * P.transpose()).transpose();

  Reset();
}

template <int States, int Inputs, int Outputs>
void KalmanFilter<States, Inputs, Outputs>::Predict(const InputVector& u,
                                                    units::second_t dt) {
  m_xHat = m_plant->CalculateX(m_xHat, u, dt);
}

template <int States, int Inputs, int Outputs>
void KalmanFilter<States, Inputs, Outputs>::Correct(const InputVector& u,
                                                    const OutputVector& y) {
  // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + K(y − (Cx̂ₖ₊₁⁻ + Duₖ₊₁))
  m_xHat += m_K * (y - (m_plant->C() * m_xHat + m_plant->D() * u));
}

}  // namespace frc
