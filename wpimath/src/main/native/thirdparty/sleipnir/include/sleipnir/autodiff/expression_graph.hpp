// Copyright (c) Sleipnir contributors

#pragma once

#include <ranges>

#include <gch/small_vector.hpp>

#include "sleipnir/autodiff/expression.hpp"

namespace slp::detail {

/**
 * Generate a topological sort of an expression graph from parent to child.
 *
 * https://en.wikipedia.org/wiki/Topological_sorting
 *
 * @param root The root node of the expression.
 */
inline gch::small_vector<Expression*> topological_sort(
    const ExpressionPtr& root) {
  gch::small_vector<Expression*> list;

  // If the root type is constant, updates are a no-op, so return an empty list
  if (root == nullptr || root->type() == ExpressionType::CONSTANT) {
    return list;
  }

  // Stack of nodes to explore
  gch::small_vector<Expression*> stack;

  // Enumerate incoming edges for each node via depth-first search
  stack.emplace_back(root.get());
  while (!stack.empty()) {
    auto node = stack.back();
    stack.pop_back();

    for (auto& arg : node->args) {
      // If the node hasn't been explored yet, add it to the stack
      if (arg != nullptr && ++arg->incoming_edges == 1) {
        stack.push_back(arg.get());
      }
    }
  }

  // Generate topological sort of graph from parent to child.
  //
  // A node is only added to the stack after all its incoming edges have been
  // traversed. Expression::incoming_edges is a decrementing counter for
  // tracking this.
  //
  // https://en.wikipedia.org/wiki/Topological_sorting
  stack.emplace_back(root.get());
  while (!stack.empty()) {
    auto node = stack.back();
    stack.pop_back();

    list.emplace_back(node);

    for (auto& arg : node->args) {
      // If we traversed all this node's incoming edges, add it to the stack
      if (arg != nullptr && --arg->incoming_edges == 0) {
        stack.push_back(arg.get());
      }
    }
  }

  return list;
}

/**
 * Update the values of all nodes in this graph based on the values of
 * their dependent nodes.
 *
 * @param list Topological sort of graph from parent to child.
 */
inline void update_values(const gch::small_vector<Expression*>& list) {
  // Traverse graph from child to parent and update values
  for (auto& node : list | std::views::reverse) {
    auto& lhs = node->args[0];
    auto& rhs = node->args[1];

    if (lhs != nullptr) {
      node->val = node->value(lhs->val, rhs ? rhs->val : 0.0);
    }
  }
}

}  // namespace slp::detail
