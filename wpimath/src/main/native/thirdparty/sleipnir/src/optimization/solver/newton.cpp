// Copyright (c) Sleipnir contributors

#include "sleipnir/optimization/solver/newton.hpp"

#include <chrono>
#include <cmath>
#include <functional>
#include <limits>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <gch/small_vector.hpp>

#include "optimization/regularized_ldlt.hpp"
#include "optimization/solver/util/error_estimate.hpp"
#include "optimization/solver/util/filter.hpp"
#include "optimization/solver/util/kkt_error.hpp"
#include "sleipnir/optimization/solver/exit_status.hpp"
#include "sleipnir/optimization/solver/iteration_info.hpp"
#include "sleipnir/optimization/solver/options.hpp"
#include "sleipnir/util/assert.hpp"
#include "util/print_diagnostics.hpp"
#include "util/scope_exit.hpp"
#include "util/scoped_profiler.hpp"
#include "util/solve_profiler.hpp"

// See docs/algorithms.md#Works_cited for citation definitions.

namespace slp {

ExitStatus newton(const NewtonMatrixCallbacks& matrix_callbacks,
                  std::span<std::function<bool(const IterationInfo& info)>>
                      iteration_callbacks,
                  const Options& options, Eigen::VectorXd& x) {
  const auto solve_start_time = std::chrono::steady_clock::now();

  gch::small_vector<SolveProfiler> solve_profilers;
  solve_profilers.emplace_back("solver");
  solve_profilers.emplace_back("  ↳ setup");
  solve_profilers.emplace_back("  ↳ iteration");
  solve_profilers.emplace_back("    ↳ feasibility ✓");
  solve_profilers.emplace_back("    ↳ iteration callbacks");
  solve_profilers.emplace_back("    ↳ iter matrix compute");
  solve_profilers.emplace_back("    ↳ iter matrix solve");
  solve_profilers.emplace_back("    ↳ line search");
  solve_profilers.emplace_back("    ↳ next iter prep");
  solve_profilers.emplace_back("    ↳ f(x)");
  solve_profilers.emplace_back("    ↳ ∇f(x)");
  solve_profilers.emplace_back("    ↳ ∇²ₓₓL");

  auto& solver_prof = solve_profilers[0];
  auto& setup_prof = solve_profilers[1];
  auto& inner_iter_prof = solve_profilers[2];
  auto& feasibility_check_prof = solve_profilers[3];
  auto& iteration_callbacks_prof = solve_profilers[4];
  auto& linear_system_compute_prof = solve_profilers[5];
  auto& linear_system_solve_prof = solve_profilers[6];
  auto& line_search_prof = solve_profilers[7];
  auto& next_iter_prep_prof = solve_profilers[8];

  // Set up profiled matrix callbacks
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
  auto& f_prof = solve_profilers[9];
  auto& g_prof = solve_profilers[10];
  auto& H_prof = solve_profilers[11];

  NewtonMatrixCallbacks matrices{
      [&](const Eigen::VectorXd& x) -> double {
        ScopedProfiler prof{f_prof};
        return matrix_callbacks.f(x);
      },
      [&](const Eigen::VectorXd& x) -> Eigen::SparseVector<double> {
        ScopedProfiler prof{g_prof};
        return matrix_callbacks.g(x);
      },
      [&](const Eigen::VectorXd& x) -> Eigen::SparseMatrix<double> {
        ScopedProfiler prof{H_prof};
        return matrix_callbacks.H(x);
      }};
#else
  const auto& matrices = matrix_callbacks;
#endif

  solver_prof.start();
  setup_prof.start();

  double f = matrices.f(x);

  int num_decision_variables = x.rows();

  Eigen::SparseVector<double> g = matrices.g(x);
  Eigen::SparseMatrix<double> H = matrices.H(x);

  // Ensure matrix callback dimensions are consistent
  slp_assert(g.rows() == num_decision_variables);
  slp_assert(H.rows() == num_decision_variables);
  slp_assert(H.cols() == num_decision_variables);

  // Check whether initial guess has finite f(xₖ)
  if (!std::isfinite(f)) {
    return ExitStatus::NONFINITE_INITIAL_COST_OR_CONSTRAINTS;
  }

  int iterations = 0;

  Filter filter;

  RegularizedLDLT solver{num_decision_variables, 0};

  // Variables for determining when a step is acceptable
  constexpr double α_reduction_factor = 0.5;
  constexpr double α_min = 1e-20;

  // Error estimate
  double E_0 = std::numeric_limits<double>::infinity();

  setup_prof.stop();

  // Prints final solver diagnostics when the solver exits
  scope_exit exit{[&] {
    if (options.diagnostics) {
      solver_prof.stop();
      if (iterations > 0) {
        print_bottom_iteration_diagnostics();
      }
      print_solver_diagnostics(solve_profilers);
    }
  }};

  while (E_0 > options.tolerance) {
    ScopedProfiler inner_iter_profiler{inner_iter_prof};
    ScopedProfiler feasibility_check_profiler{feasibility_check_prof};

    // Check for diverging iterates
    if (x.lpNorm<Eigen::Infinity>() > 1e10 || !x.allFinite()) {
      return ExitStatus::DIVERGING_ITERATES;
    }

    feasibility_check_profiler.stop();
    ScopedProfiler iteration_callbacks_profiler{iteration_callbacks_prof};

    // Call iteration callbacks
    for (const auto& callback : iteration_callbacks) {
      if (callback({iterations, x, g, H, Eigen::SparseMatrix<double>{},
                    Eigen::SparseMatrix<double>{}})) {
        return ExitStatus::CALLBACK_REQUESTED_STOP;
      }
    }

    iteration_callbacks_profiler.stop();
    ScopedProfiler linear_system_compute_profiler{linear_system_compute_prof};

    // Solve the Newton-KKT system
    //
    // Hpˣ = −∇f
    solver.compute(H);

    linear_system_compute_profiler.stop();
    ScopedProfiler linear_system_solve_profiler{linear_system_solve_prof};

    Eigen::VectorXd p_x = solver.solve(-g);

    linear_system_solve_profiler.stop();
    ScopedProfiler line_search_profiler{line_search_prof};

    constexpr double α_max = 1.0;
    double α = α_max;

    // Loop until a step is accepted. If a step becomes acceptable, the loop
    // will exit early.
    while (1) {
      Eigen::VectorXd trial_x = x + α * p_x;

      double trial_f = matrices.f(trial_x);

      // If f(xₖ + αpₖˣ) isn't finite, reduce step size immediately
      if (!std::isfinite(trial_f)) {
        // Reduce step size
        α *= α_reduction_factor;

        if (α < α_min) {
          return ExitStatus::LINE_SEARCH_FAILED;
        }
        continue;
      }

      // Check whether filter accepts trial iterate
      if (filter.try_add(FilterEntry{trial_f}, α)) {
        // Accept step
        break;
      }

      // Reduce step size
      α *= α_reduction_factor;

      // If step size hit a minimum, check if the KKT error was reduced. If it
      // wasn't, report bad line search.
      if (α < α_min) {
        double current_kkt_error = kkt_error(g);

        Eigen::VectorXd trial_x = x + α_max * p_x;

        double next_kkt_error = kkt_error(matrices.g(trial_x));

        // If the step using αᵐᵃˣ reduced the KKT error, accept it anyway
        if (next_kkt_error <= 0.999 * current_kkt_error) {
          α = α_max;

          // Accept step
          break;
        }

        return ExitStatus::LINE_SEARCH_FAILED;
      }
    }

    line_search_profiler.stop();

    // xₖ₊₁ = xₖ + αₖpₖˣ
    x += α * p_x;

    // Update autodiff for Hessian
    f = matrices.f(x);
    g = matrices.g(x);
    H = matrices.H(x);

    ScopedProfiler next_iter_prep_profiler{next_iter_prep_prof};

    // Update the error estimate
    E_0 = error_estimate(g);

    next_iter_prep_profiler.stop();
    inner_iter_profiler.stop();

    if (options.diagnostics) {
      print_iteration_diagnostics(
          iterations, IterationType::NORMAL,
          inner_iter_profiler.current_duration(), E_0, f, 0.0, 0.0, 0.0,
          solver.hessian_regularization(), α, α_max, α_reduction_factor, 1.0);
    }

    ++iterations;

    // Check for max iterations
    if (iterations >= options.max_iterations) {
      return ExitStatus::MAX_ITERATIONS_EXCEEDED;
    }

    // Check for max wall clock time
    if (std::chrono::steady_clock::now() - solve_start_time > options.timeout) {
      return ExitStatus::TIMEOUT;
    }
  }

  return ExitStatus::SUCCESS;
}

}  // namespace slp
