// Copyright (c) Sleipnir contributors

#include "sleipnir/optimization/problem.hpp"

#include <array>
#include <cmath>
#include <memory>
#include <optional>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <fmt/chrono.h>
#include <gch/small_vector.hpp>

#include "optimization/bounds.hpp"
#include "sleipnir/autodiff/expression_type.hpp"
#include "sleipnir/autodiff/gradient.hpp"
#include "sleipnir/autodiff/hessian.hpp"
#include "sleipnir/autodiff/jacobian.hpp"
#include "sleipnir/autodiff/variable.hpp"
#include "sleipnir/autodiff/variable_matrix.hpp"
#include "sleipnir/optimization/solver/exit_status.hpp"
#include "sleipnir/optimization/solver/interior_point.hpp"
#include "sleipnir/optimization/solver/newton.hpp"
#include "sleipnir/optimization/solver/options.hpp"
#include "sleipnir/optimization/solver/sqp.hpp"
#include "sleipnir/util/print.hpp"
#include "sleipnir/util/spy.hpp"
#include "util/print_diagnostics.hpp"
#include "util/setup_profiler.hpp"

namespace slp {

ExitStatus Problem::solve(const Options& options, [[maybe_unused]] bool spy) {
  // Create the initial value column vector
  Eigen::VectorXd x{m_decision_variables.size()};
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

  VariableMatrix x_ad{m_decision_variables};

  // Set up cost function
  Variable f = m_f.value_or(0.0);

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

  gch::small_vector<std::function<bool(const IterationInfo& info)>> callbacks;
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
    Hessian<Eigen::Lower> H{f, x_ad};
    ad_setup_profilers.back().stop();

    ad_setup_profilers[0].stop();

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
    // Sparsity pattern files written when spy flag is set
    std::unique_ptr<Spy> H_spy;

    if (spy) {
      H_spy = std::make_unique<Spy>(
          "H.spy", "Hessian", "Decision variables", "Decision variables",
          num_decision_variables, num_decision_variables);
      callbacks.push_back([&](const IterationInfo& info) -> bool {
        H_spy->add(info.H);
        return false;
      });
    }
#endif

    // Invoke Newton solver
    status = newton(
        NewtonMatrixCallbacks{
            [&](const Eigen::VectorXd& x) -> double {
              x_ad.set_value(x);
              return f.value();
            },
            [&](const Eigen::VectorXd& x) -> Eigen::SparseVector<double> {
              x_ad.set_value(x);
              return g.value();
            },
            [&](const Eigen::VectorXd& x) -> Eigen::SparseMatrix<double> {
              x_ad.set_value(x);
              return H.value();
            }},
        callbacks, options, x);
  } else if (m_inequality_constraints.empty()) {
    if (options.diagnostics) {
      slp::println("\nInvoking SQP solver\n");
    }

    VariableMatrix c_e_ad{m_equality_constraints};

    // Set up equality constraint Jacobian autodiff
    ad_setup_profilers.emplace_back("  ↳ ∂cₑ/∂x").start();
    Jacobian A_e{c_e_ad, x_ad};
    ad_setup_profilers.back().stop();

    // Set up Lagrangian
    VariableMatrix y_ad(num_equality_constraints);
    Variable L = f - (y_ad.T() * c_e_ad)[0];

    // Set up Lagrangian Hessian autodiff
    ad_setup_profilers.emplace_back("  ↳ ∇²ₓₓL").start();
    Hessian<Eigen::Lower> H{L, x_ad};
    ad_setup_profilers.back().stop();

    ad_setup_profilers[0].stop();

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
    // Sparsity pattern files written when spy flag is set
    std::unique_ptr<Spy> H_spy;
    std::unique_ptr<Spy> A_e_spy;

    if (spy) {
      H_spy = std::make_unique<Spy>(
          "H.spy", "Hessian", "Decision variables", "Decision variables",
          num_decision_variables, num_decision_variables);
      A_e_spy = std::make_unique<Spy>("A_e.spy", "Equality constraint Jacobian",
                                      "Constraints", "Decision variables",
                                      num_equality_constraints,
                                      num_decision_variables);
      callbacks.push_back([&](const IterationInfo& info) -> bool {
        H_spy->add(info.H);
        A_e_spy->add(info.A_e);
        return false;
      });
    }
#endif

    // Invoke SQP solver
    status =
        sqp(SQPMatrixCallbacks{
                [&](const Eigen::VectorXd& x) -> double {
                  x_ad.set_value(x);
                  return f.value();
                },
                [&](const Eigen::VectorXd& x) -> Eigen::SparseVector<double> {
                  x_ad.set_value(x);
                  return g.value();
                },
                [&](const Eigen::VectorXd& x,
                    const Eigen::VectorXd& y) -> Eigen::SparseMatrix<double> {
                  x_ad.set_value(x);
                  y_ad.set_value(y);
                  return H.value();
                },
                [&](const Eigen::VectorXd& x) -> Eigen::VectorXd {
                  x_ad.set_value(x);
                  return c_e_ad.value();
                },
                [&](const Eigen::VectorXd& x) -> Eigen::SparseMatrix<double> {
                  x_ad.set_value(x);
                  return A_e.value();
                }},
            callbacks, options, x);
  } else {
    if (options.diagnostics) {
      slp::println("\nInvoking IPM solver...\n");
    }

    VariableMatrix c_e_ad{m_equality_constraints};
    VariableMatrix c_i_ad{m_inequality_constraints};

    // Set up equality constraint Jacobian autodiff
    ad_setup_profilers.emplace_back("  ↳ ∂cₑ/∂x").start();
    Jacobian A_e{c_e_ad, x_ad};
    ad_setup_profilers.back().stop();

    // Set up inequality constraint Jacobian autodiff
    ad_setup_profilers.emplace_back("  ↳ ∂cᵢ/∂x").start();
    Jacobian A_i{c_i_ad, x_ad};
    ad_setup_profilers.back().stop();

    // Set up Lagrangian
    VariableMatrix y_ad(num_equality_constraints);
    VariableMatrix z_ad(num_inequality_constraints);
    Variable L = f - (y_ad.T() * c_e_ad)[0] - (z_ad.T() * c_i_ad)[0];

    // Set up Lagrangian Hessian autodiff
    ad_setup_profilers.emplace_back("  ↳ ∇²ₓₓL").start();
    Hessian<Eigen::Lower> H{L, x_ad};
    ad_setup_profilers.back().stop();

    ad_setup_profilers[0].stop();

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
    // Sparsity pattern files written when spy flag is set
    std::unique_ptr<Spy> H_spy;
    std::unique_ptr<Spy> A_e_spy;
    std::unique_ptr<Spy> A_i_spy;

    if (spy) {
      H_spy = std::make_unique<Spy>(
          "H.spy", "Hessian", "Decision variables", "Decision variables",
          num_decision_variables, num_decision_variables);
      A_e_spy = std::make_unique<Spy>("A_e.spy", "Equality constraint Jacobian",
                                      "Constraints", "Decision variables",
                                      num_equality_constraints,
                                      num_decision_variables);
      A_i_spy = std::make_unique<Spy>(
          "A_i.spy", "Inequality constraint Jacobian", "Constraints",
          "Decision variables", num_inequality_constraints,
          num_decision_variables);
      callbacks.push_back([&](const IterationInfo& info) -> bool {
        H_spy->add(info.H);
        A_e_spy->add(info.A_e);
        A_i_spy->add(info.A_i);
        return false;
      });
    }
#endif

    const auto [bound_constraint_mask, bounds, conflicting_bound_indices] =
        get_bounds(m_decision_variables, m_inequality_constraints, A_i.value());
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
    status = interior_point(
        InteriorPointMatrixCallbacks{
            [&](const Eigen::VectorXd& x) -> double {
              x_ad.set_value(x);
              return f.value();
            },
            [&](const Eigen::VectorXd& x) -> Eigen::SparseVector<double> {
              x_ad.set_value(x);
              return g.value();
            },
            [&](const Eigen::VectorXd& x, const Eigen::VectorXd& y,
                const Eigen::VectorXd& z) -> Eigen::SparseMatrix<double> {
              x_ad.set_value(x);
              y_ad.set_value(y);
              z_ad.set_value(z);
              return H.value();
            },
            [&](const Eigen::VectorXd& x) -> Eigen::VectorXd {
              x_ad.set_value(x);
              return c_e_ad.value();
            },
            [&](const Eigen::VectorXd& x) -> Eigen::SparseMatrix<double> {
              x_ad.set_value(x);
              return A_e.value();
            },
            [&](const Eigen::VectorXd& x) -> Eigen::VectorXd {
              x_ad.set_value(x);
              return c_i_ad.value();
            },
            [&](const Eigen::VectorXd& x) -> Eigen::SparseMatrix<double> {
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
    slp::println("\nExit: {}", to_message(status));
  }

  // Assign the solution to the original Variable instances
  VariableMatrix{m_decision_variables}.set_value(x);

  return status;
}

void Problem::print_exit_conditions([[maybe_unused]] const Options& options) {
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

void Problem::print_problem_analysis() {
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
      [](const gch::small_vector<Variable>& constraints) {
        std::array<size_t, 5> counts{};
        for (const auto& constraint : constraints) {
          ++counts[static_cast<uint8_t>(constraint.type())];
        }
        for (size_t i = 0; i < counts.size(); ++i) {
          constexpr std::array names{"empty", "constant", "linear", "quadratic",
                                     "nonlinear"};
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
    slp::println("{} inequality constraints", m_inequality_constraints.size());
  }
  print_constraint_types(m_inequality_constraints);
}

}  // namespace slp
