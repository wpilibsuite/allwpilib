// Copyright (c) Sleipnir contributors

#pragma once

#include <span>

#include "sleipnir/autodiff/Expression.hpp"
#include "sleipnir/util/FunctionRef.hpp"
#include "sleipnir/util/SmallVector.hpp"
#include "sleipnir/util/SymbolExports.hpp"

namespace sleipnir::detail {

/**
 * This class is an adaptor type that performs value updates of an expression's
 * computational graph in a way that skips duplicates.
 */
class SLEIPNIR_DLLEXPORT ExpressionGraph {
 public:
  /**
   * Generates the deduplicated computational graph for the given expression.
   *
   * @param root The root node of the expression.
   */
  explicit ExpressionGraph(ExpressionPtr& root) {
    // If the root type is a constant, Update() is a no-op, so there's no work
    // to do
    if (root == nullptr || root->type == ExpressionType::kConstant) {
      return;
    }

    // Breadth-first search (BFS) is used as opposed to a depth-first search
    // (DFS) to avoid counting duplicate nodes multiple times. A list of nodes
    // ordered from parent to child with no duplicates is generated.
    //
    // https://en.wikipedia.org/wiki/Breadth-first_search

    // BFS list sorted from parent to child.
    small_vector<Expression*> stack;

    stack.emplace_back(root.Get());

    // Initialize the number of instances of each node in the tree
    // (Expression::duplications)
    while (!stack.empty()) {
      auto currentNode = stack.back();
      stack.pop_back();

      for (auto&& arg : currentNode->args) {
        // Only continue if the node is not a constant and hasn't already been
        // explored.
        if (arg != nullptr && arg->type != ExpressionType::kConstant) {
          // If this is the first instance of the node encountered (it hasn't
          // been explored yet), add it to stack so it's recursed upon
          if (arg->duplications == 0) {
            stack.push_back(arg.Get());
          }
          ++arg->duplications;
        }
      }
    }

    stack.emplace_back(root.Get());

    while (!stack.empty()) {
      auto currentNode = stack.back();
      stack.pop_back();

      // BFS lists sorted from parent to child.
      m_rowList.emplace_back(currentNode->row);
      m_adjointList.emplace_back(currentNode);
      if (currentNode->valueFunc != nullptr) {
        // Constants are skipped because they have no valueFunc and don't need
        // to be updated
        m_valueList.emplace_back(currentNode);
      }

      for (auto&& arg : currentNode->args) {
        // Only add node if it's not a constant and doesn't already exist in the
        // tape.
        if (arg != nullptr && arg->type != ExpressionType::kConstant) {
          // Once the number of node visitations equals the number of
          // duplications (the counter hits zero), add it to the stack. Note
          // that this means the node is only enqueued once.
          --arg->duplications;
          if (arg->duplications == 0) {
            stack.push_back(arg.Get());
          }
        }
      }
    }
  }

  /**
   * Update the values of all nodes in this computational tree based on the
   * values of their dependent nodes.
   */
  void Update() {
    // Traverse the BFS list backward from child to parent and update the value
    // of each node.
    for (auto it = m_valueList.rbegin(); it != m_valueList.rend(); ++it) {
      auto& node = *it;

      auto& lhs = node->args[0];
      auto& rhs = node->args[1];

      if (lhs != nullptr) {
        if (rhs != nullptr) {
          node->value = node->valueFunc(lhs->value, rhs->value);
        } else {
          node->value = node->valueFunc(lhs->value, 0.0);
        }
      }
    }
  }

