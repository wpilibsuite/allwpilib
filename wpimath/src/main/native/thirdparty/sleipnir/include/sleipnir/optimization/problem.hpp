// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <iterator>
#include <optional>
#include <ranges>
#include <utility>

#include <Eigen/Core>
#include <gch/small_vector.hpp>

#include "sleipnir/autodiff/expression_type.hpp"
#include "sleipnir/autodiff/variable.hpp"
#include "sleipnir/autodiff/variable_matrix.hpp"
#include "sleipnir/optimization/solver/exit_status.hpp"
#include "sleipnir/optimization/solver/iteration_info.hpp"
#include "sleipnir/optimization/solver/options.hpp"
#include "sleipnir/util/symbol_exports.hpp"

namespace slp {

/**
 * This class allows the user to pose a constrained nonlinear optimization
 * problem in natural mathematical notation and solve it.
 *
 * This class supports problems of the form:
@verbatim
      minₓ f(x)
subject to cₑ(x) = 0
           cᵢ(x) ≥ 0
@endverbatim
 *
 * where f(x) is the scalar cost function, x is the vector of decision variables
 * (variables the solver can tweak to minimize the cost function), cᵢ(x) are the
 * inequality constraints, and cₑ(x) are the equality constraints. Constraints
 * are equations or inequalities of the decision variables that constrain what
 * values the solver is allowed to use when searching for an optimal solution.
 *
 * The nice thing about this class is users don't have to put their system in
 * the form shown above manually; they can write it in natural mathematical form
 * and it'll be converted for them.
 */
class SLEIPNIR_DLLEXPORT Problem {
 public:
  /**
   * Construct the optimization problem.
   */
  Problem() noexcept = default;

  /**
   * Create a decision variable in the optimization problem.
   *
   * @return A decision variable in the optimization problem.
   */
  [[nodiscard]]
  Variable decision_variable() {
    m_decision_variables.emplace_back();
    return m_decision_variables.back();
  }

  /**
   * Create a matrix of decision variables in the optimization problem.
   *
   * @param rows Number of matrix rows.
   * @param cols Number of matrix columns.
   * @return A matrix of decision variables in the optimization problem.
   */
  [[nodiscard]]
  VariableMatrix decision_variable(int rows, int cols = 1) {
    m_decision_variables.reserve(m_decision_variables.size() + rows * cols);

    VariableMatrix vars{VariableMatrix::empty, rows, cols};

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        m_decision_variables.emplace_back();
        vars(row, col) = m_decision_variables.back();
      }
    }

