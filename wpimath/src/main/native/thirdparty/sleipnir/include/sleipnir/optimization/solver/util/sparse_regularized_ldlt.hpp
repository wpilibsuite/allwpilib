// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <limits>

#include <Eigen/Core>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseCore>

#include "sleipnir/optimization/solver/util/inertia.hpp"

namespace slp {

/// Solves sparse systems of linear equations using a regularized LDLT
/// factorization.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
class SparseRegularizedLDLT {
 public:
  /// Type alias for dense vector.
  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
  /// Type alias for sparse matrix.
  using SparseMatrix = Eigen::SparseMatrix<Scalar>;

  /// Constructs a SparseRegularizedLDLT instance.
  ///
  /// @param num_decision_variables The number of decision variables in the
  ///     system.
  /// @param num_equality_constraints The number of equality constraints in the
  ///     system.
  SparseRegularizedLDLT(int num_decision_variables,
                        int num_equality_constraints)
      : m_num_decision_variables{num_decision_variables},
        m_num_equality_constraints{num_equality_constraints} {}

  /// Constructs a SparseRegularizedLDLT instance.
  ///
  /// @param num_decision_variables The number of decision variables in the
  ///     system.
  /// @param num_equality_constraints The number of equality constraints in the
  ///     system.
  /// @param γ_min The minimum constraint regularization.
  SparseRegularizedLDLT(int num_decision_variables,
                        int num_equality_constraints, Scalar γ_min)
      : m_num_decision_variables{num_decision_variables},
        m_num_equality_constraints{num_equality_constraints},
        m_γ_min{γ_min} {}

  /// Reports whether previous computation was successful.
  ///
  /// @return Whether previous computation was successful.
  Eigen::ComputationInfo info() const { return m_info; }

  /// Computes the regularized LDLT factorization of a matrix.
  ///
  /// The matrix's symbolic decomposition is reused in subsequent calls, so
  /// subsequent calls must be given a matrix with the same sparsity pattern.
  ///
  /// @param lhs Left-hand side of the system.
  /// @return The factorization.
  SparseRegularizedLDLT& compute(const SparseMatrix& lhs) {
    // Regularization with zeros ensures the pattern analysis in the sparse
    // solver is reused by all factorizations
    SparseMatrix unregularized_lhs = lhs + regularization(Scalar(0), Scalar(0));

    if (!m_analyzed_pattern) {
      m_solver.analyzePattern(unregularized_lhs);
      m_analyzed_pattern = true;
    }

    m_solver.factorize(unregularized_lhs);
    m_info = m_solver.info();

    if (m_info == Eigen::Success) {
      auto D = m_solver.vectorD();

      // If the inertia is ideal and D from LDLT is sufficiently far from zero,
      // don't regularize the system
      if (Inertia{D} == ideal_inertia &&
          (D.cwiseAbs().array() >= Scalar(1e-4)).all()) {
        m_prev_δ = Scalar(0);
        m_prev_γ = Scalar(0);
        return *this;
      }
    }

    // We'll give lhs the correct inertia by adding [δI, 0; 0, −γI] where δ and
    // γ regularize the Hessian and equality constraint Jacobian respectively.

    // If the previous δ was zero, attempt a small value. Otherwise, attempt a
    // smaller value than the previous δ so δ trends downward.
    Scalar δ = m_prev_δ == Scalar(0)
                   ? Scalar(1e-4)
                   : std::max(m_prev_δ / Scalar(2),
                              std::numeric_limits<Scalar>::epsilon());

    // Start γ at the minimum to minimize equality constraint Jacobian
    // distortion.
    Scalar γ = m_γ_min;

    while (true) {
      m_solver.factorize(lhs + regularization(δ, γ));
      m_info = m_solver.info();

      if (m_info == Eigen::Success) {
        Inertia inertia{m_solver.vectorD()};

        if (inertia == ideal_inertia) {
          // If the inertia is ideal, report success
          m_prev_δ = δ;
          m_prev_γ = γ;
          return *this;
        } else if (inertia.zero > 0) {
          if (γ == Scalar(0)) {
            // If there's zero eigenvalues and γ = 0, increase γ to potentially
            // compensate for a rank-deficient equality constraint Jacobian
            γ = Scalar(1e-10);
          } else {
            // If there's zero eigenvalues and γ > 0, increase δ and γ to drive
            // all eigenvalues away from zero
            δ *= Scalar(10);
            γ *= Scalar(10);
          }
        } else if (inertia.negative > ideal_inertia.negative) {
          // If there's too many negative eigenvalues, increase δ to add more
          // positive eigenvalues
          δ *= Scalar(10);
        } else if (inertia.positive > ideal_inertia.positive) {
          // If there's too many positive eigenvalues, increase γ to add more
          // negative eigenvalues
          γ = γ == Scalar(0) ? Scalar(1e-10) : γ * Scalar(10);
        }
      } else {
        // If the decomposition failed, increase δ and γ to drive all
        // eigenvalues away from zero
        δ *= Scalar(10);
        γ = γ == Scalar(0) ? Scalar(1e-10) : γ * Scalar(10);
      }

      // If the lhs perturbation is too high, report failure. This can be caused
      // by ill-conditioning.
      if (δ > Scalar(1e20) || γ > Scalar(1e20)) {
        m_info = Eigen::NumericalIssue;
        m_prev_δ = δ;
        m_prev_γ = γ;
        return *this;
      }
    }
  }

