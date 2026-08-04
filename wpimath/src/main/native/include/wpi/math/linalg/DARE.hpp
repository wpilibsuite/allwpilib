// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <cmath>
#include <expected>
#include <limits>
#include <string_view>

#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <Eigen/LU>

#include "wpi/math/linalg/detail/MatrixBalance.hpp"
#include "wpi/math/system/LinearSystemUtil.hpp"

namespace wpi::math {

/**
 * Errors the DARE solver can encounter.
 */
enum class DAREError {
  /// Q was not symmetric.
  QNotSymmetric,
  /// Q was not positive semidefinite.
  QNotPositiveSemidefinite,
  /// R was not symmetric.
  RNotSymmetric,
  /// R was not positive definite.
  RNotPositiveDefinite,
  /// (A, B) pair was not stabilizable.
  ABNotStabilizable,
  /// (A, C) pair where Q = CᵀC was not detectable.
  ACNotDetectable,
};

/**
 * Converts the given DAREError enum to a string.
 */
constexpr std::string_view to_string(const DAREError& error) {
  switch (error) {
    case DAREError::QNotSymmetric:
      return "Q was not symmetric.";
    case DAREError::QNotPositiveSemidefinite:
      return "Q was not positive semidefinite.";
    case DAREError::RNotSymmetric:
      return "R was not symmetric.";
    case DAREError::RNotPositiveDefinite:
      return "R was not positive definite.";
    case DAREError::ABNotStabilizable:
      return "(A, B) pair was not stabilizable.";
    case DAREError::ACNotDetectable:
      return "(A, C) pair where Q = CᵀC was not detectable.";
  }

  return "";
}

namespace detail {

constexpr double kDAREFallbackResidualTolerance = 1e-8;

/**
 * Applies diagonal power-of-two scaling to a matrix.
 *
 * The returned matrix is
 *   diag(2^rowExponents) matrix diag(2^colExponents).
 *
 * @tparam Rows Number of rows.
 * @tparam Cols Number of columns.
 * @param matrix The matrix to scale.
 * @param rowExponents The row scale exponents.
 * @param colExponents The column scale exponents.
 * @return The scaled matrix.
 */
template <int Rows, int Cols>
Eigen::Matrix<double, Rows, Cols> ScaleByPowerOfTwo(
    const Eigen::Matrix<double, Rows, Cols>& matrix,
    const Eigen::Ref<const Eigen::VectorXi>& rowExponents,
    const Eigen::Ref<const Eigen::VectorXi>& colExponents) {
  Eigen::Matrix<double, Rows, Cols> result = matrix;

  for (int row = 0; row < matrix.rows(); ++row) {
    for (int col = 0; col < matrix.cols(); ++col) {
      result(row, col) =
          std::ldexp(matrix(row, col), rowExponents[row] + colExponents[col]);
    }
  }

  return result;
}

template <int States>
struct DAREBalanceResult {
  Eigen::Matrix<double, States, States> A;
  Eigen::Matrix<double, States, States> G;
  Eigen::Matrix<double, States, States> H;
  Eigen::VectorXi solutionScaleExponents;
  bool wasBalanced;
};

/**
 * Balances the SDA starting matrices with a structured diagonal similarity.
 *
 * For D = diag(2^p), the DARE is invariant under
 *   A_b = DAD^-1, G_b = DGD, H_b = D^-1HD^-1, and X = D X_b D.
 *
 * To choose p, this function first balances the magnitudes of the 2n-by-2n
 * pencil-like block matrix [A G; H Aᵀ]. A general balance of that block matrix
 * would independently scale the upper and lower copies of each state, which
 * would destroy the Riccati structure. The raw block exponents are therefore
 * projected back onto the structured state scale p.
 *
 * @tparam States Number of states.
 * @param A The SDA A matrix.
 * @param G The SDA G matrix.
 * @param H The SDA H matrix.
 * @return Balanced matrices and solution unscaling exponents.
 */
template <int States>
DAREBalanceResult<States> BalanceDARETerms(
    const Eigen::Matrix<double, States, States>& A,
    const Eigen::Matrix<double, States, States>& G,
    const Eigen::Matrix<double, States, States>& H) {
  using StateMatrix = Eigen::Matrix<double, States, States>;

  const int states = A.rows();
  DAREBalanceResult<States> result{A, G, H, Eigen::VectorXi::Zero(states),
                                   false};

  if (!A.allFinite() || !G.allFinite() || !H.allFinite()) {
    return result;
  }

  // Balance only the entry magnitudes; signs don't matter when choosing row and
  // column scales.
  Eigen::MatrixXd pencilBalance = Eigen::MatrixXd::Zero(2 * states, 2 * states);

  pencilBalance.topLeftCorner(states, states) = A.cwiseAbs();
  pencilBalance.topRightCorner(states, states) = G.cwiseAbs();
  pencilBalance.bottomLeftCorner(states, states) = H.cwiseAbs();
  pencilBalance.bottomRightCorner(states, states) = A.transpose().cwiseAbs();

  // Diagonal entries are unchanged by diagonal similarity scaling, so ignore
  // them when choosing scale factors.
  pencilBalance.diagonal().setZero();

  Eigen::VectorXi rawScales = detail::BalanceMatrixPowerOfTwo(pencilBalance);

  // The upper block uses D and the lower block uses D^-1, so state exponent p_i
  // is half the difference between the two raw block exponents.
  Eigen::VectorXi scaleExponents = Eigen::VectorXi::Zero(states);
  for (int i = 0; i < states; ++i) {
    scaleExponents[i] =
        detail::RoundHalfToEvenDiv2(rawScales[states + i] - rawScales[i]);
  }

  if (scaleExponents.isZero()) {
    return result;
  }

  Eigen::VectorXi negativeScaleExponents = -scaleExponents;
  StateMatrix balancedA =
      ScaleByPowerOfTwo(A, scaleExponents, negativeScaleExponents);
  StateMatrix balancedG = ScaleByPowerOfTwo(G, scaleExponents, scaleExponents);
  StateMatrix balancedH =
      ScaleByPowerOfTwo(H, negativeScaleExponents, negativeScaleExponents);

  if (!balancedA.allFinite() || !balancedG.allFinite() ||
      !balancedH.allFinite()) {
    return result;
  }

  result.A = balancedA;
  result.G = balancedG;
  result.H = balancedH;
  result.solutionScaleExponents = scaleExponents;
  result.wasBalanced = true;
  return result;
}

/**
 * Computes the normalized residual of the DARE fixed-point equation.
 *
 * The SDA operates on G = BR⁻¹Bᵀ, so the residual is evaluated as:
 *
 *   AᵀX(I + GX)⁻¹A − X + H = 0.
 *
 * @tparam States Number of states.
 * @param A The SDA A matrix.
 * @param G The SDA G matrix.
 * @param H The SDA H matrix.
 * @param X The candidate DARE solution.
 * @return Normalized residual, or infinity if it can't be evaluated.
 */
template <int States>
double DAREFixedPointResidual(const Eigen::Matrix<double, States, States>& A,
                              const Eigen::Matrix<double, States, States>& G,
                              const Eigen::Matrix<double, States, States>& H,
                              const Eigen::Matrix<double, States, States>& X) {
  using StateMatrix = Eigen::Matrix<double, States, States>;

  if (!X.allFinite()) {
    return std::numeric_limits<double>::infinity();
  }

  StateMatrix W = StateMatrix::Identity(X.rows(), X.cols()) + G * X;
  if (!W.allFinite()) {
    return std::numeric_limits<double>::infinity();
  }

  StateMatrix solvedA = W.lu().solve(A);
  if (!solvedA.allFinite()) {
    return std::numeric_limits<double>::infinity();
  }

  StateMatrix stateTerm = A.transpose() * X * solvedA;
  StateMatrix residual = stateTerm - X + H;
  if (!stateTerm.allFinite() || !residual.allFinite()) {
    return std::numeric_limits<double>::infinity();
  }

  double normalizer = std::max({1.0, stateTerm.norm(), X.norm(), H.norm()});
  if (!std::isfinite(normalizer)) {
    return std::numeric_limits<double>::infinity();
  }

  return residual.norm() / normalizer;
}

/**
 * Runs the structured doubling iteration for the DARE.
 *
 * @tparam States Number of states.
 * @param A_k The initial SDA A matrix.
 * @param G_k The initial SDA G matrix.
 * @param H_k1 The initial SDA H matrix.
 * @return Solution to the DARE in the same coordinates as the inputs.
 */
template <int States>
Eigen::Matrix<double, States, States> RunDARESDA(
    Eigen::Matrix<double, States, States> A_k,
    Eigen::Matrix<double, States, States> G_k,
    Eigen::Matrix<double, States, States> H_k1) {
  using StateMatrix = Eigen::Matrix<double, States, States>;

  StateMatrix H_k;

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
    //
    // Since W, Gₖ, and Hₖ are symmetric, drop the transposes on Gₖ and V₂.
    //
    // V₂ = W.solve(Gₖ)
    StateMatrix V_2 = W_solver.solve(G_k);

    // Gₖ₊₁ = Gₖ + AₖV₂Aₖᵀ
    // Hₖ₊₁ = Hₖ + V₁ᵀHₖAₖ
    // Aₖ₊₁ = AₖV₁
    G_k += A_k * V_2 * A_k.transpose();
    H_k1 = H_k + V_1.transpose() * H_k * A_k;
    A_k *= V_1;

    // while |Hₖ₊₁ − Hₖ| > ε |Hₖ₊₁|
  } while ((H_k1 - H_k).norm() > 1e-10 * H_k1.norm());

