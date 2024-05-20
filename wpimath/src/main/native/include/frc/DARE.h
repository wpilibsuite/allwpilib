// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>
#include <string>

#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <Eigen/LU>

#include "frc/StateSpaceUtil.h"
#include "frc/fmt/Eigen.h"

// Works cited:
//
// [1] E. K.-W. Chu, H.-Y. Fan, W.-W. Lin & C.-S. Wang "Structure-Preserving
//     Algorithms for Periodic Discrete-Time Algebraic Riccati Equations",
//     International Journal of Control, 77:8, 767-788, 2004.
//     DOI: 10.1080/00207170410001714988

namespace frc {

namespace detail {

/**
 * Checks the preconditions of A, B, and Q for the DARE solver.
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 * @param A The system matrix.
 * @param B The input matrix.
 * @param Q The state cost matrix.
 * @throws std::invalid_argument if Q isn't symmetric positive semidefinite.
 * @throws std::invalid_argument if the (A, B) pair isn't stabilizable.
 * @throws std::invalid_argument if the (A, C) pair where Q = CᵀC isn't
 *   detectable.
 */
template <int States, int Inputs>
void CheckDARE_ABQ(const Eigen::Matrix<double, States, States>& A,
                   const Eigen::Matrix<double, States, Inputs>& B,
                   const Eigen::Matrix<double, States, States>& Q) {
  // Require Q be symmetric
  if ((Q - Q.transpose()).norm() > 1e-10) {
    std::string msg = fmt::format("Q isn't symmetric!\n\nQ =\n{}\n", Q);
    throw std::invalid_argument(msg);
  }

  // Require Q be positive semidefinite
  //
  // If Q is a symmetric matrix with a decomposition LDLᵀ, the number of
  // positive, negative, and zero diagonal entries in D equals the number of
  // positive, negative, and zero eigenvalues respectively in Q (see
  // https://en.wikipedia.org/wiki/Sylvester's_law_of_inertia).
  //
  // Therefore, D having no negative diagonal entries is sufficient to prove Q
  // is positive semidefinite.
  auto Q_ldlt = Q.ldlt();
  if (Q_ldlt.info() != Eigen::Success ||
      (Q_ldlt.vectorD().array() < 0.0).any()) {
    std::string msg =
        fmt::format("Q isn't positive semidefinite!\n\nQ =\n{}\n", Q);
    throw std::invalid_argument(msg);
  }

  // Require (A, B) pair be stabilizable
  if (!IsStabilizable<States, Inputs>(A, B)) {
    std::string msg = fmt::format(
        "The (A, B) pair isn't stabilizable!\n\nA =\n{}\nB =\n{}\n", A, B);
    throw std::invalid_argument(msg);
  }

  // Require (A, C) pair be detectable where Q = CᵀC
  //
  // Q = CᵀC = PᵀLDLᵀP
  // C = √(D)LᵀP
  {
    Eigen::Matrix<double, States, States> C =
        Q_ldlt.vectorD().cwiseSqrt().asDiagonal() *
        Eigen::Matrix<double, States, States>{Q_ldlt.matrixL().transpose()} *
        Q_ldlt.transpositionsP();

    if (!IsDetectable<States, States>(A, C)) {
      std::string msg = fmt::format(
          "The (A, C) pair where Q = CᵀC isn't detectable!\n\nA =\n{}\nQ "
          "=\n{}\n",
          A, Q);
      throw std::invalid_argument(msg);
    }
  }
}

/**
 * Computes the unique stabilizing solution X to the discrete-time algebraic
 * Riccati equation:
 *
 *   AᵀXA − X − AᵀXB(BᵀXB + R)⁻¹BᵀXA + Q = 0
 *
 * This internal function skips expensive precondition checks for increased
 * performance. The solver may hang if any of the following occur:
 * <ul>
 *   <li>Q isn't symmetric positive semidefinite</li>
 *   <li>R isn't symmetric positive definite</li>
 *   <li>The (A, B) pair isn't stabilizable</li>
 *   <li>The (A, C) pair where Q = CᵀC isn't detectable</li>
 * </ul>
 * Only use this function if you're sure the preconditions are met.
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 * @param A The system matrix.
 * @param B The input matrix.
 * @param Q The state cost matrix.
 * @param R_llt The LLT decomposition of the input cost matrix.
 */
template <int States, int Inputs>
Eigen::Matrix<double, States, States> DARE(
    const Eigen::Matrix<double, States, States>& A,
    const Eigen::Matrix<double, States, Inputs>& B,
    const Eigen::Matrix<double, States, States>& Q,
    const Eigen::LLT<Eigen::Matrix<double, Inputs, Inputs>>& R_llt) {
  using StateMatrix = Eigen::Matrix<double, States, States>;

  // Implements the SDA algorithm on page 5 of [1].

  // A₀ = A
  StateMatrix A_k = A;

  // G₀ = BR⁻¹Bᵀ
  //
  // See equation (4) of [1].
  StateMatrix G_k = B * R_llt.solve(B.transpose());

  // H₀ = Q
  //
  // See equation (4) of [1].
  StateMatrix H_k;
  StateMatrix H_k1 = Q;

  do {
    H_k = H_k1;

    // W = I + GₖHₖ
    StateMatrix W = StateMatrix::Identity(H_k.rows(), H_k.cols()) + G_k * H_k;

    auto W_solver = W.lu();

    // Solve WV₁ = Aₖ for V₁
    StateMatrix V_1 = W_solver.solve(A_k);

    // Solve V₂Wᵀ = Gₖ for V₂
    //
    // We want to put V₂Wᵀ = Gₖ into Ax = b form so we can solve it more
    // efficiently.
    //
    // V₂Wᵀ = Gₖ
    // (V₂Wᵀ)ᵀ = Gₖᵀ
    // WV₂ᵀ = Gₖᵀ
    //
    // The solution of Ax = b can be found via x = A.solve(b).
    //
    // V₂ᵀ = W.solve(Gₖᵀ)
    // V₂ = W.solve(Gₖᵀ)ᵀ
    StateMatrix V_2 = W_solver.solve(G_k.transpose()).transpose();

    // Gₖ₊₁ = Gₖ + AₖV₂Aₖᵀ
    G_k += A_k * V_2 * A_k.transpose();

    // Hₖ₊₁ = Hₖ + V₁ᵀHₖAₖ
    H_k1 = H_k + V_1.transpose() * H_k * A_k;

    // Aₖ₊₁ = AₖV₁
    A_k *= V_1;

    // while |Hₖ₊₁ − Hₖ| > ε |Hₖ₊₁|
  } while ((H_k1 - H_k).norm() > 1e-10 * H_k1.norm());

  return H_k1;
}

/**
Computes the unique stabilizing solution X to the discrete-time algebraic
Riccati equation:

  AᵀXA − X − (AᵀXB + N)(BᵀXB + R)⁻¹(BᵀXA + Nᵀ) + Q = 0

This is equivalent to solving the original DARE:

  A₂ᵀXA₂ − X − A₂ᵀXB(BᵀXB + R)⁻¹BᵀXA₂ + Q₂ = 0

where A₂ and Q₂ are a change of variables:

  A₂ = A − BR⁻¹Nᵀ and Q₂ = Q − NR⁻¹Nᵀ

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

This internal function skips expensive precondition checks for increased
performance. The solver may hang if any of the following occur:
<ul>
  <li>Q₂ isn't symmetric positive semidefinite</li>
  <li>R isn't symmetric positive definite</li>
  <li>The (A₂, B) pair isn't stabilizable</li>
  <li>The (A₂, C) pair where Q₂ = CᵀC isn't detectable</li>
</ul>
Only use this function if you're sure the preconditions are met.

@tparam States Number of states.
@tparam Inputs Number of inputs.
@param A The system matrix.
@param B The input matrix.
@param Q The state cost matrix.
@param R_llt The LLT decomposition of the input cost matrix.
@param N The state-input cross cost matrix.
*/
template <int States, int Inputs>
Eigen::Matrix<double, States, States> DARE(
    const Eigen::Matrix<double, States, States>& A,
    const Eigen::Matrix<double, States, Inputs>& B,
    const Eigen::Matrix<double, States, States>& Q,
    const Eigen::LLT<Eigen::Matrix<double, Inputs, Inputs>>& R_llt,
    const Eigen::Matrix<double, Inputs, Inputs>& N) {
  // This is a change of variables to make the DARE that includes Q, R, and N
  // cost matrices fit the form of the DARE that includes only Q and R cost
  // matrices.
  //
  // This is equivalent to solving the original DARE:
  //
  //   A₂ᵀXA₂ − X − A₂ᵀXB(BᵀXB + R)⁻¹BᵀXA₂ + Q₂ = 0
  //
  // where A₂ and Q₂ are a change of variables:
  //
  //   A₂ = A − BR⁻¹Nᵀ and Q₂ = Q − NR⁻¹Nᵀ
  return detail::DARE<States, Inputs>(A - B * R_llt.solve(N.transpose()), B,
                                      Q - N * R_llt.solve(N.transpose()),
                                      R_llt);
}

}  // namespace detail

/**
 * Computes the unique stabilizing solution X to the discrete-time algebraic
 * Riccati equation:
 *
 *   AᵀXA − X − AᵀXB(BᵀXB + R)⁻¹BᵀXA + Q = 0
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
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
template <int States, int Inputs>
Eigen::Matrix<double, States, States> DARE(
    const Eigen::Matrix<double, States, States>& A,
    const Eigen::Matrix<double, States, Inputs>& B,
    const Eigen::Matrix<double, States, States>& Q,
    const Eigen::Matrix<double, Inputs, Inputs>& R) {
  // Require R be symmetric
  if ((R - R.transpose()).norm() > 1e-10) {
    std::string msg = fmt::format("R isn't symmetric!\n\nR =\n{}\n", R);
    throw std::invalid_argument(msg);
  }

  // Require R be positive definite
  auto R_llt = R.llt();
  if (R_llt.info() != Eigen::Success) {
    std::string msg = fmt::format("R isn't positive definite!\n\nR =\n{}\n", R);
    throw std::invalid_argument(msg);
  }

  detail::CheckDARE_ABQ<States, Inputs>(A, B, Q);

  return detail::DARE<States, Inputs>(A, B, Q, R_llt);
}

/**
Computes the unique stabilizing solution X to the discrete-time algebraic
Riccati equation:

  AᵀXA − X − (AᵀXB + N)(BᵀXB + R)⁻¹(BᵀXA + Nᵀ) + Q = 0

This is equivalent to solving the original DARE:

  A₂ᵀXA₂ − X − A₂ᵀXB(BᵀXB + R)⁻¹BᵀXA₂ + Q₂ = 0

where A₂ and Q₂ are a change of variables:

  A₂ = A − BR⁻¹Nᵀ and Q₂ = Q − NR⁻¹Nᵀ

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

@tparam States Number of states.
@tparam Inputs Number of inputs.
@param A The system matrix.
@param B The input matrix.
@param Q The state cost matrix.
@param R The input cost matrix.
@param N The state-input cross cost matrix.
@throws std::invalid_argument if Q₂ isn't symmetric positive semidefinite.
@throws std::invalid_argument if R isn't symmetric positive definite.
@throws std::invalid_argument if the (A₂, B) pair isn't stabilizable.
@throws std::invalid_argument if the (A₂, C) pair where Q₂ = CᵀC isn't
  detectable.
*/
template <int States, int Inputs>
Eigen::Matrix<double, States, States> DARE(
    const Eigen::Matrix<double, States, States>& A,
    const Eigen::Matrix<double, States, Inputs>& B,
    const Eigen::Matrix<double, States, States>& Q,
    const Eigen::Matrix<double, Inputs, Inputs>& R,
    const Eigen::Matrix<double, States, Inputs>& N) {
  // Require R be symmetric
  if ((R - R.transpose()).norm() > 1e-10) {
    std::string msg = fmt::format("R isn't symmetric!\n\nR =\n{}\n", R);
    throw std::invalid_argument(msg);
  }

  // Require R be positive definite
  auto R_llt = R.llt();
  if (R_llt.info() != Eigen::Success) {
    std::string msg = fmt::format("R isn't positive definite!\n\nR =\n{}\n", R);
    throw std::invalid_argument(msg);
  }

  // This is a change of variables to make the DARE that includes Q, R, and N
  // cost matrices fit the form of the DARE that includes only Q and R cost
  // matrices.
  //
  // This is equivalent to solving the original DARE:
  //
  //   A₂ᵀXA₂ − X − A₂ᵀXB(BᵀXB + R)⁻¹BᵀXA₂ + Q₂ = 0
  //
  // where A₂ and Q₂ are a change of variables:
  //
  //   A₂ = A − BR⁻¹Nᵀ and Q₂ = Q − NR⁻¹Nᵀ
  Eigen::Matrix<double, States, States> A_2 =
      A - B * R_llt.solve(N.transpose());
  Eigen::Matrix<double, States, States> Q_2 =
      Q - N * R_llt.solve(N.transpose());

  detail::CheckDARE_ABQ<States, Inputs>(A_2, B, Q_2);

  return detail::DARE<States, Inputs>(A_2, B, Q_2, R_llt);
}

}  // namespace frc