    return vars;
  }

  /**
   * Create a symmetric matrix of decision variables in the optimization
   * problem.
   *
   * Variable instances are reused across the diagonal, which helps reduce
   * problem dimensionality.
   *
   * @param rows Number of matrix rows.
   * @return A symmetric matrix of decision varaibles in the optimization
   *   problem.
   */
  [[nodiscard]]
  VariableMatrix symmetric_decision_variable(int rows) {
    // We only need to store the lower triangle of an n x n symmetric matrix;
    // the other elements are duplicates. The lower triangle has (n² + n)/2
    // elements.
    //
    //   n
    //   Σ k = (n² + n)/2
    //  k=1
    m_decision_variables.reserve(m_decision_variables.size() +
                                 (rows * rows + rows) / 2);

    VariableMatrix vars{VariableMatrix::empty, rows, rows};

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col <= row; ++col) {
        m_decision_variables.emplace_back();
        vars(row, col) = m_decision_variables.back();
        vars(col, row) = m_decision_variables.back();
      }
    }

    return vars;
  }

  /**
   * Tells the solver to minimize the output of the given cost function.
   *
   * Note that this is optional. If only constraints are specified, the solver
   * will find the closest solution to the initial conditions that's in the
   * feasible set.
   *
   * @param cost The cost function to minimize.
   */
  void minimize(const Variable& cost) { m_f = cost; }

  /**
   * Tells the solver to minimize the output of the given cost function.
   *
   * Note that this is optional. If only constraints are specified, the solver
   * will find the closest solution to the initial conditions that's in the
   * feasible set.
   *
   * @param cost The cost function to minimize.
   */
  void minimize(Variable&& cost) { m_f = std::move(cost); }

  /**
   * Tells the solver to maximize the output of the given objective function.
   *
   * Note that this is optional. If only constraints are specified, the solver
   * will find the closest solution to the initial conditions that's in the
   * feasible set.
   *
   * @param objective The objective function to maximize.
   */
  void maximize(const Variable& objective) {
    // Maximizing a cost function is the same as minimizing its negative
    m_f = -objective;
  }

  /**
   * Tells the solver to maximize the output of the given objective function.
   *
   * Note that this is optional. If only constraints are specified, the solver
   * will find the closest solution to the initial conditions that's in the
   * feasible set.
   *
   * @param objective The objective function to maximize.
   */
  void maximize(Variable&& objective) {
    // Maximizing a cost function is the same as minimizing its negative
    m_f = -std::move(objective);
  }

  /**
   * Tells the solver to solve the problem while satisfying the given equality
   * constraint.
   *
   * @param constraint The constraint to satisfy.
   */
  void subject_to(const EqualityConstraints& constraint) {
    m_equality_constraints.reserve(m_equality_constraints.size() +
                                   constraint.constraints.size());
    std::ranges::copy(constraint.constraints,
                      std::back_inserter(m_equality_constraints));
  }

  /**
   * Tells the solver to solve the problem while satisfying the given equality
   * constraint.
   *
   * @param constraint The constraint to satisfy.
   */
  void subject_to(EqualityConstraints&& constraint) {
    m_equality_constraints.reserve(m_equality_constraints.size() +
                                   constraint.constraints.size());
    std::ranges::copy(constraint.constraints,
                      std::back_inserter(m_equality_constraints));
  }

  /**
   * Tells the solver to solve the problem while satisfying the given inequality
   * constraint.
   *
   * @param constraint The constraint to satisfy.
   */
  void subject_to(const InequalityConstraints& constraint) {
    m_inequality_constraints.reserve(m_inequality_constraints.size() +
                                     constraint.constraints.size());
    std::ranges::copy(constraint.constraints,
                      std::back_inserter(m_inequality_constraints));
  }

  /**
   * Tells the solver to solve the problem while satisfying the given inequality
   * constraint.
   *
   * @param constraint The constraint to satisfy.
   */
  void subject_to(InequalityConstraints&& constraint) {
    m_inequality_constraints.reserve(m_inequality_constraints.size() +
                                     constraint.constraints.size());
    std::ranges::copy(constraint.constraints,
                      std::back_inserter(m_inequality_constraints));
  }

  /**
   * Returns the cost function's type.
   *
   * @return The cost function's type.
   */
  ExpressionType cost_function_type() const {
    if (m_f) {
      return m_f.value().type();
    } else {
      return ExpressionType::NONE;
    }
  }

  /**
   * Returns the type of the highest order equality constraint.
   *
   * @return The type of the highest order equality constraint.
   */
  ExpressionType equality_constraint_type() const {
    if (!m_equality_constraints.empty()) {
      return std::ranges::max(m_equality_constraints, {}, &Variable::type)
          .type();
    } else {
      return ExpressionType::NONE;
    }
  }

  /**
   * Returns the type of the highest order inequality constraint.
   *
   * @return The type of the highest order inequality constraint.
   */
  ExpressionType inequality_constraint_type() const {
    if (!m_inequality_constraints.empty()) {
      return std::ranges::max(m_inequality_constraints, {}, &Variable::type)
          .type();
    } else {
      return ExpressionType::NONE;
    }
  }

  /**
   * Solve the optimization problem. The solution will be stored in the original
   * variables used to construct the problem.
   *
   * @param options Solver options.
   * @param spy Enables writing sparsity patterns of H, Aₑ, and Aᵢ to files
   *   named H.spy, A_e.spy, and A_i.spy respectively during solve. Use
   *   tools/spy.py to plot them.
   * @return The solver status.
   */
  ExitStatus solve(const Options& options = Options{},
                   [[maybe_unused]] bool spy = false);

  /**
   * Adds a callback to be called at the beginning of each solver iteration.
   *
   * The callback for this overload should return void.
   *
   * @param callback The callback.
   */
  template <typename F>
    requires requires(F callback, const IterationInfo& info) {
      { callback(info) } -> std::same_as<void>;
    }
  void add_callback(F&& callback) {
    m_iteration_callbacks.emplace_back(
        [=, callback = std::forward<F>(callback)](const IterationInfo& info) {
          callback(info);
          return false;
        });
  }

  /**
   * Adds a callback to be called at the beginning of each solver iteration.
   *
   * The callback for this overload should return bool.
   *
   * @param callback The callback. Returning true from the callback causes the
   *   solver to exit early with the solution it has so far.
   */
  template <typename F>
    requires requires(F callback, const IterationInfo& info) {
      { callback(info) } -> std::same_as<bool>;
    }
  void add_callback(F&& callback) {
    m_iteration_callbacks.emplace_back(std::forward<F>(callback));
  }

  /**
   * Clears the registered callbacks.
   */
  void clear_callbacks() { m_iteration_callbacks.clear(); }

  /**
   * Adds a callback to be called at the beginning of each solver iteration.
   *
   * Language bindings should call this in the Problem constructor to register
   * callbacks that shouldn't be removed by clear_callbacks(). Persistent
   * callbacks run after non-persistent callbacks.
   *
   * @param callback The callback. Returning true from the callback causes the
   *   solver to exit early with the solution it has so far.
   */
  template <typename F>
    requires requires(F callback, const IterationInfo& info) {
      { callback(info) } -> std::same_as<bool>;
    }
  void add_persistent_callback(F&& callback) {
    m_persistent_iteration_callbacks.emplace_back(std::forward<F>(callback));
  }

 private:
  // The list of decision variables, which are the root of the problem's
  // expression tree
  gch::small_vector<Variable> m_decision_variables;

  // The cost function: f(x)
  std::optional<Variable> m_f;

  // The list of equality constraints: cₑ(x) = 0
  gch::small_vector<Variable> m_equality_constraints;

  // The list of inequality constraints: cᵢ(x) ≥ 0
  gch::small_vector<Variable> m_inequality_constraints;

  // The iteration callbacks
  gch::small_vector<std::function<bool(const IterationInfo& info)>>
      m_iteration_callbacks;
  gch::small_vector<std::function<bool(const IterationInfo& info)>>
      m_persistent_iteration_callbacks;

  void print_exit_conditions([[maybe_unused]] const Options& options);
  void print_problem_analysis();
};

}  // namespace slp
