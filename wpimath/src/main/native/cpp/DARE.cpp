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
  Eigen::EigenSolver<Eigen::MatrixXd> es{A};

  for (int i = 0; i < A.rows(); ++i) {
    if (es.eigenvalues()[i].real() * es.eigenvalues()[i].real() +
            es.eigenvalues()[i].imag() * es.eigenvalues()[i].imag() <
        1) {
      continue;
    }

    Eigen::MatrixXcd E{A.rows(), A.rows() + B.cols()};
    E << es.eigenvalues()[i] * Eigen::MatrixXcd::Identity(A.rows(), A.rows()) -
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

/**
 * Returns true if all the matrix's eigenvalues are greater than or equal to
 * zero.
 *
 * @param A The matrix.
 */
bool IsPositiveSemidefinite(const Eigen::Ref<const Eigen::MatrixXd>& A) {
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es{A};
  for (int i = 0; i < A.rows(); ++i) {
    if (es.eigenvalues()[i] < 0) {
      return false;
    }
  }

  return true;
}

/**
 * Returns true if all the matrix's eigenvalues are greater than zero.
 *
 * @param A The matrix.
 */
bool IsPositiveDefinite(const Eigen::Ref<const Eigen::MatrixXd>& A) {
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es{A};
  for (int i = 0; i < A.rows(); ++i) {
    if (es.eigenvalues()[i] <= 0) {
      return false;
    }
  }

  return true;
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

  // Require the number of inputs be less than or equal to the number of states
  if (inputs > states) {
    std::string msg = fmt::format(
        "Number of inputs ({}) is greater than number of states ({})!", inputs,
        states);
    throw std::invalid_argument(msg);
  }

  // Require Q be symmetric
  if ((Q - Q.transpose()).norm() > 1e-10) {
    std::string msg =
        fmt::format("Q isn't symmetric!\n\nQ =\n{}\n", Eigen::MatrixXd{Q});
    throw std::invalid_argument(msg);
  }

  // Require Q be positive semidefinite
  if (!IsPositiveSemidefinite(Q)) {
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

  // Require R be positive definite
  if (!IsPositiveDefinite(R)) {
    std::string msg = fmt::format("R isn't positive definite!\n\nR =\n{}\n",
                                  Eigen::MatrixXd{R});
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
    Eigen::LDLT<Eigen::MatrixXd> ldlt{Q};
    Eigen::MatrixXd C = Eigen::MatrixXd{ldlt.matrixL()} *
                        ldlt.vectorD().cwiseSqrt().asDiagonal();

    if (!IsDetectable(A, C)) {
      std::string msg = fmt::format(
          "The (A, C) pair where Q = CᵀC isn't detectable!\n\nA =\n{}\nQ "
          "=\n{}\n",
          Eigen::MatrixXd{A}, Eigen::MatrixXd{Q});
      throw std::invalid_argument(msg);
    }
  }

  // Implements the SSCA algorithm on page 12 of [1].

  // A₀ = A
  Eigen::MatrixXd A_k = A;
  Eigen::MatrixXd A_k1 = A;

  // G₀ = BR⁻¹Bᵀ
  //
  // See equation (4) of [1].
  Eigen::MatrixXd G_k = B * R.llt().solve(B.transpose());

  Eigen::MatrixXd I = Eigen::MatrixXd::Identity(A.rows(), A.cols());

  // H₀ = Q
  //
  // See equation (4) of [1].
  Eigen::MatrixXd H_k = Q;
  Eigen::MatrixXd H_k1 = Q;

  do {
    A_k = A_k1;
    H_k = H_k1;

    // W = I + HₖGₖ
    Eigen::MatrixXd W = I + H_k * G_k;

    // W is symmetric positive definite, so the LLT decomposition would work
    // here and is faster than the householder QR decomposition [2]. However,
    // it's not accurate enough. Experimentation showed that so many iterations
    // of iterative refinement [3] were required to fix the accuracy that the
    // total system solve time was much higher than householder QR.
    //
    // [2] https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html
    // [3] https://en.wikipedia.org/wiki/Iterative_refinement
    auto W_solver = W.householderQr();

    // Solve WV₁ = Aₖᵀ for V₁
    Eigen::MatrixXd V_1 = W_solver.solve(A_k.transpose());

    // Solve WV₂ = Hₖ for V₂
    Eigen::MatrixXd V_2 = W_solver.solve(H_k);

    // Aₖ₊₁ = V₁ᵀAₖ
    A_k1 = V_1.transpose() * A_k;

    // Gₖ₊₁ = Gₖ + AₖGₖV₁
    G_k += A_k * G_k * V_1;

    // Hₖ₊₁ = Hₖ + AₖᵀV₂Aₖ
    H_k1 = H_k + A_k.transpose() * V_2 * A_k;

    // while |Hₖ₊₁ − Hₖ| > ε |Hₖ₊₁|
  } while ((H_k1 - H_k).norm() > 1e-10 * H_k1.norm());

  return H_k1;
}

Eigen::MatrixXd DARE(const Eigen::Ref<const Eigen::MatrixXd>& A,
                     const Eigen::Ref<const Eigen::MatrixXd>& B,
                     const Eigen::Ref<const Eigen::MatrixXd>& Q,
                     const Eigen::Ref<const Eigen::MatrixXd>& R,
                     const Eigen::Ref<const Eigen::MatrixXd>& N) {
  // Check argument dimensions
  assert(N.rows() == B.rows() && N.cols() == B.cols());

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
  return DARE(A - B * R.llt().solve(N.transpose()), B,
              Q - N * R.llt().solve(N.transpose()), R);
}

}  // namespace frc
