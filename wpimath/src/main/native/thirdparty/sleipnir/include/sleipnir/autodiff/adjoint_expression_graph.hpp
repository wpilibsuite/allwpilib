// Copyright (c) Sleipnir contributors

#pragma once

#include <ranges>
#include <utility>

#include <Eigen/SparseCore>
#include <gch/small_vector.hpp>

#include "sleipnir/autodiff/expression_graph.hpp"
#include "sleipnir/autodiff/variable.hpp"
#include "sleipnir/autodiff/variable_matrix.hpp"
#include "sleipnir/util/assert.hpp"

namespace slp::detail {

/**
 * This class is an adaptor type that performs value updates of an expression's
 * adjoint graph.
 */
class AdjointExpressionGraph {
 public:
  /**
   * Generates the adjoint graph for the given expression.
   *
   * @param root The root node of the expression.
   */
  explicit AdjointExpressionGraph(const Variable& root)
      : m_top_list{topological_sort(root.expr)} {
    for (const auto& node : m_top_list) {
      m_col_list.emplace_back(node->col);
    }
  }

  /**
   * Update the values of all nodes in this adjoint graph based on the values of
   * their dependent nodes.
   */
  void update_values() { detail::update_values(m_top_list); }

  /**
   * Returns the variable's gradient tree.
   *
   * This function lazily allocates variables, so elements of the returned
   * VariableMatrix will be empty if the corresponding element of wrt had no
   * adjoint. Ensure Variable::expr isn't nullptr before calling member
   * functions.
   *
   * @param wrt Variables with respect to which to compute the gradient.
   * @return The variable's gradient tree.
   */
  VariableMatrix generate_gradient_tree(const VariableMatrix& wrt) const {
    slp_assert(wrt.cols() == 1);

    // Read docs/algorithms.md#Reverse_accumulation_automatic_differentiation
    // for background on reverse accumulation automatic differentiation.

    if (m_top_list.empty()) {
      return VariableMatrix{VariableMatrix::empty, wrt.rows(), 1};
    }

    // Set root node's adjoint to 1 since df/df is 1
    m_top_list[0]->adjoint_expr = make_expression_ptr<ConstExpression>(1.0);

    // df/dx = (df/dy)(dy/dx). The adjoint of x is equal to the adjoint of y
    // multiplied by dy/dx. If there are multiple "paths" from the root node to
    // variable; the variable's adjoint is the sum of each path's adjoint
    // contribution.
    for (auto& node : m_top_list) {
      auto& lhs = node->args[0];
      auto& rhs = node->args[1];

      if (lhs != nullptr) {
        lhs->adjoint_expr += node->grad_expr_l(lhs, rhs, node->adjoint_expr);
        if (rhs != nullptr) {
          rhs->adjoint_expr += node->grad_expr_r(lhs, rhs, node->adjoint_expr);
        }
      }
    }

    // Move gradient tree to return value
    VariableMatrix grad{VariableMatrix::empty, wrt.rows(), 1};
    for (int row = 0; row < grad.rows(); ++row) {
      grad[row] = Variable{std::move(wrt[row].expr->adjoint_expr)};
    }

    // Unlink adjoints to avoid circular references between them and their
    // parent expressions. This ensures all expressions are returned to the free
    // list.
    for (auto& node : m_top_list) {
      node->adjoint_expr = nullptr;
    }

    return grad;
  }

  /**
   * Updates the adjoints in the expression graph (computes the gradient) then
   * appends the adjoints of wrt to the sparse matrix triplets.
   *
   * @param triplets The sparse matrix triplets.
   * @param row The row of wrt.
   * @param wrt Vector of variables with respect to which to compute the
   *   Jacobian.
   */
  void append_adjoint_triplets(
      gch::small_vector<Eigen::Triplet<double>>& triplets, int row,
      const VariableMatrix& wrt) const {
    // Read docs/algorithms.md#Reverse_accumulation_automatic_differentiation
    // for background on reverse accumulation automatic differentiation.

    // If wrt has fewer nodes than graph, zero wrt's adjoints
    if (static_cast<size_t>(wrt.rows()) < m_top_list.size()) {
      for (const auto& elem : wrt) {
        elem.expr->adjoint = 0.0;
      }
    }

    if (m_top_list.empty()) {
      return;
    }

    // Set root node's adjoint to 1 since df/df is 1
    m_top_list[0]->adjoint = 1.0;

    // Zero the rest of the adjoints
    for (auto& node : m_top_list | std::views::drop(1)) {
      node->adjoint = 0.0;
    }

    // df/dx = (df/dy)(dy/dx). The adjoint of x is equal to the adjoint of y
    // multiplied by dy/dx. If there are multiple "paths" from the root node to
    // variable; the variable's adjoint is the sum of each path's adjoint
    // contribution.
    for (const auto& node : m_top_list) {
      auto& lhs = node->args[0];
      auto& rhs = node->args[1];

      if (lhs != nullptr) {
        if (rhs != nullptr) {
          lhs->adjoint += node->grad_l(lhs->val, rhs->val, node->adjoint);
          rhs->adjoint += node->grad_r(lhs->val, rhs->val, node->adjoint);
        } else {
          lhs->adjoint += node->grad_l(lhs->val, 0.0, node->adjoint);
        }
      }
    }

    // If wrt has fewer nodes than graph, iterate over wrt
    if (static_cast<size_t>(wrt.rows()) < m_top_list.size()) {
      for (int col = 0; col < wrt.rows(); ++col) {
        const auto& node = wrt[col].expr;

        // Append adjoints of wrt to sparse matrix triplets
        if (node->adjoint != 0.0) {
          triplets.emplace_back(row, col, node->adjoint);
        }
      }
    } else {
      for (size_t i = 0; i < m_top_list.size(); ++i) {
        const auto& col = m_col_list[i];
        const auto& node = m_top_list[i];

        // Append adjoints of wrt to sparse matrix triplets
        if (col != -1 && node->adjoint != 0.0) {
          triplets.emplace_back(row, col, node->adjoint);
        }
      }
    }
  }

 private:
  // Topological sort of graph from parent to child
  gch::small_vector<Expression*> m_top_list;

  // List that maps nodes to their respective column
  gch::small_vector<int> m_col_list;
};

}  // namespace slp::detail