  return H_k1;
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
 * @return Solution to the DARE.
 */
template <int States, int Inputs>
Eigen::Matrix<double, States, States> DARE(
    const Eigen::Matrix<double, States, States>& A,
    const Eigen::Matrix<double, States, Inputs>& B,
    const Eigen::Matrix<double, States, States>& Q,
    const Eigen::LLT<Eigen::Matrix<double, Inputs, Inputs>>& R_llt) {
  using StateMatrix = Eigen::Matrix<double, States, States>;

  // Implements SDA algorithm on p. 5 of [1] (initial A, G, H are from (4)).
  //
  // [1] E. K.-W. Chu, H.-Y. Fan, W.-W. Lin & C.-S. Wang "Structure-Preserving
  //     Algorithms for Periodic Discrete-Time Algebraic Riccati Equations",
  //     International Journal of Control, 77:8, 767-788, 2004.
  //     DOI: 10.1080/00207170410001714988

  // A₀ = A
  // G₀ = BR⁻¹Bᵀ
  // H₀ = Q
  StateMatrix G = B * R_llt.solve(B.transpose());

  auto balanced = BalanceDARETerms(A, G, Q);
  StateMatrix solution = RunDARESDA(balanced.A, balanced.G, balanced.H);

  if (!balanced.wasBalanced) {
    return solution;
  }

  // Convert X_b back to the original coordinates: X = D X_b D.
  solution = ScaleByPowerOfTwo(solution, balanced.solutionScaleExponents,
                               balanced.solutionScaleExponents);

  // Balancing usually improves conditioning, but for some scale patterns it can
  // produce finite iterates that no longer satisfy the original DARE. Keep the
  // optimization only when the unscaled result is still a fixed point.
  double residual = DAREFixedPointResidual(A, G, Q, solution);
  if (!std::isfinite(residual) || residual > kDAREFallbackResidualTolerance) {
    return RunDARESDA(A, G, Q);
  }

  return solution;
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
 * @param checkPreconditions Whether to check preconditions (30% less time if
 *   user is sure precondtions are already met).
 * @return Solution to the DARE on success, or DAREError on failure.
 */
template <int States, int Inputs>
std::expected<Eigen::Matrix<double, States, States>, DAREError> DARE(
    const Eigen::Matrix<double, States, States>& A,
    const Eigen::Matrix<double, States, Inputs>& B,
    const Eigen::Matrix<double, States, States>& Q,
    const Eigen::Matrix<double, Inputs, Inputs>& R,
    bool checkPreconditions = true) {
  if (checkPreconditions) {
    // Require R be symmetric
    if ((R - R.transpose()).norm() > 1e-10) {
      return std::unexpected{DAREError::RNotSymmetric};
    }
  }

  // Require R be positive definite
  auto R_llt = R.llt();
  if (R_llt.info() != Eigen::Success) {
    return std::unexpected{DAREError::RNotPositiveDefinite};
  }

  if (checkPreconditions) {
    // Require Q be symmetric
    if ((Q - Q.transpose()).norm() > 1e-10) {
      return std::unexpected{DAREError::QNotSymmetric};
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
      return std::unexpected{DAREError::QNotPositiveSemidefinite};
    }

    // Require (A, B) pair be stabilizable
    if (!IsStabilizable<States, Inputs>(A, B)) {
      return std::unexpected{DAREError::ABNotStabilizable};
    }

    // Require (A, C) pair be detectable where Q = CᵀC
    //
    // Q = CᵀC = PᵀLDLᵀP
    // C = √(D)LᵀP
    Eigen::Matrix<double, States, States> C =
        Q_ldlt.vectorD().cwiseSqrt().asDiagonal() *
        Eigen::Matrix<double, States, States>{Q_ldlt.matrixL().transpose()} *
        Q_ldlt.transpositionsP();

    if (!IsDetectable<States, States>(A, C)) {
      return std::unexpected{DAREError::ACNotDetectable};
    }
  }

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
@param checkPreconditions Whether to check preconditions (30% less time if user
  is sure precondtions are already met).
@return Solution to the DARE on success, or DAREError on failure.
*/
template <int States, int Inputs>
std::expected<Eigen::Matrix<double, States, States>, DAREError> DARE(
    const Eigen::Matrix<double, States, States>& A,
    const Eigen::Matrix<double, States, Inputs>& B,
    const Eigen::Matrix<double, States, States>& Q,
    const Eigen::Matrix<double, Inputs, Inputs>& R,
    const Eigen::Matrix<double, States, Inputs>& N,
    bool checkPreconditions = true) {
  if (checkPreconditions) {
    // Require R be symmetric
    if ((R - R.transpose()).norm() > 1e-10) {
      return std::unexpected{DAREError::RNotSymmetric};
    }
  }

  // Require R be positive definite
  auto R_llt = R.llt();
  if (R_llt.info() != Eigen::Success) {
    return std::unexpected{DAREError::RNotPositiveDefinite};
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

  if (checkPreconditions) {
    // Require Q be symmetric
    if ((Q_2 - Q_2.transpose()).norm() > 1e-10) {
      return std::unexpected{DAREError::QNotSymmetric};
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
    auto Q_ldlt = Q_2.ldlt();
    if (Q_ldlt.info() != Eigen::Success ||
        (Q_ldlt.vectorD().array() < 0.0).any()) {
      return std::unexpected{DAREError::QNotPositiveSemidefinite};
    }

    // Require (A, B) pair be stabilizable
    if (!IsStabilizable<States, Inputs>(A_2, B)) {
      return std::unexpected{DAREError::ABNotStabilizable};
    }

    // Require (A, C) pair be detectable where Q = CᵀC
    //
    // Q = CᵀC = PᵀLDLᵀP
    // C = √(D)LᵀP
    Eigen::Matrix<double, States, States> C =
        Q_ldlt.vectorD().cwiseSqrt().asDiagonal() *
        Eigen::Matrix<double, States, States>{Q_ldlt.matrixL().transpose()} *
        Q_ldlt.transpositionsP();

    if (!IsDetectable<States, States>(A_2, C)) {
      return std::unexpected{DAREError::ACNotDetectable};
    }
  }

  return detail::DARE<States, Inputs>(A_2, B, Q_2, R_llt);
}

}  // namespace wpi::math
