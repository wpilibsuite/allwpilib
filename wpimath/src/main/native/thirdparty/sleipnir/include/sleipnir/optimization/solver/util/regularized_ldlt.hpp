// Copyright (c) Sleipnir contributors

#pragma once

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "sleipnir/optimization/solver/util/dense_regularized_ldlt.hpp"
#include "sleipnir/optimization/solver/util/sparse_regularized_ldlt.hpp"

namespace slp {

/// Solves systems of linear equations using a regularized LDLT factorization.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
class RegularizedLDLT {
 public:
  /// Type alias for dense vector.
  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
  /// Type alias for sparse matrix.
  using SparseMatrix = Eigen::SparseMatrix<Scalar>;

  /// Constructs a RegularizedLDLT instance.
  ///
  /// @param use_sparse_solver Whether to use sparse or dense solver.
  /// @param num_decision_variables The number of decision variables in the
  ///     system.
  /// @param num_equality_constraints The number of equality constraints in the
  ///     system.
  RegularizedLDLT(bool use_sparse_solver, int num_decision_variables,
                  int num_equality_constraints)
      : m_use_sparse_solver{use_sparse_solver},
        m_sparse_solver{num_decision_variables, num_equality_constraints},
        m_dense_solver{num_decision_variables, num_equality_constraints} {}

  /// Constructs a RegularizedLDLT instance.
  ///
  /// @param use_sparse_solver Whether to use sparse or dense solver.
  /// @param num_decision_variables The number of decision variables in the
  ///     system.
  /// @param num_equality_constraints The number of equality constraints in the
  ///     system.
  /// @param γ_min The minimum constraint regularization.
  RegularizedLDLT(bool use_sparse_solver, int num_decision_variables,
                  int num_equality_constraints, Scalar γ_min)
      : m_use_sparse_solver{use_sparse_solver},
        m_sparse_solver{num_decision_variables, num_equality_constraints,
                        γ_min},
        m_dense_solver{num_decision_variables, num_equality_constraints,
                       γ_min} {}

  /// Reports whether previous computation was successful.
  ///
  /// @return Whether previous computation was successful.
  Eigen::ComputationInfo info() const {
    if (m_use_sparse_solver) {
      return m_sparse_solver.info();
    } else {
      return m_dense_solver.info();
    }
  }

  /// Computes the regularized LDLT factorization of a matrix.
  ///
  /// In sparse mode, the matrix's symbolic decomposition is reused in
  /// subsequent calls, so subsequent calls must be given a matrix with the same
  /// sparsity pattern.
  ///
  /// @param lhs Left-hand side of the system.
  /// @return The factorization.
  RegularizedLDLT& compute(const SparseMatrix& lhs) {
    if (m_use_sparse_solver) {
      m_sparse_solver.compute(lhs);
    } else {
      m_dense_solver.compute(lhs);
    }

    return *this;
  }

  /// Solves the system of equations using a regularized LDLT factorization.
  ///
  /// @param rhs Right-hand side of the system.
  /// @return The solution.
  template <typename Rhs>
  DenseVector solve(const Eigen::MatrixBase<Rhs>& rhs) const {
    if (m_use_sparse_solver) {
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
  DenseVector solve(const Eigen::SparseMatrixBase<Rhs>& rhs) const {
    if (m_use_sparse_solver) {
      return m_sparse_solver.solve(rhs);
    } else {
      return m_dense_solver.solve(rhs);
    }
  }

  /// Returns the Hessian regularization factor.
  ///
  /// @return Hessian regularization factor.
  Scalar hessian_regularization() const {
    if (m_use_sparse_solver) {
      return m_sparse_solver.hessian_regularization();
    } else {
      return m_dense_solver.hessian_regularization();
    }
  }

  /// Returns the constraint Jacobian regularization factor.
  ///
  /// @return Constraint Jacobian regularization factor.
  Scalar constraint_jacobian_regularization() const {
    if (m_use_sparse_solver) {
      return m_sparse_solver.constraint_jacobian_regularization();
    } else {
      return m_dense_solver.constraint_jacobian_regularization();
    }
  }

 private:
  bool m_use_sparse_solver;
  SparseRegularizedLDLT<Scalar> m_sparse_solver;
  DenseRegularizedLDLT<Scalar> m_dense_solver;
};

}  // namespace slp
