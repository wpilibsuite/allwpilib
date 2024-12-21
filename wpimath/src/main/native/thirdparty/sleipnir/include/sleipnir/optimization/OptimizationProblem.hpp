// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <functional>
#include <iterator>
#include <optional>
#include <utility>

#include <Eigen/Core>
#include <wpi/SmallVector.h>

#include "sleipnir/autodiff/Variable.hpp"
#include "sleipnir/autodiff/VariableMatrix.hpp"
#include "sleipnir/optimization/SolverConfig.hpp"
#include "sleipnir/optimization/SolverExitCondition.hpp"
#include "sleipnir/optimization/SolverIterationInfo.hpp"
#include "sleipnir/optimization/SolverStatus.hpp"
#include "sleipnir/optimization/solver/InteriorPoint.hpp"
#include "sleipnir/optimization/solver/SQP.hpp"
#include "sleipnir/util/Print.hpp"
#include "sleipnir/util/SymbolExports.hpp"

namespace sleipnir {

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
class SLEIPNIR_DLLEXPORT OptimizationProblem {
 public:
  /**
   * Construct the optimization problem.
   */
  OptimizationProblem() noexcept = default;

  /**
   * Create a decision variable in the optimization problem.
   */
  [[nodiscard]]
  Variable DecisionVariable() {
    m_decisionVariables.emplace_back();
    return m_decisionVariables.back();
  }

