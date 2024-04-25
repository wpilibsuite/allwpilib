// Copyright (c) Sleipnir contributors

#pragma once

#include <cmath>
#include <cstddef>

#include <Eigen/Core>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseCore>

#include "optimization/Inertia.hpp"

// See docs/algorithms.md#Works_cited for citation definitions

namespace sleipnir {

/**
 * Solves systems of linear equations using a regularized LDLT factorization.
 */
class RegularizedLDLT {
 public:
  using Solver = Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>,
                                       Eigen::Lower, Eigen::AMDOrdering<int>>;

  /**
   * Constructs a RegularizedLDLT instance.
   */
  RegularizedLDLT() = default;

  /**
   * Reports whether previous computation was successful.
   */
  Eigen::ComputationInfo Info() { return m_info; }

  /**
   * Computes the regularized LDLT factorization of a matrix.
   *
   * @param lhs Left-hand side of the system.
   * @param numEqualityConstraints The number of equality constraints in the
   *   system.
   * @param μ The barrier parameter for the current interior-point iteration.
   */
  void Compute(const Eigen::SparseMatrix<double>& lhs,
               size_t numEqualityConstraints, double μ) {
    // The regularization procedure is based on algorithm B.1 of [1]
    m_numDecisionVariables = lhs.rows() - numEqualityConstraints;
    m_numEqualityConstraints = numEqualityConstraints;

    const Inertia idealInertia{m_numDecisionVariables, m_numEqualityConstraints,
                               0};
    Inertia inertia;

    double δ = 0.0;
    double γ = 0.0;

    AnalyzePattern(lhs);
    m_solver.factorize(lhs);

    if (m_solver.info() == Eigen::Success) {
      inertia = Inertia{m_solver};

      // If the inertia is ideal, don't regularize the system
      if (inertia == idealInertia) {
        m_info = Eigen::Success;
        return;
      }
    }

    // If the decomposition succeeded and the inertia has some zero eigenvalues,
    // or the decomposition failed, regularize the equality constraints
    if ((m_solver.info() == Eigen::Success && inertia.zero > 0) ||
        m_solver.info() != Eigen::Success) {
      γ = 1e-8 * std::pow(μ, 0.25);
    }

    // Also regularize the Hessian. If the Hessian wasn't regularized in a
    // previous run of Compute(), start at a small value of δ. Otherwise,
    // attempt a δ half as big as the previous run so δ can trend downwards over
    // time.
    if (m_δOld == 0.0) {
      δ = 1e-4;
    } else {
      δ = m_δOld / 2.0;
    }

    while (true) {
      // Regularize lhs by adding a multiple of the identity matrix
      //
      // lhs = [H + AᵢᵀΣAᵢ + δI   Aₑᵀ]
      //       [       Aₑ        −γI ]
      Eigen::SparseMatrix<double> lhsReg = lhs + Regularization(δ, γ);
      AnalyzePattern(lhsReg);
      m_solver.factorize(lhsReg);
      inertia = Inertia{m_solver};

      // If the inertia is ideal, store that value of δ and return.
      // Otherwise, increase δ by an order of magnitude and try again.
      if (inertia == idealInertia) {
        m_δOld = δ;
        m_info = Eigen::Success;
        return;
      } else {
        δ *= 10.0;

        // If the Hessian perturbation is too high, report failure. This can
        // happen due to a rank-deficient equality constraint Jacobian with
        // linearly dependent constraints.
        if (δ > 1e20) {
          m_info = Eigen::NumericalIssue;
          return;
        }
      }
    }
  }

  /**
   * Solve the system of equations using a regularized LDLT factorization.
   *
   * @param rhs Right-hand side of the system.
   */
  template <typename Rhs>
  auto Solve(const Eigen::MatrixBase<Rhs>& rhs) {
    return m_solver.solve(rhs);
  }

 private:
  Solver m_solver;

  Eigen::ComputationInfo m_info = Eigen::Success;

  /// The number of decision variables in the system.
  size_t m_numDecisionVariables = 0;

  /// The number of equality constraints in the system.
  size_t m_numEqualityConstraints = 0;

  /// The value of δ from the previous run of Compute().
  double m_δOld = 0.0;

  // Number of non-zeros in LHS.
  int m_nonZeros = -1;

  /**
   * Reanalize LHS matrix's sparsity pattern if it changed.
   *
   * @param lhs Matrix to analyze.
   */
  void AnalyzePattern(const Eigen::SparseMatrix<double>& lhs) {
    int nonZeros = lhs.nonZeros();
    if (m_nonZeros != nonZeros) {
      m_solver.analyzePattern(lhs);
      m_nonZeros = nonZeros;
    }
  }

  /**
   * Returns regularization matrix.
   *
   * @param δ The Hessian regularization factor.
   * @param γ The equality constraint Jacobian regularization factor.
   */
  Eigen::SparseMatrix<double> Regularization(double δ, double γ) {
    Eigen::VectorXd vec{m_numDecisionVariables + m_numEqualityConstraints};
    size_t row = 0;
    while (row < m_numDecisionVariables) {
      vec(row) = δ;
      ++row;
    }
    while (row < m_numDecisionVariables + m_numEqualityConstraints) {
      vec(row) = -γ;
      ++row;
    }

    return Eigen::SparseMatrix<double>{vec.asDiagonal()};
  }
};

}  // namespace sleipnir