  /// Solves the system of equations using a regularized LDLT factorization.
  ///
  /// @param rhs Right-hand side of the system.
  /// @return The solution.
  template <typename Rhs>
  DenseVector solve(const Eigen::MatrixBase<Rhs>& rhs) const {
    return m_solver.solve(rhs);
  }

  /// Solves the system of equations using a regularized LDLT factorization.
  ///
  /// @param rhs Right-hand side of the system.
  /// @return The solution.
  template <typename Rhs>
  DenseVector solve(const Eigen::SparseMatrixBase<Rhs>& rhs) const {
    return m_solver.solve(rhs);
  }

  /// Returns the Hessian regularization factor.
  ///
  /// @return Hessian regularization factor.
  Scalar hessian_regularization() const { return m_prev_δ; }

  /// Returns the constraint Jacobian regularization factor.
  ///
  /// @return Constraint Jacobian regularization factor.
  Scalar constraint_jacobian_regularization() const { return m_prev_γ; }

 private:
  using Solver = Eigen::SimplicialLDLT<SparseMatrix>;

  Solver m_solver;
  bool m_analyzed_pattern = false;

  Eigen::ComputationInfo m_info = Eigen::Success;

  /// The number of decision variables in the system.
  int m_num_decision_variables = 0;

  /// The number of equality constraints in the system.
  int m_num_equality_constraints = 0;

  /// The minimum constraint regularization.
  Scalar m_γ_min{1e-10};

  /// The ideal system inertia.
  Inertia ideal_inertia{m_num_decision_variables, m_num_equality_constraints,
                        0};

  /// The value of δ from the previous run of compute().
  Scalar m_prev_δ{0};

  /// The value of γ from the previous run of compute().
  Scalar m_prev_γ{0};

  /// Returns regularization matrix.
  ///
  ///   [δI    0]
  ///   [ 0  −γI]
  ///
  /// @param δ The Hessian regularization factor.
  /// @param γ The equality constraint Jacobian regularization factor.
  /// @return Regularization matrix.
  SparseMatrix regularization(Scalar δ, Scalar γ) const {
    DenseVector vec{m_num_decision_variables + m_num_equality_constraints};
    vec.segment(0, m_num_decision_variables).setConstant(δ);
    vec.segment(m_num_decision_variables, m_num_equality_constraints)
        .setConstant(-γ);

    return SparseMatrix{vec.asDiagonal()};
  }
};

}  // namespace slp
