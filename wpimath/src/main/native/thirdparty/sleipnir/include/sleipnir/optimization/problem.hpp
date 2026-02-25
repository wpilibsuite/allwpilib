// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <utility>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <fmt/chrono.h>
#include <gch/small_vector.hpp>

#include "sleipnir/autodiff/expression_type.hpp"
#include "sleipnir/autodiff/gradient.hpp"
#include "sleipnir/autodiff/hessian.hpp"
#include "sleipnir/autodiff/jacobian.hpp"
#include "sleipnir/autodiff/variable.hpp"
#include "sleipnir/autodiff/variable_matrix.hpp"
#include "sleipnir/optimization/solver/exit_status.hpp"
#include "sleipnir/optimization/solver/interior_point.hpp"
#include "sleipnir/optimization/solver/iteration_info.hpp"
#include "sleipnir/optimization/solver/newton.hpp"
#include "sleipnir/optimization/solver/options.hpp"
#include "sleipnir/optimization/solver/sqp.hpp"
#include "sleipnir/optimization/solver/util/bounds.hpp"
#include "sleipnir/util/empty.hpp"
#include "sleipnir/util/print.hpp"
#include "sleipnir/util/print_diagnostics.hpp"
#include "sleipnir/util/setup_profiler.hpp"
#include "sleipnir/util/spy.hpp"
#include "sleipnir/util/symbol_exports.hpp"

namespace slp {

/// This class allows the user to pose a constrained nonlinear optimization
/// problem in natural mathematical notation and solve it.
///
/// This class supports problems of the form:
///
/// ```
///       minₓ f(x)
/// subject to cₑ(x) = 0
///            cᵢ(x) ≥ 0
/// ```
///
/// where f(x) is the scalar cost function, x is the vector of decision
/// variables (variables the solver can tweak to minimize the cost function),
/// cᵢ(x) are the inequality constraints, and cₑ(x) are the equality
/// constraints. Constraints are equations or inequalities of the decision
/// variables that constrain what values the solver is allowed to use when
/// searching for an optimal solution.
///
/// The nice thing about this class is users don't have to put their system in
/// the form shown above manually; they can write it in natural mathematical
/// form and it'll be converted for them.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
class Problem {
 public:
  /// Construct the optimization problem.
  Problem() noexcept = default;

  /// Create a decision variable in the optimization problem.
  ///
  /// @return A decision variable in the optimization problem.
  [[nodiscard]]
  Variable<Scalar> decision_variable() {
    m_decision_variables.emplace_back();
    return m_decision_variables.back();
  }

  /// Create a matrix of decision variables in the optimization problem.
  ///
  /// @param rows Number of matrix rows.
  /// @param cols Number of matrix columns.
  /// @return A matrix of decision variables in the optimization problem.
  [[nodiscard]]
  VariableMatrix<Scalar> decision_variable(int rows, int cols = 1) {
    m_decision_variables.reserve(m_decision_variables.size() + rows * cols);

    VariableMatrix<Scalar> vars{detail::empty, rows, cols};

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        m_decision_variables.emplace_back();
        vars(row, col) = m_decision_variables.back();
      }
    }

