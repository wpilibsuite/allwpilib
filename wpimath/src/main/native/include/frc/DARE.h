// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "Eigen/Core"

namespace frc {

/**
 * Computes the unique stabilizing solution X to the discrete-time algebraic
 * Riccati equation:
 *
 *   AᵀXA − X − AᵀXB(BᵀXB + R)⁻¹BᵀXA + Q = 0
 *
 * @param A The system matrix.
 * @param B The input matrix.
 * @param Q The state cost matrix.
 * @param R The input cost matrix.
 * @throws std::invalid_argument if Q isn't symmetric positive semidefinite.
 * @throws std::invalid_argument if R isn't symmetric positive definite.
 * @throws std::invalid_argument if the (A, B) pair isn't stabilizable.
 * @throws std::invalid_argument if the (A, C) pair where Q = CᵀC isn't
 *   detectable.
 */
WPILIB_DLLEXPORT
Eigen::MatrixXd DARE(const Eigen::Ref<const Eigen::MatrixXd>& A,
                     const Eigen::Ref<const Eigen::MatrixXd>& B,
                     const Eigen::Ref<const Eigen::MatrixXd>& Q,
                     const Eigen::Ref<const Eigen::MatrixXd>& R);

/**
Computes the unique stabilizing solution X to the discrete-time algebraic
Riccati equation:

  AᵀXA − X − (AᵀXB + N)(BᵀXB + R)⁻¹(BᵀXA + Nᵀ) + Q = 0

This overload of the DARE is useful for finding the control law uₖ that
minimizes the following cost function subject to xₖ₊₁ = Axₖ + Buₖ.

@verbatim
    ∞ [xₖ]ᵀ[Q  N][xₖ]
J = Σ [uₖ] [Nᵀ R][uₖ] ΔT
   k=0
@endverbatim

This is a more general form of the following. The linear-quadratic regulator
is the feedback control law uₖ that minimizes the following cost function
subject to xₖ₊₁ = Axₖ + Buₖ:

@verbatim
    ∞
J = Σ (xₖᵀQxₖ + uₖᵀRuₖ) ΔT
   k=0
@endverbatim

This can be refactored as:

@verbatim
    ∞ [xₖ]ᵀ[Q 0][xₖ]
J = Σ [uₖ] [0 R][uₖ] ΔT
   k=0
@endverbatim

@param A The system matrix.
@param B The input matrix.
@param Q The state cost matrix.
@param R The input cost matrix.
@param N The state-input cross cost matrix.
@throws std::invalid_argument if Q − NR⁻¹Nᵀ isn't symmetric positive
  semidefinite.
@throws std::invalid_argument if R isn't symmetric positive definite.
@throws std::invalid_argument if the (A, B) pair isn't stabilizable.
@throws std::invalid_argument if the (A, C) pair where Q = CᵀC isn't detectable.
*/
WPILIB_DLLEXPORT
Eigen::MatrixXd DARE(const Eigen::Ref<const Eigen::MatrixXd>& A,
                     const Eigen::Ref<const Eigen::MatrixXd>& B,
                     const Eigen::Ref<const Eigen::MatrixXd>& Q,
                     const Eigen::Ref<const Eigen::MatrixXd>& R,
                     const Eigen::Ref<const Eigen::MatrixXd>& N);

}  // namespace frc
