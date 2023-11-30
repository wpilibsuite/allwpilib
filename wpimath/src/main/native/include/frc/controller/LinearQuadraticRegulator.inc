// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>
#include <string>

#include <Eigen/Cholesky>
#include <unsupported/Eigen/MatrixFunctions>

#include "frc/DARE.h"
#include "frc/StateSpaceUtil.h"
#include "frc/controller/LinearQuadraticRegulator.h"
#include "frc/fmt/Eigen.h"
#include "frc/system/Discretization.h"
#include "wpimath/MathShared.h"

namespace frc {

template <int States, int Inputs>
template <int Outputs>
LinearQuadraticRegulator<States, Inputs>::LinearQuadraticRegulator(
    const LinearSystem<States, Inputs, Outputs>& plant,
    const StateArray& Qelems, const InputArray& Relems, units::second_t dt)
    : LinearQuadraticRegulator(plant.A(), plant.B(), Qelems, Relems, dt) {}

template <int States, int Inputs>
LinearQuadraticRegulator<States, Inputs>::LinearQuadraticRegulator(
    const Matrixd<States, States>& A, const Matrixd<States, Inputs>& B,
    const StateArray& Qelems, const InputArray& Relems, units::second_t dt)
    : LinearQuadraticRegulator(A, B, MakeCostMatrix(Qelems),
                               MakeCostMatrix(Relems), dt) {}

template <int States, int Inputs>
LinearQuadraticRegulator<States, Inputs>::LinearQuadraticRegulator(
    const Matrixd<States, States>& A, const Matrixd<States, Inputs>& B,
    const Matrixd<States, States>& Q, const Matrixd<Inputs, Inputs>& R,
    units::second_t dt) {
  Matrixd<States, States> discA;
  Matrixd<States, Inputs> discB;
  DiscretizeAB<States, Inputs>(A, B, dt, &discA, &discB);

  if (!IsStabilizable<States, Inputs>(discA, discB)) {
    std::string msg = fmt::format(
        "The system passed to the LQR is uncontrollable!\n\nA =\n{}\nB "
        "=\n{}\n",
        discA, discB);

    wpi::math::MathSharedStore::ReportError(msg);
    throw std::invalid_argument(msg);
  }

  Matrixd<States, States> S = DARE<States, Inputs>(discA, discB, Q, R);

  // K = (BᵀSB + R)⁻¹BᵀSA
  m_K = (discB.transpose() * S * discB + R)
            .llt()
            .solve(discB.transpose() * S * discA);

  Reset();
}

template <int States, int Inputs>
LinearQuadraticRegulator<States, Inputs>::LinearQuadraticRegulator(
    const Matrixd<States, States>& A, const Matrixd<States, Inputs>& B,
    const Matrixd<States, States>& Q, const Matrixd<Inputs, Inputs>& R,
    const Matrixd<States, Inputs>& N, units::second_t dt) {
  Matrixd<States, States> discA;
  Matrixd<States, Inputs> discB;
  DiscretizeAB<States, Inputs>(A, B, dt, &discA, &discB);

  Matrixd<States, States> S = DARE<States, Inputs>(discA, discB, Q, R, N);

  // K = (BᵀSB + R)⁻¹(BᵀSA + Nᵀ)
  m_K = (discB.transpose() * S * discB + R)
            .llt()
            .solve(discB.transpose() * S * discA + N.transpose());

  Reset();
}

template <int States, int Inputs>
typename LinearQuadraticRegulator<States, Inputs>::InputVector
LinearQuadraticRegulator<States, Inputs>::Calculate(const StateVector& x) {
  m_u = m_K * (m_r - x);
  return m_u;
}

template <int States, int Inputs>
typename LinearQuadraticRegulator<States, Inputs>::InputVector
LinearQuadraticRegulator<States, Inputs>::Calculate(const StateVector& x,
                                                    const StateVector& nextR) {
  m_r = nextR;
  return Calculate(x);
}

template <int States, int Inputs>
template <int Outputs>
void LinearQuadraticRegulator<States, Inputs>::LatencyCompensate(
    const LinearSystem<States, Inputs, Outputs>& plant, units::second_t dt,
    units::second_t inputDelay) {
  Matrixd<States, States> discA;
  Matrixd<States, Inputs> discB;
  DiscretizeAB<States, Inputs>(plant.A(), plant.B(), dt, &discA, &discB);

  m_K = m_K * (discA - discB * m_K).pow(inputDelay / dt);
}

}  // namespace frc