    return vars;
  }

  /// Create a symmetric matrix of decision variables in the optimization
  /// problem.
  ///
  /// Variable instances are reused across the diagonal, which helps reduce
  /// problem dimensionality.
  ///
  /// @param rows Number of matrix rows.
  /// @return A symmetric matrix of decision varaibles in the optimization
  ///     problem.
  [[nodiscard]]
  VariableMatrix<Scalar> symmetric_decision_variable(int rows) {
    // We only need to store the lower triangle of an n x n symmetric matrix;
    // the other elements are duplicates. The lower triangle has (n² + n)/2
    // elements.
    //
    //   n
    //   Σ k = (n² + n)/2
    //  k=1
    m_decision_variables.reserve(m_decision_variables.size() +
                                 (rows * rows + rows) / 2);

    VariableMatrix<Scalar> vars{detail::empty, rows, rows};

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col <= row; ++col) {
        m_decision_variables.emplace_back();
        vars(row, col) = m_decision_variables.back();
        vars(col, row) = m_decision_variables.back();
      }
    }

    return vars;
  }

  /// Tells the solver to minimize the output of the given cost function.
  ///
  /// Note that this is optional. If only constraints are specified, the solver
  /// will find the closest solution to the initial conditions that's in the
  /// feasible set.
  ///
  /// @param cost The cost function to minimize.
  void minimize(const Variable<Scalar>& cost) { m_f = cost; }

  /// Tells the solver to minimize the output of the given cost function.
  ///
  /// Note that this is optional. If only constraints are specified, the solver
  /// will find the closest solution to the initial conditions that's in the
  /// feasible set.
  ///
  /// @param cost The cost function to minimize.
  void minimize(Variable<Scalar>&& cost) { m_f = std::move(cost); }

  /// Tells the solver to maximize the output of the given objective function.
  ///
  /// Note that this is optional. If only constraints are specified, the solver
  /// will find the closest solution to the initial conditions that's in the
  /// feasible set.
  ///
  /// @param objective The objective function to maximize.
  void maximize(const Variable<Scalar>& objective) {
    // Maximizing a cost function is the same as minimizing its negative
    m_f = -objective;
  }

  /// Tells the solver to maximize the output of the given objective function.
  ///
  /// Note that this is optional. If only constraints are specified, the solver
  /// will find the closest solution to the initial conditions that's in the
  /// feasible set.
  ///
  /// @param objective The objective function to maximize.
  void maximize(Variable<Scalar>&& objective) {
    // Maximizing a cost function is the same as minimizing its negative
    m_f = -std::move(objective);
  }

  /// Tells the solver to solve the problem while satisfying the given equality
  /// constraint.
  ///
  /// @param constraint The constraint to satisfy.
  void subject_to(const EqualityConstraints<Scalar>& constraint) {
    m_equality_constraints.reserve(m_equality_constraints.size() +
                                   constraint.constraints.size());
    std::ranges::copy(constraint.constraints,
                      std::back_inserter(m_equality_constraints));
  }

  /// Tells the solver to solve the problem while satisfying the given equality
  /// constraint.
  ///
  /// @param constraint The constraint to satisfy.
  void subject_to(EqualityConstraints<Scalar>&& constraint) {
    m_equality_constraints.reserve(m_equality_constraints.size() +
                                   constraint.constraints.size());
    std::ranges::copy(constraint.constraints,
                      std::back_inserter(m_equality_constraints));
  }

  /// Tells the solver to solve the problem while satisfying the given
  /// inequality constraint.
  ///
  /// @param constraint The constraint to satisfy.
  void subject_to(const InequalityConstraints<Scalar>& constraint) {
    m_inequality_constraints.reserve(m_inequality_constraints.size() +
                                     constraint.constraints.size());
    std::ranges::copy(constraint.constraints,
                      std::back_inserter(m_inequality_constraints));
  }

  /// Tells the solver to solve the problem while satisfying the given
  /// inequality constraint.
  ///
  /// @param constraint The constraint to satisfy.
  void subject_to(InequalityConstraints<Scalar>&& constraint) {
    m_inequality_constraints.reserve(m_inequality_constraints.size() +
                                     constraint.constraints.size());
    std::ranges::copy(constraint.constraints,
                      std::back_inserter(m_inequality_constraints));
  }

  /// Returns the cost function's type.
  ///
  /// @return The cost function's type.
  ExpressionType cost_function_type() const {
    if (m_f) {
      return m_f.value().type();
    } else {
      return ExpressionType::NONE;
    }
  }

  /// Returns the type of the highest order equality constraint.
  ///
  /// @return The type of the highest order equality constraint.
  ExpressionType equality_constraint_type() const {
    if (!m_equality_constraints.empty()) {
      return std::ranges::max(m_equality_constraints, {},
                              &Variable<Scalar>::type)
          .type();
    } else {
      return ExpressionType::NONE;
    }
  }

  /// Returns the type of the highest order inequality constraint.
  ///
  /// @return The type of the highest order inequality constraint.
  ExpressionType inequality_constraint_type() const {
    if (!m_inequality_constraints.empty()) {
      return std::ranges::max(m_inequality_constraints, {},
                              &Variable<Scalar>::type)
          .type();
    } else {
      return ExpressionType::NONE;
    }
  }

  /// Solve the optimization problem. The solution will be stored in the
  /// original variables used to construct the problem.
  ///
  /// @param options Solver options.
  /// @param spy Enables writing sparsity patterns of H, Aₑ, and Aᵢ to files
  ///     named H.spy, A_e.spy, and A_i.spy respectively during solve. Use
  ///     tools/spy.py to plot them.
  /// @return The solver status.
  ExitStatus solve(const Options& options = Options{},
                   [[maybe_unused]] bool spy = false) {
    using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
    using SparseMatrix = Eigen::SparseMatrix<Scalar>;
    using SparseVector = Eigen::SparseVector<Scalar>;

    // Create the initial value column vector
    DenseVector x{m_decision_variables.size()};
    for (size_t i = 0; i < m_decision_variables.size(); ++i) {
      x[i] = m_decision_variables[i].value();
    }

    if (options.diagnostics) {
      print_exit_conditions(options);
      print_problem_analysis();
    }

    // Get the highest order constraint expression types
    auto f_type = cost_function_type();
    auto c_e_type = equality_constraint_type();
    auto c_i_type = inequality_constraint_type();

    // If the problem is empty or constant, there's nothing to do
    if (f_type <= ExpressionType::CONSTANT &&
        c_e_type <= ExpressionType::CONSTANT &&
        c_i_type <= ExpressionType::CONSTANT) {
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
      if (options.diagnostics) {
        slp::println("\nInvoking no-op solver...\n");
      }
#endif
      return ExitStatus::SUCCESS;
    }

    gch::small_vector<SetupProfiler> ad_setup_profilers;
    ad_setup_profilers.emplace_back("setup").start();

    VariableMatrix<Scalar> x_ad{m_decision_variables};

    // Set up cost function
    Variable f = m_f.value_or(Scalar(0));

    // Set up gradient autodiff
    ad_setup_profilers.emplace_back("  ↳ ∇f(x)").start();
    Gradient g{f, x_ad};
    ad_setup_profilers.back().stop();

    [[maybe_unused]]
    int num_decision_variables = m_decision_variables.size();
    [[maybe_unused]]
    int num_equality_constraints = m_equality_constraints.size();
    [[maybe_unused]]
    int num_inequality_constraints = m_inequality_constraints.size();

    gch::small_vector<std::function<bool(const IterationInfo<Scalar>& info)>>
        callbacks;
    for (const auto& callback : m_iteration_callbacks) {
      callbacks.emplace_back(callback);
    }
    for (const auto& callback : m_persistent_iteration_callbacks) {
      callbacks.emplace_back(callback);
    }

    // Solve the optimization problem
    ExitStatus status;
    if (m_equality_constraints.empty() && m_inequality_constraints.empty()) {
      if (options.diagnostics) {
        slp::println("\nInvoking Newton solver...\n");
      }

      // Set up Lagrangian Hessian autodiff
      ad_setup_profilers.emplace_back("  ↳ ∇²ₓₓL").start();
      Hessian<Scalar, Eigen::Lower> H{f, x_ad};
      ad_setup_profilers.back().stop();

      ad_setup_profilers[0].stop();

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
      // Sparsity pattern files written when spy flag is set
      std::unique_ptr<Spy<Scalar>> H_spy;

      if (spy) {
        H_spy = std::make_unique<Spy<Scalar>>(
            "H.spy", "Hessian", "Decision variables", "Decision variables",
            num_decision_variables, num_decision_variables);
        callbacks.push_back([&](const IterationInfo<Scalar>& info) -> bool {
          H_spy->add(info.H);
          return false;
        });
      }
#endif

      // Invoke Newton solver
      status = newton<Scalar>(NewtonMatrixCallbacks<Scalar>{
                                  [&](const DenseVector& x) -> Scalar {
                                    x_ad.set_value(x);
                                    return f.value();
                                  },
                                  [&](const DenseVector& x) -> SparseVector {
                                    x_ad.set_value(x);
                                    return g.value();
                                  },
                                  [&](const DenseVector& x) -> SparseMatrix {
                                    x_ad.set_value(x);
                                    return H.value();
                                  }},
                              callbacks, options, x);
    } else if (m_inequality_constraints.empty()) {
      if (options.diagnostics) {
        slp::println("\nInvoking SQP solver\n");
      }

      VariableMatrix<Scalar> c_e_ad{m_equality_constraints};

      // Set up equality constraint Jacobian autodiff
      ad_setup_profilers.emplace_back("  ↳ ∂cₑ/∂x").start();
      Jacobian A_e{c_e_ad, x_ad};
      ad_setup_profilers.back().stop();

      // Set up Lagrangian
      VariableMatrix<Scalar> y_ad(num_equality_constraints);
      Variable L = f - y_ad.T() * c_e_ad;

      // Set up Lagrangian Hessian autodiff
      ad_setup_profilers.emplace_back("  ↳ ∇²ₓₓL").start();
      Hessian<Scalar, Eigen::Lower> H{L, x_ad};
      ad_setup_profilers.back().stop();

      ad_setup_profilers[0].stop();

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
      // Sparsity pattern files written when spy flag is set
      std::unique_ptr<Spy<Scalar>> H_spy;
      std::unique_ptr<Spy<Scalar>> A_e_spy;

      if (spy) {
        H_spy = std::make_unique<Spy<Scalar>>(
            "H.spy", "Hessian", "Decision variables", "Decision variables",
            num_decision_variables, num_decision_variables);
        A_e_spy = std::make_unique<Spy<Scalar>>(
            "A_e.spy", "Equality constraint Jacobian", "Constraints",
            "Decision variables", num_equality_constraints,
            num_decision_variables);
        callbacks.push_back([&](const IterationInfo<Scalar>& info) -> bool {
          H_spy->add(info.H);
          A_e_spy->add(info.A_e);
          return false;
        });
      }
#endif

      // Invoke SQP solver
      status = sqp<Scalar>(
          SQPMatrixCallbacks<Scalar>{
              [&](const DenseVector& x) -> Scalar {
                x_ad.set_value(x);
                return f.value();
              },
              [&](const DenseVector& x) -> SparseVector {
                x_ad.set_value(x);
                return g.value();
              },
              [&](const DenseVector& x, const DenseVector& y) -> SparseMatrix {
                x_ad.set_value(x);
                y_ad.set_value(y);
                return H.value();
              },
              [&](const DenseVector& x) -> DenseVector {
                x_ad.set_value(x);
                return c_e_ad.value();
              },
              [&](const DenseVector& x) -> SparseMatrix {
                x_ad.set_value(x);
                return A_e.value();
              }},
          callbacks, options, x);
    } else {
      if (options.diagnostics) {
        slp::println("\nInvoking IPM solver...\n");
      }

      VariableMatrix<Scalar> c_e_ad{m_equality_constraints};
      VariableMatrix<Scalar> c_i_ad{m_inequality_constraints};

      // Set up equality constraint Jacobian autodiff
      ad_setup_profilers.emplace_back("  ↳ ∂cₑ/∂x").start();
      Jacobian A_e{c_e_ad, x_ad};
      ad_setup_profilers.back().stop();

      // Set up inequality constraint Jacobian autodiff
      ad_setup_profilers.emplace_back("  ↳ ∂cᵢ/∂x").start();
      Jacobian A_i{c_i_ad, x_ad};
      ad_setup_profilers.back().stop();

      // Set up Lagrangian
      VariableMatrix<Scalar> y_ad(num_equality_constraints);
      VariableMatrix<Scalar> z_ad(num_inequality_constraints);
      Variable L = f - y_ad.T() * c_e_ad - z_ad.T() * c_i_ad;

      // Set up Lagrangian Hessian autodiff
      ad_setup_profilers.emplace_back("  ↳ ∇²ₓₓL").start();
      Hessian<Scalar, Eigen::Lower> H{L, x_ad};
      ad_setup_profilers.back().stop();

      ad_setup_profilers[0].stop();

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
      // Sparsity pattern files written when spy flag is set
      std::unique_ptr<Spy<Scalar>> H_spy;
      std::unique_ptr<Spy<Scalar>> A_e_spy;
      std::unique_ptr<Spy<Scalar>> A_i_spy;

      if (spy) {
        H_spy = std::make_unique<Spy<Scalar>>(
            "H.spy", "Hessian", "Decision variables", "Decision variables",
            num_decision_variables, num_decision_variables);
        A_e_spy = std::make_unique<Spy<Scalar>>(
            "A_e.spy", "Equality constraint Jacobian", "Constraints",
            "Decision variables", num_equality_constraints,
            num_decision_variables);
        A_i_spy = std::make_unique<Spy<Scalar>>(
            "A_i.spy", "Inequality constraint Jacobian", "Constraints",
            "Decision variables", num_inequality_constraints,
            num_decision_variables);
        callbacks.push_back([&](const IterationInfo<Scalar>& info) -> bool {
          H_spy->add(info.H);
          A_e_spy->add(info.A_e);
          A_i_spy->add(info.A_i);
          return false;
        });
      }
#endif

      const auto [bound_constraint_mask, bounds, conflicting_bound_indices] =
          get_bounds<Scalar>(m_decision_variables, m_inequality_constraints,
                             A_i.value());
      if (!conflicting_bound_indices.empty()) {
        if (options.diagnostics) {
          print_bound_constraint_global_infeasibility_error(
              conflicting_bound_indices);
        }
        return ExitStatus::GLOBALLY_INFEASIBLE;
      }

#ifdef SLEIPNIR_ENABLE_BOUND_PROJECTION
      project_onto_bounds(x, bounds);
#endif
      // Invoke interior-point method solver
      status = interior_point<Scalar>(
          InteriorPointMatrixCallbacks<Scalar>{
              [&](const DenseVector& x) -> Scalar {
                x_ad.set_value(x);
                return f.value();
              },
              [&](const DenseVector& x) -> SparseVector {
                x_ad.set_value(x);
                return g.value();
              },
              [&](const DenseVector& x, const DenseVector& y,
                  const DenseVector& z) -> SparseMatrix {
                x_ad.set_value(x);
                y_ad.set_value(y);
                z_ad.set_value(z);
                return H.value();
              },
              [&](const DenseVector& x) -> DenseVector {
                x_ad.set_value(x);
                return c_e_ad.value();
              },
              [&](const DenseVector& x) -> SparseMatrix {
                x_ad.set_value(x);
                return A_e.value();
              },
              [&](const DenseVector& x) -> DenseVector {
                x_ad.set_value(x);
                return c_i_ad.value();
              },
              [&](const DenseVector& x) -> SparseMatrix {
                x_ad.set_value(x);
                return A_i.value();
              }},
          callbacks, options,
#ifdef SLEIPNIR_ENABLE_BOUND_PROJECTION
          bound_constraint_mask,
#endif
          x);
    }

    if (options.diagnostics) {
      print_autodiff_diagnostics(ad_setup_profilers);
      slp::println("\nExit: {}", status);
    }

    // Assign the solution to the original Variable instances
    VariableMatrix<Scalar>{m_decision_variables}.set_value(x);

    return status;
  }

  /// Adds a callback to be called at the beginning of each solver iteration.
  ///
  /// The callback for this overload should return void.
  ///
  /// @param callback The callback.
  template <typename F>
    requires requires(F callback, const IterationInfo<Scalar>& info) {
      { callback(info) } -> std::same_as<void>;
    }
  void add_callback(F&& callback) {
    m_iteration_callbacks.emplace_back(
        [=, callback =
                std::forward<F>(callback)](const IterationInfo<Scalar>& info) {
          callback(info);
          return false;
        });
  }

  /// Adds a callback to be called at the beginning of each solver iteration.
  ///
  /// The callback for this overload should return bool.
  ///
  /// @param callback The callback. Returning true from the callback causes the
  ///     solver to exit early with the solution it has so far.
  template <typename F>
    requires requires(F callback, const IterationInfo<Scalar>& info) {
      { callback(info) } -> std::same_as<bool>;
    }
  void add_callback(F&& callback) {
    m_iteration_callbacks.emplace_back(std::forward<F>(callback));
  }

  /// Clears the registered callbacks.
  void clear_callbacks() { m_iteration_callbacks.clear(); }

  /// Adds a callback to be called at the beginning of each solver iteration.
  ///
  /// Language bindings should call this in the Problem constructor to register
  /// callbacks that shouldn't be removed by clear_callbacks(). Persistent
  /// callbacks run after non-persistent callbacks.
  ///
  /// @param callback The callback. Returning true from the callback causes the
  ///     solver to exit early with the solution it has so far.
  template <typename F>
    requires requires(F callback, const IterationInfo<Scalar>& info) {
      { callback(info) } -> std::same_as<bool>;
    }
  void add_persistent_callback(F&& callback) {
    m_persistent_iteration_callbacks.emplace_back(std::forward<F>(callback));
  }

 private:
  // The list of decision variables, which are the root of the problem's
  // expression tree
  gch::small_vector<Variable<Scalar>> m_decision_variables;

  // The cost function: f(x)
  std::optional<Variable<Scalar>> m_f;

  // The list of equality constraints: cₑ(x) = 0
  gch::small_vector<Variable<Scalar>> m_equality_constraints;

  // The list of inequality constraints: cᵢ(x) ≥ 0
  gch::small_vector<Variable<Scalar>> m_inequality_constraints;

  // The iteration callbacks
  gch::small_vector<std::function<bool(const IterationInfo<Scalar>& info)>>
      m_iteration_callbacks;
  gch::small_vector<std::function<bool(const IterationInfo<Scalar>& info)>>
      m_persistent_iteration_callbacks;

  void print_exit_conditions([[maybe_unused]] const Options& options) {
    // Print possible exit conditions
    slp::println("User-configured exit conditions:");
    slp::println("  ↳ error below {}", options.tolerance);
    if (!m_iteration_callbacks.empty() ||
        !m_persistent_iteration_callbacks.empty()) {
      slp::println("  ↳ iteration callback requested stop");
    }
    if (std::isfinite(options.max_iterations)) {
      slp::println("  ↳ executed {} iterations", options.max_iterations);
    }
    if (std::isfinite(options.timeout.count())) {
      slp::println("  ↳ {} elapsed", options.timeout);
    }
  }

  void print_problem_analysis() {
    constexpr std::array types{"no", "constant", "linear", "quadratic",
                               "nonlinear"};

    // Print problem structure
    slp::println("\nProblem structure:");
    slp::println("  ↳ {} cost function",
                 types[static_cast<uint8_t>(cost_function_type())]);
    slp::println("  ↳ {} equality constraints",
                 types[static_cast<uint8_t>(equality_constraint_type())]);
    slp::println("  ↳ {} inequality constraints",
                 types[static_cast<uint8_t>(inequality_constraint_type())]);

    if (m_decision_variables.size() == 1) {
      slp::print("\n1 decision variable\n");
    } else {
      slp::print("\n{} decision variables\n", m_decision_variables.size());
    }

    auto print_constraint_types =
        [](const gch::small_vector<Variable<Scalar>>& constraints) {
          std::array<size_t, 5> counts{};
          for (const auto& constraint : constraints) {
            ++counts[static_cast<uint8_t>(constraint.type())];
          }
          for (size_t i = 0; i < counts.size(); ++i) {
            constexpr std::array names{"empty", "constant", "linear",
                                       "quadratic", "nonlinear"};
            const auto& count = counts[i];
            const auto& name = names[i];
            if (count > 0) {
              slp::println("  ↳ {} {}", count, name);
            }
          }
        };

    // Print constraint types
    if (m_equality_constraints.size() == 1) {
      slp::println("1 equality constraint");
    } else {
      slp::println("{} equality constraints", m_equality_constraints.size());
    }
    print_constraint_types(m_equality_constraints);
    if (m_inequality_constraints.size() == 1) {
      slp::println("1 inequality constraint");
    } else {
      slp::println("{} inequality constraints",
                   m_inequality_constraints.size());
    }
    print_constraint_types(m_inequality_constraints);
  }
};

extern template class EXPORT_TEMPLATE_DECLARE(SLEIPNIR_DLLEXPORT)
Problem<double>;

}  // namespace slp
