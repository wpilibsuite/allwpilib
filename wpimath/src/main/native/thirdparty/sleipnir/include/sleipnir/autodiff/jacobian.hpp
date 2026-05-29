// Copyright (c) Sleipnir contributors

#pragma once

#include <utility>

#include <Eigen/SparseCore>
#include <gch/small_vector.hpp>

#include "sleipnir/autodiff/expression_graph.hpp"
#include "sleipnir/autodiff/variable.hpp"
#include "sleipnir/autodiff/variable_matrix.hpp"
#include "sleipnir/util/assert.hpp"
#include "sleipnir/util/concepts.hpp"
#include "sleipnir/util/symbol_exports.hpp"

namespace slp {

/// This class calculates the Jacobian of a vector of variables with respect to
/// a vector of variables.
///
/// The Jacobian is only recomputed if the variable expression is quadratic or
/// higher order.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
class Jacobian {
 public:
  /// Constructs a Jacobian object.
  ///
  /// @param variable Variable of which to compute the Jacobian.
  /// @param wrt Variable with respect to which to compute the Jacobian.
  Jacobian(Variable<Scalar> variable, Variable<Scalar> wrt)
      : Jacobian{VariableMatrix<Scalar>{std::move(variable)},
                 VariableMatrix<Scalar>{std::move(wrt)}} {}

  /// Constructs a Jacobian object.
  ///
  /// @param variable Variable of which to compute the Jacobian.
  /// @param wrt Vector of variables with respect to which to compute the
  ///     Jacobian.
  Jacobian(Variable<Scalar> variable, SleipnirMatrixLike<Scalar> auto wrt)
      : Jacobian{VariableMatrix<Scalar>{std::move(variable)}, std::move(wrt)} {}

  /// Constructs a Jacobian object.
  ///
  /// @param variables Vector of variables of which to compute the Jacobian.
  /// @param wrt Vector of variables with respect to which to compute the
  ///     Jacobian.
  Jacobian(VariableMatrix<Scalar> variables,
           SleipnirMatrixLike<Scalar> auto wrt)
      : m_variables{std::move(variables)}, m_wrt{std::move(wrt)} {
    slp_assert(m_variables.cols() == 1);
    slp_assert(m_wrt.cols() == 1);

    for (auto& variable : m_variables) {
      m_top_lists.emplace_back(detail::topological_sort(variable.expr));
    }

    // Initialize column each expression's adjoint occupies in the Jacobian
    for (size_t col = 0; col < m_wrt.size(); ++col) {
      m_wrt[col].expr->scratch = col;
    }

    // Make list of output rows as column-node pairs
    for (auto& top_list : m_top_lists) {
      m_output_lists.emplace_back();
      for (const auto& node : top_list) {
        if (node->scratch != -1) {
          m_output_lists.back().emplace_back(node->scratch, node);
        }
      }
    }

    // Reset col to -1
    for (auto& node : m_wrt) {
      node.expr->scratch = -1;
    }

    for (int row = 0; row < m_variables.rows(); ++row) {
      if (m_variables[row].expr == nullptr) {
        continue;
      }

      if (m_variables[row].type() == ExpressionType::LINEAR) {
        // If the row is linear, compute its gradient once here and cache its
        // triplets. Constant rows are ignored because their gradients have no
        // nonzero triplets.
        detail::append_triplets(m_top_lists[row], m_output_lists[row],
                                m_cached_triplets, row);
      } else if (m_variables[row].type() > ExpressionType::LINEAR) {
        // If the row is quadratic or nonlinear, add it to the list of nonlinear
        // rows to be recomputed in value().
        m_nonlinear_rows.emplace_back(row);
      }
    }

    if (m_nonlinear_rows.empty()) {
      m_J.setFromTriplets(m_cached_triplets.begin(), m_cached_triplets.end());
    }
  }

  /// Returns the Jacobian as a VariableMatrix.
  ///
  /// This is useful when constructing optimization problems with derivatives in
  /// them.
  ///
  /// @return The Jacobian as a VariableMatrix.
  VariableMatrix<Scalar> get() const {
    VariableMatrix<Scalar> result{detail::empty, m_variables.rows(),
                                  m_wrt.rows()};

    for (int row = 0; row < m_variables.rows(); ++row) {
      auto grad = detail::gradient_tree(m_top_lists[row], m_wrt);
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

  /// Evaluates the Jacobian at wrt's value.
  ///
  /// @return The Jacobian at wrt's value.
  const Eigen::SparseMatrix<Scalar>& value() {
    if (m_nonlinear_rows.empty()) {
      return m_J;
    }

    for (auto& top_list : m_top_lists) {
      detail::update_values(top_list);
    }

    // Copy the cached triplets so triplets added for the nonlinear rows are
    // thrown away at the end of the function
    auto triplets = m_cached_triplets;

    // Compute each nonlinear row of the Jacobian
    for (int row : m_nonlinear_rows) {
      detail::append_triplets(m_top_lists[row], m_output_lists[row], triplets,
                              row);
    }

    m_J.setFromTriplets(triplets.begin(), triplets.end());

    return m_J;
  }

 private:
  VariableMatrix<Scalar> m_variables;
  VariableMatrix<Scalar> m_wrt;

  /// List of topologically sorted graphs from parent to child, one for each row
  gch::small_vector<detail::ExpressionGraph<Scalar>> m_top_lists;

  /// List of output rows as column-node pairs
  gch::small_vector<
      gch::small_vector<std::pair<int, detail::Expression<Scalar>*>>>
      m_output_lists;

  Eigen::SparseMatrix<Scalar> m_J{m_variables.rows(), m_wrt.rows()};

  /// Cached triplets for gradients of linear rows
  gch::small_vector<Eigen::Triplet<Scalar>> m_cached_triplets;

  /// List of row indices for nonlinear rows whose graients will be computed in
  /// value()
  gch::small_vector<int> m_nonlinear_rows;
};

extern template class EXPORT_TEMPLATE_DECLARE(SLEIPNIR_DLLEXPORT)
Jacobian<double>;

}  // namespace slp