  /**
   * Create a matrix of decision variables in the optimization problem.
   *
   * @param rows Number of matrix rows.
   * @param cols Number of matrix columns.
   */
  [[nodiscard]]
  VariableMatrix DecisionVariable(int rows, int cols = 1) {
    m_decisionVariables.reserve(m_decisionVariables.size() + rows * cols);

    VariableMatrix vars{rows, cols};

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        m_decisionVariables.emplace_back();
        vars(row, col) = m_decisionVariables.back();
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
   */
  [[nodiscard]]
  VariableMatrix SymmetricDecisionVariable(int rows) {
    // We only need to store the lower triangle of an n x n symmetric matrix;
    // the other elements are duplicates. The lower triangle has (n² + n)/2
    // elements.
    //
    //   n
    //   Σ k = (n² + n)/2
    //  k=1
    m_decisionVariables.reserve(m_decisionVariables.size() +
                                (rows * rows + rows) / 2);

    VariableMatrix vars{rows, rows};

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col <= row; ++col) {
        m_decisionVariables.emplace_back();
        vars(row, col) = m_decisionVariables.back();
        vars(col, row) = m_decisionVariables.back();
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
  void Minimize(const Variable& cost) {
    m_f = cost;
    status.costFunctionType = m_f.value().Type();
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
  void Minimize(Variable&& cost) {
    m_f = std::move(cost);
    status.costFunctionType = m_f.value().Type();
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
  void Maximize(const Variable& objective) {
    // Maximizing a cost function is the same as minimizing its negative
    m_f = -objective;
    status.costFunctionType = m_f.value().Type();
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
  void Maximize(Variable&& objective) {
    // Maximizing a cost function is the same as minimizing its negative
    m_f = -std::move(objective);
    status.costFunctionType = m_f.value().Type();
  }

  /**
   * Tells the solver to solve the problem while satisfying the given equality
   * constraint.
   *
   * @param constraint The constraint to satisfy.
   */
  void SubjectTo(const EqualityConstraints& constraint) {
    // Get the highest order equality constraint expression type
    for (const auto& c : constraint.constraints) {
      status.equalityConstraintType =
          std::max(status.equalityConstraintType, c.Type());
    }

    m_equalityConstraints.reserve(m_equalityConstraints.size() +
                                  constraint.constraints.size());
    std::copy(constraint.constraints.begin(), constraint.constraints.end(),
              std::back_inserter(m_equalityConstraints));
  }

  /**
   * Tells the solver to solve the problem while satisfying the given equality
   * constraint.
   *
   * @param constraint The constraint to satisfy.
   */
  void SubjectTo(EqualityConstraints&& constraint) {
    // Get the highest order equality constraint expression type
    for (const auto& c : constraint.constraints) {
      status.equalityConstraintType =
          std::max(status.equalityConstraintType, c.Type());
    }

    m_equalityConstraints.reserve(m_equalityConstraints.size() +
                                  constraint.constraints.size());
    std::copy(constraint.constraints.begin(), constraint.constraints.end(),
              std::back_inserter(m_equalityConstraints));
  }

  /**
   * Tells the solver to solve the problem while satisfying the given inequality
   * constraint.
   *
   * @param constraint The constraint to satisfy.
   */
  void SubjectTo(const InequalityConstraints& constraint) {
    // Get the highest order inequality constraint expression type
    for (const auto& c : constraint.constraints) {
      status.inequalityConstraintType =
          std::max(status.inequalityConstraintType, c.Type());
    }

    m_inequalityConstraints.reserve(m_inequalityConstraints.size() +
                                    constraint.constraints.size());
    std::copy(constraint.constraints.begin(), constraint.constraints.end(),
              std::back_inserter(m_inequalityConstraints));
  }

  /**
   * Tells the solver to solve the problem while satisfying the given inequality
   * constraint.
   *
   * @param constraint The constraint to satisfy.
   */
  void SubjectTo(InequalityConstraints&& constraint) {
    // Get the highest order inequality constraint expression type
    for (const auto& c : constraint.constraints) {
      status.inequalityConstraintType =
          std::max(status.inequalityConstraintType, c.Type());
    }

    m_inequalityConstraints.reserve(m_inequalityConstraints.size() +
                                    constraint.constraints.size());
    std::copy(constraint.constraints.begin(), constraint.constraints.end(),
              std::back_inserter(m_inequalityConstraints));
  }

  /**
   * Solve the optimization problem. The solution will be stored in the original
   * variables used to construct the problem.
   *
   * @param config Configuration options for the solver.
   */
  SolverStatus Solve(const SolverConfig& config = SolverConfig{}) {
    // Create the initial value column vector
    Eigen::VectorXd x{m_decisionVariables.size()};
    for (size_t i = 0; i < m_decisionVariables.size(); ++i) {
      x(i) = m_decisionVariables[i].Value();
    }

    status.exitCondition = SolverExitCondition::kSuccess;

    // If there's no cost function, make it zero and continue
    if (!m_f.has_value()) {
      m_f = Variable();
    }

    if (config.diagnostics) {
      constexpr std::array kExprTypeToName{"empty", "constant", "linear",
                                           "quadratic", "nonlinear"};

      // Print cost function and constraint expression types
      sleipnir::println(
          "The cost function is {}.",
          kExprTypeToName[static_cast<int>(status.costFunctionType)]);
      sleipnir::println(
          "The equality constraints are {}.",
          kExprTypeToName[static_cast<int>(status.equalityConstraintType)]);
      sleipnir::println(
          "The inequality constraints are {}.",
          kExprTypeToName[static_cast<int>(status.inequalityConstraintType)]);
      sleipnir::println("");

      // Print problem dimensionality
      sleipnir::println("Number of decision variables: {}",
                        m_decisionVariables.size());
      sleipnir::println("Number of equality constraints: {}",
                        m_equalityConstraints.size());
      sleipnir::println("Number of inequality constraints: {}\n",
                        m_inequalityConstraints.size());
    }

    // If the problem is empty or constant, there's nothing to do
    if (status.costFunctionType <= ExpressionType::kConstant &&
        status.equalityConstraintType <= ExpressionType::kConstant &&
        status.inequalityConstraintType <= ExpressionType::kConstant) {
      return status;
    }

    // Solve the optimization problem
    if (m_inequalityConstraints.empty()) {
      SQP(m_decisionVariables, m_equalityConstraints, m_f.value(), m_callback,
          config, x, &status);
    } else {
      Eigen::VectorXd s = Eigen::VectorXd::Ones(m_inequalityConstraints.size());
      InteriorPoint(m_decisionVariables, m_equalityConstraints,
                    m_inequalityConstraints, m_f.value(), m_callback, config,
                    false, x, s, &status);
    }

    if (config.diagnostics) {
      sleipnir::println("Exit condition: {}", ToMessage(status.exitCondition));
    }

    // Assign the solution to the original Variable instances
    VariableMatrix{m_decisionVariables}.SetValue(x);

    return status;
  }

  /**
   * Sets a callback to be called at each solver iteration.
   *
   * The callback for this overload should return void.
   *
   * @param callback The callback.
   */
  template <typename F>
    requires requires(F callback, const SolverIterationInfo& info) {
      { callback(info) } -> std::same_as<void>;
    }
  void Callback(F&& callback) {
    m_callback = [=, callback = std::forward<F>(callback)](
                     const SolverIterationInfo& info) {
      callback(info);
      return false;
    };
  }

  /**
   * Sets a callback to be called at each solver iteration.
   *
   * The callback for this overload should return bool.
   *
   * @param callback The callback. Returning true from the callback causes the
   *   solver to exit early with the solution it has so far.
   */
  template <typename F>
    requires requires(F callback, const SolverIterationInfo& info) {
      { callback(info) } -> std::same_as<bool>;
    }
  void Callback(F&& callback) {
    m_callback = std::forward<F>(callback);
  }

 private:
  // The list of decision variables, which are the root of the problem's
  // expression tree
  wpi::SmallVector<Variable> m_decisionVariables;

  // The cost function: f(x)
  std::optional<Variable> m_f;

  // The list of equality constraints: cₑ(x) = 0
  wpi::SmallVector<Variable> m_equalityConstraints;

  // The list of inequality constraints: cᵢ(x) ≥ 0
  wpi::SmallVector<Variable> m_inequalityConstraints;

  // The user callback
  std::function<bool(const SolverIterationInfo& info)> m_callback =
      [](const SolverIterationInfo&) { return false; };

  // The solver status
  SolverStatus status;
};

}  // namespace sleipnir
