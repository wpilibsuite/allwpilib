// Copyright (c) Sleipnir contributors

#pragma once

#include <utility>

#include <Eigen/SparseCore>
#include <gch/small_vector.hpp>

#include "sleipnir/autodiff/adjoint_expression_graph.hpp"
#include "sleipnir/autodiff/variable.hpp"
#include "sleipnir/autodiff/variable_matrix.hpp"
#include "sleipnir/util/assert.hpp"
#include "sleipnir/util/concepts.hpp"

namespace slp {

/**
 * This class calculates the Hessian of a variable with respect to a vector of
 * variables.
 *
 * The gradient tree is cached so subsequent Hessian calculations are faster,
 * and the Hessian is only recomputed if the variable expression is nonlinear.
 *
 * @tparam Scalar Scalar type.
 * @tparam UpLo Which part of the Hessian to compute (Lower or Lower | Upper).
 */
template <typename Scalar, int UpLo>
  requires(UpLo == Eigen::Lower) || (UpLo == (Eigen::Lower | Eigen::Upper))
class Hessian {
 public:
  /**
   * Constructs a Hessian object.
   *
   * @param variable Variable of which to compute the Hessian.
   * @param wrt Variable with respect to which to compute the Hessian.
   */
  Hessian(Variable<Scalar> variable, Variable<Scalar> wrt)
      : Hessian{std::move(variable), VariableMatrix<Scalar>{std::move(wrt)}} {}

  /**
   * Constructs a Hessian object.
   *
   * @param variable Variable of which to compute the Hessian.
   * @param wrt Vector of variables with respect to which to compute the
   *   Hessian.
   */
  Hessian(Variable<Scalar> variable, SleipnirMatrixLike<Scalar> auto wrt)
      : m_variables{detail::AdjointExpressionGraph<Scalar>{variable}
                        .generate_gradient_tree(wrt)},
        m_wrt{wrt} {
    slp_assert(m_wrt.cols() == 1);

    // Initialize column each expression's adjoint occupies in the Jacobian
    for (size_t col = 0; col < m_wrt.size(); ++col) {
      m_wrt[col].expr->col = col;
    }

    for (auto& variable : m_variables) {
      m_graphs.emplace_back(variable);
    }

    // Reset col to -1
    for (auto& node : m_wrt) {
      node.expr->col = -1;
    }

    for (int row = 0; row < m_variables.rows(); ++row) {
      if (m_variables[row].expr == nullptr) {
        continue;
      }

      if (m_variables[row].type() == ExpressionType::LINEAR) {
        // If the row is linear, compute its gradient once here and cache its
        // triplets. Constant rows are ignored because their gradients have no
        // nonzero triplets.
        m_graphs[row].append_gradient_triplets(m_cached_triplets, row, m_wrt);
      } else if (m_variables[row].type() > ExpressionType::LINEAR) {
        // If the row is quadratic or nonlinear, add it to the list of nonlinear
        // rows to be recomputed in Value().
        m_nonlinear_rows.emplace_back(row);
      }
    }

    if (m_nonlinear_rows.empty()) {
      m_H.setFromTriplets(m_cached_triplets.begin(), m_cached_triplets.end());
      if constexpr (UpLo == Eigen::Lower) {
        m_H = m_H.template triangularView<Eigen::Lower>();
      }
    }
  }

  /**
   * Returns the Hessian as a VariableMatrix.
   *
   * This is useful when constructing optimization problems with derivatives in
   * them.
   *
   * @return The Hessian as a VariableMatrix.
   */
  VariableMatrix<Scalar> get() const {
    VariableMatrix<Scalar> result{detail::empty, m_variables.rows(),
                                  m_wrt.rows()};

    for (int row = 0; row < m_variables.rows(); ++row) {
      auto grad = m_graphs[row].generate_gradient_tree(m_wrt);
      for (int col = 0; col < m_wrt.rows(); ++col) {
        if (grad[col].expr != nullptr) {
          result(row, col) = std::move(grad[col]);
        } else {
          result(row, col) = Variable{Scalar(0)};
        }
      }
    }

    return result;
  }

  /**
   * Evaluates the Hessian at wrt's value.
   *
   * @return The Hessian at wrt's value.
   */
  const Eigen::SparseMatrix<Scalar>& value() {
    if (m_nonlinear_rows.empty()) {
      return m_H;
    }

    for (auto& graph : m_graphs) {
      graph.update_values();
    }

    // Copy the cached triplets so triplets added for the nonlinear rows are
    // thrown away at the end of the function
    auto triplets = m_cached_triplets;

    // Compute each nonlinear row of the Hessian
    for (int row : m_nonlinear_rows) {
      m_graphs[row].append_gradient_triplets(triplets, row, m_wrt);
    }

    m_H.setFromTriplets(triplets.begin(), triplets.end());
    if constexpr (UpLo == Eigen::Lower) {
      m_H = m_H.template triangularView<Eigen::Lower>();
    }

    return m_H;
  }

 private:
  VariableMatrix<Scalar> m_variables;
  VariableMatrix<Scalar> m_wrt;

  gch::small_vector<detail::AdjointExpressionGraph<Scalar>> m_graphs;

  Eigen::SparseMatrix<Scalar> m_H{m_variables.rows(), m_wrt.rows()};

  // Cached triplets for gradients of linear rows
  gch::small_vector<Eigen::Triplet<Scalar>> m_cached_triplets;

  // List of row indices for nonlinear rows whose graients will be computed in
  // Value()
  gch::small_vector<int> m_nonlinear_rows;
};

// @cond Suppress Doxygen
extern template class EXPORT_TEMPLATE_DECLARE(SLEIPNIR_DLLEXPORT)
Hessian<double, Eigen::Lower | Eigen::Upper>;
// @endcond

}  // namespace slp
