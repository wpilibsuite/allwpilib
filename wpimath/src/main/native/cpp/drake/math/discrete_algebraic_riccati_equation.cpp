#include "drake/math/discrete_algebraic_riccati_equation.h"

#include <Eigen/Eigenvalues>
#include <Eigen/QR>

#include "discrete_algebraic_riccati_equation_helpers.h"
#include "drake/common/drake_assert.h"
#include "drake/common/drake_throw.h"
#include "drake/common/is_approx_equal_abstol.h"

namespace drake::math {
/**
 * DiscreteAlgebraicRiccatiEquation function
 * computes the unique stabilizing solution X to the discrete-time algebraic
 * Riccati equation:
 *
 * AᵀXA − X − AᵀXB(BᵀXB + R)⁻¹BᵀXA + Q = 0
 *
 * @throws std::exception if Q is not positive semi-definite.
 * @throws std::exception if R is not positive definite.
 *
 * Based on the Schur Vector approach outlined in this paper:
 * "On the Numerical Solution of the Discrete-Time Algebraic Riccati Equation"
 * by Thrasyvoulos Pappas, Alan J. Laub, and Nils R. Sandell, in TAC, 1980,
 * http://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=1102434
 *
 * Note: When, for example, n = 100, m = 80, and entries of A, B, Q_half,
 * R_half are sampled from standard normal distributions, where
 * Q = Q_halfᵀ Q_half and similar for R, the absolute error of the solution
 * is 10⁻⁶, while the absolute error of the solution computed by Matlab is
 * 10⁻⁸.
 *
 * TODO(weiqiao.han): I may overwrite the RealQZ function to improve the
 * accuracy, together with more thorough tests.
 */

Eigen::MatrixXd DiscreteAlgebraicRiccatiEquation(
    const Eigen::Ref<const Eigen::MatrixXd>& A,
    const Eigen::Ref<const Eigen::MatrixXd>& B,
    const Eigen::Ref<const Eigen::MatrixXd>& Q,
    const Eigen::Ref<const Eigen::MatrixXd>& R) {
  int n = B.rows(), m = B.cols();

  DRAKE_DEMAND(m <= n);
  DRAKE_DEMAND(A.rows() == n && A.cols() == n);
  DRAKE_DEMAND(Q.rows() == n && Q.cols() == n);
  DRAKE_DEMAND(R.rows() == m && R.cols() == m);
  DRAKE_DEMAND(is_approx_equal_abstol(Q, Q.transpose(), 1e-10));
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(Q);
  for (int i = 0; i < n; i++) {
    DRAKE_THROW_UNLESS(es.eigenvalues()[i] >= 0);
  }
  DRAKE_DEMAND(is_approx_equal_abstol(R, R.transpose(), 1e-10));
  Eigen::LLT<Eigen::MatrixXd> R_cholesky(R);
  DRAKE_THROW_UNLESS(R_cholesky.info() == Eigen::Success);
  check_stabilizable(A, B);
  check_detectable(A, Q);

  Eigen::MatrixXd M(2 * n, 2 * n), L(2 * n, 2 * n);
  M << A, Eigen::MatrixXd::Zero(n, n), -Q, Eigen::MatrixXd::Identity(n, n);
  L << Eigen::MatrixXd::Identity(n, n), B * R.inverse() * B.transpose(),
      Eigen::MatrixXd::Zero(n, n), A.transpose();

  // QZ decomposition of M and L
  // QMZ = S, QLZ = T
  // where Q and Z are real orthogonal matrixes
  // T is upper-triangular matrix, and S is upper quasi-triangular matrix
  Eigen::RealQZ<Eigen::MatrixXd> qz(2 * n);
  qz.compute(M, L);  // M = Q S Z,  L = Q T Z (Q and Z computed by Eigen package
                     // are adjoints of Q and Z above)
  Eigen::MatrixXd S = qz.matrixS(), T = qz.matrixT(),
                  Z = qz.matrixZ().adjoint();

  // Reorder the generalized eigenvalues of (S,T).
  Eigen::MatrixXd Z2 = Eigen::MatrixXd::Identity(2 * n, 2 * n);
  reorder_eigen(S, T, Z2);
  Z = (Z * Z2).eval();

  // The first n columns of Z is ( U1 ) .
  //                             ( U2 )
  //            -1
  // X = U2 * U1   is a solution of the discrete time Riccati equation.
  Eigen::MatrixXd U1 = Z.block(0, 0, n, n), U2 = Z.block(n, 0, n, n);
  Eigen::MatrixXd X = U2 * U1.inverse();
  X = (X + X.adjoint().eval()) / 2.0;
  return X;
}

Eigen::MatrixXd DiscreteAlgebraicRiccatiEquation(
    const Eigen::Ref<const Eigen::MatrixXd>& A,
    const Eigen::Ref<const Eigen::MatrixXd>& B,
    const Eigen::Ref<const Eigen::MatrixXd>& Q,
    const Eigen::Ref<const Eigen::MatrixXd>& R,
    const Eigen::Ref<const Eigen::MatrixXd>& N) {
    DRAKE_DEMAND(N.rows() == B.rows() && N.cols() == B.cols());

    // This is a change of variables to make the DARE that includes Q, R, and N
    // cost matrices fit the form of the DARE that includes only Q and R cost
    // matrices.
    Eigen::MatrixXd A2 = A - B * R.llt().solve(N.transpose());
    Eigen::MatrixXd Q2 = Q - N * R.llt().solve(N.transpose());
    return DiscreteAlgebraicRiccatiEquation(A2, B, Q2, R);
}

}  // namespace drake::math
