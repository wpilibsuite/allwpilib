// Copyright (c) Sleipnir contributors

#pragma once

#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseCore>

#include "sleipnir/optimization/solver/util/inertia.hpp"

// See docs/algorithms.md#Works_cited for citation definitions

namespace slp {

/// Solves systems of linear equations using a regularized LDLT factorization.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
class RegularizedLDLT {
 public:
  /// Type alias for dense matrix.
  using DenseMatrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
  /// Type alias for dense vector.
  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
  /// Type alias for sparse matrix.
  using SparseMatrix = Eigen::SparseMatrix<Scalar>;

  /// Constructs a RegularizedLDLT instance.
  ///
  /// @param num_decision_variables The number of decision variables in the
  ///     system.
  /// @param num_equality_constraints The number of equality constraints in the
  ///     system.
  RegularizedLDLT(int num_decision_variables, int num_equality_constraints)
      : m_num_decision_variables{num_decision_variables},
        m_num_equality_constraints{num_equality_constraints} {}

  /// Reports whether previous computation was successful.
  ///
  /// @return Whether previous computation was successful.
  Eigen::ComputationInfo info() const { return m_info; }

  /// Computes the regularized LDLT factorization of a matrix.
  ///
  /// @param lhs Left-hand side of the system.
  /// @return The factorization.
  RegularizedLDLT& compute(const SparseMatrix& lhs) {
    // The regularization procedure is based on algorithm B.1 of [1]

    // Max density is 50% due to the caller only providing the lower triangle.
    // We consider less than 25% to be sparse.
    m_is_sparse = lhs.nonZeros() < 0.25 * lhs.size();

    m_info = m_is_sparse ? compute_sparse(lhs).info()
                         : m_dense_solver.compute(lhs).info();

    Inertia inertia;

    if (m_info == Eigen::Success) {
      inertia = m_is_sparse ? Inertia{m_sparse_solver.vectorD()}
                            : Inertia{m_dense_solver.vectorD()};

      // If the inertia is ideal, don't regularize the system
      if (inertia == ideal_inertia) {
        m_prev_δ = Scalar(0);
        return *this;
      }
    }

    // Also regularize the Hessian. If the Hessian wasn't regularized in a
    // previous run of compute(), start at small values of δ and γ. Otherwise,
    // attempt a δ and γ half as big as the previous run so δ and γ can trend
    // downwards over time.
    Scalar δ = m_prev_δ == Scalar(0) ? Scalar(1e-4) : m_prev_δ / Scalar(2);
    Scalar γ(1e-10);

    while (true) {
      // Regularize lhs by adding a multiple of the identity matrix
      //
      // lhs = [H + AᵢᵀΣAᵢ + δI  Aₑᵀ]
      //       [      Aₑ         −γI]
      if (m_is_sparse) {
        m_info = compute_sparse(lhs + regularization(δ, γ)).info();
        if (m_info == Eigen::Success) {
          inertia = Inertia{m_sparse_solver.vectorD()};
        }
      } else {
        m_info = m_dense_solver.compute(lhs + regularization(δ, γ)).info();
        if (m_info == Eigen::Success) {
          inertia = Inertia{m_dense_solver.vectorD()};
        }
      }

      if (m_info == Eigen::Success) {
        if (inertia == ideal_inertia) {
          // If the inertia is ideal, store δ and return
          m_prev_δ = δ;
          return *this;
        } else if (inertia.zero > 0) {
          // If there's zero eigenvalues, increase δ and γ by an order of
          // magnitude and try again
          δ *= Scalar(10);
          γ *= Scalar(10);
        } else if (inertia.negative > ideal_inertia.negative) {
          // If there's too many negative eigenvalues, increase δ by an order of
          // magnitude and try again
          δ *= Scalar(10);
        } else if (inertia.positive > ideal_inertia.positive) {
          // If there's too many positive eigenvalues, increase γ by an order of
          // magnitude and try again
          γ *= Scalar(10);
        }
      } else {
        // If the decomposition failed, increase δ and γ by an order of
        // magnitude and try again
        δ *= Scalar(10);
        γ *= Scalar(10);
      }

      // If the Hessian perturbation is too high, report failure. This can be
      // caused by ill-conditioning.
      if (δ > Scalar(1e20) || γ > Scalar(1e20)) {
        m_info = Eigen::NumericalIssue;
        return *this;
      }
    }
  }

  /// Solves the system of equations using a regularized LDLT factorization.
  ///
  /// @param rhs Right-hand side of the system.
  /// @return The solution.
  template <typename Rhs>
  DenseVector solve(const Eigen::MatrixBase<Rhs>& rhs) {
    if (m_is_sparse) {
      return m_sparse_solver.solve(rhs);
    } else {
      return m_dense_solver.solve(rhs);
    }
  }

  /// Solves the system of equations using a regularized LDLT factorization.
  ///
  /// @param rhs Right-hand side of the system.
  /// @return The solution.
  template <typename Rhs>
  DenseVector solve(const Eigen::SparseMatrixBase<Rhs>& rhs) {
    if (m_is_sparse) {
      return m_sparse_solver.solve(rhs);
    } else {
      return m_dense_solver.solve(rhs.toDense());
    }
  }

  /// Returns the Hessian regularization factor.
  ///
  /// @return Hessian regularization factor.
  Scalar hessian_regularization() const { return m_prev_δ; }

 private:
  using SparseSolver = Eigen::SimplicialLDLT<SparseMatrix>;
  using DenseSolver = Eigen::LDLT<DenseMatrix>;

  SparseSolver m_sparse_solver;
  DenseSolver m_dense_solver;
  bool m_is_sparse = true;

  Eigen::ComputationInfo m_info = Eigen::Success;

  /// The number of decision variables in the system.
  int m_num_decision_variables = 0;

  /// The number of equality constraints in the system.
  int m_num_equality_constraints = 0;

  /// The ideal system inertia.
  Inertia ideal_inertia{m_num_decision_variables, m_num_equality_constraints,
                        0};

  /// The value of δ from the previous run of compute().
  Scalar m_prev_δ{0};

  // Number of non-zeros in LHS.
  int m_non_zeros = -1;

  /// Computes factorization of a sparse matrix.
  ///
  /// @param lhs Matrix to factorize.
  /// @return The factorization.
  SparseSolver& compute_sparse(const SparseMatrix& lhs) {
    // Reanalize lhs's sparsity pattern if it changed
    int non_zeros = lhs.nonZeros();
    if (m_non_zeros != non_zeros) {
      m_sparse_solver.analyzePattern(lhs);
      m_non_zeros = non_zeros;
    }

    m_sparse_solver.factorize(lhs);

    return m_sparse_solver;
  }

  /// Returns regularization matrix.
  ///
  /// @param δ The Hessian regularization factor.
  /// @param γ The equality constraint Jacobian regularization factor.
  /// @return Regularization matrix.
  SparseMatrix regularization(Scalar δ, Scalar γ) {
    DenseVector vec{m_num_decision_variables + m_num_equality_constraints};
    vec.segment(0, m_num_decision_variables).setConstant(δ);
    vec.segment(m_num_decision_variables, m_num_equality_constraints)
        .setConstant(-γ);

    return SparseMatrix{vec.asDiagonal()};
  }
};

}  // namespace slp