  /**
   * Returns the variable's gradient tree.
   *
   * @param wrt Variables with respect to which to compute the gradient.
   */
  small_vector<ExpressionPtr> GenerateGradientTree(
      std::span<const ExpressionPtr> wrt) const {
    // Read docs/algorithms.md#Reverse_accumulation_automatic_differentiation
    // for background on reverse accumulation automatic differentiation.

    for (size_t row = 0; row < wrt.size(); ++row) {
      wrt[row]->row = row;
    }

    small_vector<ExpressionPtr> grad;
    grad.reserve(wrt.size());
    for (size_t row = 0; row < wrt.size(); ++row) {
      grad.emplace_back(MakeExpressionPtr());
    }

    // Zero adjoints. The root node's adjoint is 1.0 as df/df is always 1.
    if (m_adjointList.size() > 0) {
      m_adjointList[0]->adjointExpr = MakeExpressionPtr(1.0);
      for (auto it = m_adjointList.begin() + 1; it != m_adjointList.end();
           ++it) {
        auto& node = *it;
        node->adjointExpr = MakeExpressionPtr();
      }
    }

    // df/dx = (df/dy)(dy/dx). The adjoint of x is equal to the adjoint of y
    // multiplied by dy/dx. If there are multiple "paths" from the root node to
    // variable; the variable's adjoint is the sum of each path's adjoint
    // contribution.
    for (auto node : m_adjointList) {
      auto& lhs = node->args[0];
      auto& rhs = node->args[1];

      if (lhs != nullptr && !lhs->IsConstant(0.0)) {
        lhs->adjointExpr = lhs->adjointExpr +
                           node->gradientFuncs[0](lhs, rhs, node->adjointExpr);
      }
      if (rhs != nullptr && !rhs->IsConstant(0.0)) {
        rhs->adjointExpr = rhs->adjointExpr +
                           node->gradientFuncs[1](lhs, rhs, node->adjointExpr);
      }

      // If variable is a leaf node, assign its adjoint to the gradient.
      if (node->row != -1) {
        grad[node->row] = node->adjointExpr;
      }
    }

    // Unlink adjoints to avoid circular references between them and their
    // parent expressions. This ensures all expressions are returned to the free
    // list.
    for (auto node : m_adjointList) {
      for (auto& arg : node->args) {
        if (arg != nullptr) {
          arg->adjointExpr = nullptr;
        }
      }
    }

    for (size_t row = 0; row < wrt.size(); ++row) {
      wrt[row]->row = -1;
    }

    return grad;
  }

  /**
   * Updates the adjoints in the expression graph, effectively computing the
   * gradient.
   *
   * @param func A function that takes two arguments: an int for the gradient
   *   row, and a double for the adjoint (gradient value).
   */
  void ComputeAdjoints(function_ref<void(int, double)> func) {
    // Zero adjoints. The root node's adjoint is 1.0 as df/df is always 1.
    m_adjointList[0]->adjoint = 1.0;
    for (auto it = m_adjointList.begin() + 1; it != m_adjointList.end(); ++it) {
      auto& node = *it;
      node->adjoint = 0.0;
    }

    // df/dx = (df/dy)(dy/dx). The adjoint of x is equal to the adjoint of y
    // multiplied by dy/dx. If there are multiple "paths" from the root node to
    // variable; the variable's adjoint is the sum of each path's adjoint
    // contribution.
    for (size_t col = 0; col < m_adjointList.size(); ++col) {
      auto& node = m_adjointList[col];
      auto& lhs = node->args[0];
      auto& rhs = node->args[1];

      if (lhs != nullptr) {
        if (rhs != nullptr) {
          lhs->adjoint += node->gradientValueFuncs[0](lhs->value, rhs->value,
                                                      node->adjoint);
          rhs->adjoint += node->gradientValueFuncs[1](lhs->value, rhs->value,
                                                      node->adjoint);
        } else {
          lhs->adjoint +=
              node->gradientValueFuncs[0](lhs->value, 0.0, node->adjoint);
        }
      }

      // If variable is a leaf node, assign its adjoint to the gradient.
      int row = m_rowList[col];
      if (row != -1) {
        func(row, node->adjoint);
      }
    }
  }

 private:
  // List that maps nodes to their respective row.
  small_vector<int> m_rowList;

  // List for updating adjoints
  small_vector<Expression*> m_adjointList;

  // List for updating values
  small_vector<Expression*> m_valueList;
};

}  // namespace sleipnir::detail
