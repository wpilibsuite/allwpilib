// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DARE.h"

#include <cassert>
#include <stdexcept>
#include <string>

#include "Eigen/Cholesky"
#include "Eigen/Core"
#include "Eigen/Eigenvalues"
#include "Eigen/LU"
#include "Eigen/QR"
#include "frc/fmt/Eigen.h"

// Works cited:
//
// [1] E. K.-W. Chu, H.-Y. Fan, W.-W. Lin & C.-S. Wang "Structure-Preserving
//     Algorithms for Periodic Discrete-Time Algebraic Riccati Equations",
//     International Journal of Control, 77:8, 767-788, 2004.
//     DOI: 10.1080/00207170410001714988

namespace frc {

namespace {

/**
 * Returns true if (A, B) is a stabilizable pair.
 *
 * (A, B) is stabilizable if and only if the uncontrollable eigenvalues of A, if
 * any, have absolute values less than one, where an eigenvalue is
 * uncontrollable if rank([λI - A, B]) < n where n is the number of states.
 *
 * @param A System matrix.
 * @param B Input matrix.
 */
bool IsStabilizable(const Eigen::Ref<const Eigen::MatrixXd>& A,
                    const Eigen::Ref<const Eigen::MatrixXd>& B) {
  Eigen::EigenSolver<Eigen::MatrixXd> es{A, false};

  for (int i = 0; i < A.rows(); ++i) {
    if (es.eigenvalues()[i].real() * es.eigenvalues()[i].real() +
            es.eigenvalues()[i].imag() * es.eigenvalues()[i].imag() <
        1) {
      continue;
    }

    Eigen::MatrixXcd E{A.rows(), A.rows() + B.cols()};
    E << es.eigenvalues()[i] * Eigen::MatrixXcd::Identity(A.rows(), A.cols()) -
             A,
        B;

    Eigen::ColPivHouseholderQR<Eigen::MatrixXcd> qr{E};
    if (qr.rank() < A.rows()) {
      return false;
    }
  }
  return true;
}

/**
 * Returns true if (A, C) is a detectable pair.
 *
 * (A, C) is detectable if and only if the unobservable eigenvalues of A, if
 * any, have absolute values less than one, where an eigenvalue is unobservable
 * if rank([λI - A; C]) < n where n is the number of states.
 *
 * @param A System matrix.
 * @param C Output matrix.
 */
bool IsDetectable(const Eigen::Ref<const Eigen::MatrixXd>& A,
                  const Eigen::Ref<const Eigen::MatrixXd>& C) {
  return IsStabilizable(A.transpose(), C.transpose());
}

}  // namespace

Eigen::MatrixXd DARE(const Eigen::Ref<const Eigen::MatrixXd>& A,
                     const Eigen::Ref<const Eigen::MatrixXd>& B,
                     const Eigen::Ref<const Eigen::MatrixXd>& Q,
                     const Eigen::Ref<const Eigen::MatrixXd>& R) {
  // These are unused if assertions aren't compiled in
  [[maybe_unused]] int states = A.rows();
  [[maybe_unused]] int inputs = B.cols();

  // Check argument dimensions
  assert(A.rows() == states && A.cols() == states);
  assert(B.rows() == states && B.cols() == inputs);
  assert(Q.rows() == states && Q.cols() == states);
  assert(R.rows() == inputs && R.cols() == inputs);

  // Require Q be symmetric
  if ((Q - Q.transpose()).norm() > 1e-10) {
    std::string msg =
        fmt::format("Q isn't symmetric!\n\nQ =\n{}\n", Eigen::MatrixXd{Q});
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
    std::string msg = fmt::format("Q isn't positive semidefinite!\n\nQ =\n{}\n",
                                  Eigen::MatrixXd{Q});
    throw std::invalid_argument(msg);
  }

  // Require R be symmetric
  if ((R - R.transpose()).norm() > 1e-10) {
    std::string msg =
        fmt::format("R isn't symmetric!\n\nR =\n{}\n", Eigen::MatrixXd{R});
    throw std::invalid_argument(msg);
  }

  // Require (A, B) pair be stabilizable
  if (!IsStabilizable(A, B)) {
    std::string msg =
        fmt::format("The (A, B) pair isn't stabilizable!\n\nA =\n{}\nB =\n{}\n",
                    Eigen::MatrixXd{A}, Eigen::MatrixXd{B});
    throw std::invalid_argument(msg);
  }

  // Require (A, C) pair be detectable where Q = CᵀC
  {
    Eigen::MatrixXd C = Eigen::MatrixXd{Q_ldlt.matrixL()} *
                        Q_ldlt.vectorD().cwiseSqrt().asDiagonal();

    if (!IsDetectable(A, C)) {
      std::string msg = fmt::format(
          "The (A, C) pair where Q = CᵀC isn't detectable!\n\nA =\n{}\nQ "
          "=\n{}\n",
          Eigen::MatrixXd{A}, Eigen::MatrixXd{Q});
      throw std::invalid_argument(msg);
    }
  }

  return internal::DARE(A, B, Q, R);
}

Eigen::MatrixXd DARE(const Eigen::Ref<const Eigen::MatrixXd>& A,
                     const Eigen::Ref<const Eigen::MatrixXd>& B,
                     const Eigen::Ref<const Eigen::MatrixXd>& Q,
                     const Eigen::Ref<const Eigen::MatrixXd>& R,
                     const Eigen::Ref<const Eigen::MatrixXd>& N) {
  // These are unused if assertions aren't compiled in
  [[maybe_unused]] int states = A.rows();
  [[maybe_unused]] int inputs = B.cols();

  // Check argument dimensions
  assert(N.rows() == states && N.cols() == inputs);

  auto R_llt = R.llt();
  if (R_llt.info() != Eigen::Success) {
    std::string msg = fmt::format("R isn't positive definite!\n\nR =\n{}\n",
                                  Eigen::MatrixXd{R});
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
  return DARE(A - B * R_llt.solve(N.transpose()), B,
              Q - N * R_llt.solve(N.transpose()), R);
}

namespace internal {

Eigen::MatrixXd DARE(const Eigen::Ref<const Eigen::MatrixXd>& A,
                     const Eigen::Ref<const Eigen::MatrixXd>& B,
                     const Eigen::Ref<const Eigen::MatrixXd>& Q,
                     const Eigen::Ref<const Eigen::MatrixXd>& R) {
  // Require R be positive definite
  auto R_llt = R.llt();
  if (R_llt.info() != Eigen::Success) {
    std::string msg = fmt::format("R isn't positive definite!\n\nR =\n{}\n",
                                  Eigen::MatrixXd{R});
    throw std::invalid_argument(msg);
  }

  // Implements the SDA algorithm on page 5 of [1].

  // A₀ = A
  Eigen::MatrixXd A_k = A;

  // G₀ = BR⁻¹Bᵀ
  //
  // See equation (4) of [1].
  Eigen::MatrixXd G_k = B * R_llt.solve(B.transpose());

  // H₀ = Q
  //
  // See equation (4) of [1].
  Eigen::MatrixXd H_k;
  Eigen::MatrixXd H_k1 = Q;

  do {
    H_k = H_k1;

    // W = I + GₖHₖ
    Eigen::MatrixXd W =
        Eigen::MatrixXd::Identity(H_k.rows(), H_k.cols()) + G_k * H_k;

    auto W_solver = W.lu();

    // Solve WV₁ = Aₖ for V₁
    Eigen::MatrixXd V_1 = W_solver.solve(A_k);

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
    Eigen::MatrixXd V_2 = W_solver.solve(G_k.transpose()).transpose();

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

Eigen::MatrixXd DARE(const Eigen::Ref<const Eigen::MatrixXd>& A,
                     const Eigen::Ref<const Eigen::MatrixXd>& B,
                     const Eigen::Ref<const Eigen::MatrixXd>& Q,
                     const Eigen::Ref<const Eigen::MatrixXd>& R,
                     const Eigen::Ref<const Eigen::MatrixXd>& N) {
  auto R_llt = R.llt();
  if (R_llt.info() != Eigen::Success) {
    std::string msg = fmt::format("R isn't positive definite!\n\nR =\n{}\n",
                                  Eigen::MatrixXd{R});
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
  return internal::DARE(A - B * R_llt.solve(N.transpose()), B,
                        Q - N * R_llt.solve(N.transpose()), R);
}

}  // namespace internal
}  // namespace frc
