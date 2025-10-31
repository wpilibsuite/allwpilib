// Copyright (c) Sleipnir contributors

#pragma once

#include <chrono>
#include <cmath>
#include <functional>
#include <limits>
#include <span>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <gch/small_vector.hpp>

#include "sleipnir/optimization/solver/exit_status.hpp"
#include "sleipnir/optimization/solver/iteration_info.hpp"
#include "sleipnir/optimization/solver/options.hpp"
#include "sleipnir/optimization/solver/sqp_matrix_callbacks.hpp"
#include "sleipnir/optimization/solver/util/error_estimate.hpp"
#include "sleipnir/optimization/solver/util/filter.hpp"
#include "sleipnir/optimization/solver/util/is_locally_infeasible.hpp"
#include "sleipnir/optimization/solver/util/kkt_error.hpp"
#include "sleipnir/optimization/solver/util/regularized_ldlt.hpp"
#include "sleipnir/util/assert.hpp"
#include "sleipnir/util/print_diagnostics.hpp"
#include "sleipnir/util/scope_exit.hpp"
#include "sleipnir/util/scoped_profiler.hpp"
#include "sleipnir/util/solve_profiler.hpp"
#include "sleipnir/util/symbol_exports.hpp"

// See docs/algorithms.md#Works_cited for citation definitions.

namespace slp {

/**
Finds the optimal solution to a nonlinear program using Sequential Quadratic
Programming (SQP).

A nonlinear program has the form:

@verbatim
     min_x f(x)
subject to cₑ(x) = 0
@endverbatim

where f(x) is the cost function and cₑ(x) are the equality constraints.

@tparam Scalar Scalar type.
@param[in] matrix_callbacks Matrix callbacks.
@param[in] iteration_callbacks The list of callbacks to call at the beginning of
  each iteration.
@param[in] options Solver options.
@param[in,out] x The initial guess and output location for the decision
  variables.
@return The exit status.
*/
template <typename Scalar>
ExitStatus sqp(const SQPMatrixCallbacks<Scalar>& matrix_callbacks,
               std::span<std::function<bool(const IterationInfo<Scalar>& info)>>
                   iteration_callbacks,
               const Options& options,
               Eigen::Vector<Scalar, Eigen::Dynamic>& x) {
  /**
   * SQP step direction.
   */
  struct Step {
    /// Primal step.
    Eigen::Vector<Scalar, Eigen::Dynamic> p_x;
    /// Dual step.
    Eigen::Vector<Scalar, Eigen::Dynamic> p_y;
  };

  using std::isfinite;

  const auto solve_start_time = std::chrono::steady_clock::now();

  gch::small_vector<SolveProfiler> solve_profilers;
  solve_profilers.emplace_back("solver");
  solve_profilers.emplace_back("  ↳ setup");
  solve_profilers.emplace_back("  ↳ iteration");
  solve_profilers.emplace_back("    ↳ feasibility ✓");
  solve_profilers.emplace_back("    ↳ iter callbacks");
  solve_profilers.emplace_back("    ↳ KKT matrix build");
  solve_profilers.emplace_back("    ↳ KKT matrix decomp");
  solve_profilers.emplace_back("    ↳ KKT system solve");
  solve_profilers.emplace_back("    ↳ line search");
  solve_profilers.emplace_back("      ↳ SOC");
  solve_profilers.emplace_back("    ↳ next iter prep");
  solve_profilers.emplace_back("    ↳ f(x)");
  solve_profilers.emplace_back("    ↳ ∇f(x)");
  solve_profilers.emplace_back("    ↳ ∇²ₓₓL");
  solve_profilers.emplace_back("    ↳ cₑ(x)");
  solve_profilers.emplace_back("    ↳ ∂cₑ/∂x");

  auto& solver_prof = solve_profilers[0];
  auto& setup_prof = solve_profilers[1];
  auto& inner_iter_prof = solve_profilers[2];
  auto& feasibility_check_prof = solve_profilers[3];
  auto& iter_callbacks_prof = solve_profilers[4];
  auto& kkt_matrix_build_prof = solve_profilers[5];
  auto& kkt_matrix_decomp_prof = solve_profilers[6];
  auto& kkt_system_solve_prof = solve_profilers[7];
  auto& line_search_prof = solve_profilers[8];
  auto& soc_prof = solve_profilers[9];
  auto& next_iter_prep_prof = solve_profilers[10];

  // Set up profiled matrix callbacks
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
  auto& f_prof = solve_profilers[11];
  auto& g_prof = solve_profilers[12];
  auto& H_prof = solve_profilers[13];
  auto& c_e_prof = solve_profilers[14];
  auto& A_e_prof = solve_profilers[15];

  SQPMatrixCallbacks<Scalar> matrices{
      [&](const Eigen::Vector<Scalar, Eigen::Dynamic>& x) -> Scalar {
        ScopedProfiler prof{f_prof};
        return matrix_callbacks.f(x);
      },
      [&](const Eigen::Vector<Scalar, Eigen::Dynamic>& x)
          -> Eigen::SparseVector<Scalar> {
        ScopedProfiler prof{g_prof};
        return matrix_callbacks.g(x);
      },
      [&](const Eigen::Vector<Scalar, Eigen::Dynamic>& x,
          const Eigen::Vector<Scalar, Eigen::Dynamic>& y)
          -> Eigen::SparseMatrix<Scalar> {
        ScopedProfiler prof{H_prof};
        return matrix_callbacks.H(x, y);
      },
      [&](const Eigen::Vector<Scalar, Eigen::Dynamic>& x)
          -> Eigen::Vector<Scalar, Eigen::Dynamic> {
        ScopedProfiler prof{c_e_prof};
        return matrix_callbacks.c_e(x);
      },
      [&](const Eigen::Vector<Scalar, Eigen::Dynamic>& x)
          -> Eigen::SparseMatrix<Scalar> {
        ScopedProfiler prof{A_e_prof};
        return matrix_callbacks.A_e(x);
      }};
#else
  const auto& matrices = matrix_callbacks;
#endif

  solver_prof.start();
  setup_prof.start();

  Scalar f = matrices.f(x);
  Eigen::Vector<Scalar, Eigen::Dynamic> c_e = matrices.c_e(x);

  int num_decision_variables = x.rows();
  int num_equality_constraints = c_e.rows();

  // Check for overconstrained problem
  if (num_equality_constraints > num_decision_variables) {
    if (options.diagnostics) {
      print_too_few_dofs_error(c_e);
    }

    return ExitStatus::TOO_FEW_DOFS;
  }

  Eigen::SparseVector<Scalar> g = matrices.g(x);
  Eigen::SparseMatrix<Scalar> A_e = matrices.A_e(x);

  Eigen::Vector<Scalar, Eigen::Dynamic> y =
      Eigen::Vector<Scalar, Eigen::Dynamic>::Zero(num_equality_constraints);

  Eigen::SparseMatrix<Scalar> H = matrices.H(x, y);

  // Ensure matrix callback dimensions are consistent
  slp_assert(g.rows() == num_decision_variables);
  slp_assert(A_e.rows() == num_equality_constraints);
  slp_assert(A_e.cols() == num_decision_variables);
  slp_assert(H.rows() == num_decision_variables);
  slp_assert(H.cols() == num_decision_variables);

  // Check whether initial guess has finite f(xₖ) and cₑ(xₖ)
  if (!isfinite(f) || !c_e.allFinite()) {
    return ExitStatus::NONFINITE_INITIAL_COST_OR_CONSTRAINTS;
  }

  int iterations = 0;

  Filter<Scalar> filter;

  // Kept outside the loop so its storage can be reused
  gch::small_vector<Eigen::Triplet<Scalar>> triplets;

  RegularizedLDLT<Scalar> solver{num_decision_variables,
                                 num_equality_constraints};

  // Variables for determining when a step is acceptable
  constexpr Scalar α_reduction_factor(0.5);
  constexpr Scalar α_min(1e-7);

  int full_step_rejected_counter = 0;

  // Error estimate
  Scalar E_0 = std::numeric_limits<Scalar>::infinity();

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

  while (E_0 > Scalar(options.tolerance)) {
    ScopedProfiler inner_iter_profiler{inner_iter_prof};
    ScopedProfiler feasibility_check_profiler{feasibility_check_prof};

    // Check for local equality constraint infeasibility
    if (is_equality_locally_infeasible(A_e, c_e)) {
      if (options.diagnostics) {
        print_c_e_local_infeasibility_error(c_e);
      }

      return ExitStatus::LOCALLY_INFEASIBLE;
    }

    // Check for diverging iterates
    if (x.template lpNorm<Eigen::Infinity>() > Scalar(1e10) || !x.allFinite()) {
      return ExitStatus::DIVERGING_ITERATES;
    }

    feasibility_check_profiler.stop();
    ScopedProfiler iter_callbacks_profiler{iter_callbacks_prof};

    // Call iteration callbacks
    for (const auto& callback : iteration_callbacks) {
      if (callback({iterations, x, g, H, A_e, Eigen::SparseMatrix<Scalar>{}})) {
        return ExitStatus::CALLBACK_REQUESTED_STOP;
      }
    }

    iter_callbacks_profiler.stop();
    ScopedProfiler kkt_matrix_build_profiler{kkt_matrix_build_prof};

    // lhs = [H   Aₑᵀ]
    //       [Aₑ   0 ]
    //
    // Don't assign upper triangle because solver only uses lower triangle.
    triplets.clear();
    triplets.reserve(H.nonZeros() + A_e.nonZeros());
    for (int col = 0; col < H.cols(); ++col) {
      // Append column of H lower triangle in top-left quadrant
      for (typename Eigen::SparseMatrix<Scalar>::InnerIterator it{H, col}; it;
           ++it) {
        triplets.emplace_back(it.row(), it.col(), it.value());
      }
      // Append column of Aₑ in bottom-left quadrant
      for (typename Eigen::SparseMatrix<Scalar>::InnerIterator it{A_e, col}; it;
           ++it) {
        triplets.emplace_back(H.rows() + it.row(), it.col(), it.value());
      }
    }
    Eigen::SparseMatrix<Scalar> lhs(
        num_decision_variables + num_equality_constraints,
        num_decision_variables + num_equality_constraints);
    lhs.setFromSortedTriplets(triplets.begin(), triplets.end());

    // rhs = −[∇f − Aₑᵀy]
    //        [   cₑ    ]
    Eigen::Vector<Scalar, Eigen::Dynamic> rhs{x.rows() + y.rows()};
    rhs.segment(0, x.rows()) = -g + A_e.transpose() * y;
    rhs.segment(x.rows(), y.rows()) = -c_e;

    kkt_matrix_build_profiler.stop();
    ScopedProfiler kkt_matrix_decomp_profiler{kkt_matrix_decomp_prof};

    Step step;
    constexpr Scalar α_max(1);
    Scalar α(1);

    // Solve the Newton-KKT system
    //
    // [H   Aₑᵀ][ pˣ] = −[∇f − Aₑᵀy]
    // [Aₑ   0 ][−pʸ]    [   cₑ    ]
    if (solver.compute(lhs).info() != Eigen::Success) [[unlikely]] {
      return ExitStatus::FACTORIZATION_FAILED;
    }

    kkt_matrix_decomp_profiler.stop();
    ScopedProfiler kkt_system_solve_profiler{kkt_system_solve_prof};

    auto compute_step = [&](Step& step) {
      // p = [ pˣ]
      //     [−pʸ]
      Eigen::Vector<Scalar, Eigen::Dynamic> p = solver.solve(rhs);
      step.p_x = p.segment(0, x.rows());
      step.p_y = -p.segment(x.rows(), y.rows());
    };
    compute_step(step);

    kkt_system_solve_profiler.stop();
    ScopedProfiler line_search_profiler{line_search_prof};

    α = α_max;

    // Loop until a step is accepted
    while (1) {
      Eigen::Vector<Scalar, Eigen::Dynamic> trial_x = x + α * step.p_x;
      Eigen::Vector<Scalar, Eigen::Dynamic> trial_y = y + α * step.p_y;

      Scalar trial_f = matrices.f(trial_x);
      Eigen::Vector<Scalar, Eigen::Dynamic> trial_c_e = matrices.c_e(trial_x);

      // If f(xₖ + αpₖˣ) or cₑ(xₖ + αpₖˣ) aren't finite, reduce step size
      // immediately
      if (!isfinite(trial_f) || !trial_c_e.allFinite()) {
        // Reduce step size
        α *= α_reduction_factor;

        if (α < α_min) {
          return ExitStatus::LINE_SEARCH_FAILED;
        }
        continue;
      }

      // Check whether filter accepts trial iterate
      if (filter.try_add(FilterEntry{trial_f, trial_c_e}, α)) {
        // Accept step
        break;
      }

      Scalar prev_constraint_violation = c_e.template lpNorm<1>();
      Scalar next_constraint_violation = trial_c_e.template lpNorm<1>();

      // Second-order corrections
      //
      // If first trial point was rejected and constraint violation stayed the
      // same or went up, apply second-order corrections
      if (α == α_max &&
          next_constraint_violation >= prev_constraint_violation) {
        // Apply second-order corrections. See section 2.4 of [2].
        auto soc_step = step;

        Scalar α_soc = α;
        Eigen::Vector<Scalar, Eigen::Dynamic> c_e_soc = c_e;

        bool step_acceptable = false;
        for (int soc_iteration = 0; soc_iteration < 5 && !step_acceptable;
             ++soc_iteration) {
          ScopedProfiler soc_profiler{soc_prof};

          scope_exit soc_exit{[&] {
            soc_profiler.stop();

            if (options.diagnostics) {
              print_iteration_diagnostics(
                  iterations,
                  step_acceptable ? IterationType::ACCEPTED_SOC
                                  : IterationType::REJECTED_SOC,
                  soc_profiler.current_duration(),
                  error_estimate<Scalar>(g, A_e, trial_c_e, trial_y), trial_f,
                  trial_c_e.template lpNorm<1>(), Scalar(0), Scalar(0),
                  solver.hessian_regularization(), α_soc, Scalar(1),
                  α_reduction_factor, Scalar(1));
            }
          }};

          // Rebuild Newton-KKT rhs with updated constraint values.
          //
          // rhs = −[∇f − Aₑᵀy]
          //        [  cₑˢᵒᶜ  ]
          //
          // where cₑˢᵒᶜ = αc(xₖ) + c(xₖ + αpₖˣ)
          c_e_soc = α_soc * c_e_soc + trial_c_e;
          rhs.bottomRows(y.rows()) = -c_e_soc;

          // Solve the Newton-KKT system
          compute_step(soc_step);

          trial_x = x + α_soc * soc_step.p_x;
          trial_y = y + α_soc * soc_step.p_y;

          trial_f = matrices.f(trial_x);
          trial_c_e = matrices.c_e(trial_x);

          // Constraint violation scale factor for second-order corrections
          constexpr Scalar κ_soc(0.99);

          // If constraint violation hasn't been sufficiently reduced, stop
          // making second-order corrections
          next_constraint_violation = trial_c_e.template lpNorm<1>();
          if (next_constraint_violation > κ_soc * prev_constraint_violation) {
            break;
          }

          // Check whether filter accepts trial iterate
          if (filter.try_add(FilterEntry{trial_f, trial_c_e}, α)) {
            step = soc_step;
            α = α_soc;
            step_acceptable = true;
          }
        }

        if (step_acceptable) {
          // Accept step
          break;
        }
      }

      // If we got here and α is the full step, the full step was rejected.
      // Increment the full-step rejected counter to keep track of how many full
      // steps have been rejected in a row.
      if (α == α_max) {
        ++full_step_rejected_counter;
      }

      // If the full step was rejected enough times in a row, reset the filter
      // because it may be impeding progress.
      //
      // See section 3.2 case I of [2].
      if (full_step_rejected_counter >= 4 &&
          filter.max_constraint_violation >
              filter.back().constraint_violation / Scalar(10)) {
        filter.max_constraint_violation *= Scalar(0.1);
        filter.reset();
        continue;
      }

      // Reduce step size
      α *= α_reduction_factor;

      // If step size hit a minimum, check if the KKT error was reduced. If it
      // wasn't, report line search failure.
      if (α < α_min) {
        Scalar current_kkt_error = kkt_error<Scalar>(g, A_e, c_e, y);

        trial_x = x + α_max * step.p_x;
        trial_y = y + α_max * step.p_y;

        trial_c_e = matrices.c_e(trial_x);

        Scalar next_kkt_error = kkt_error<Scalar>(
            matrices.g(trial_x), matrices.A_e(trial_x), trial_c_e, trial_y);

        // If the step using αᵐᵃˣ reduced the KKT error, accept it anyway
        if (next_kkt_error <= Scalar(0.999) * current_kkt_error) {
          α = α_max;

          // Accept step
          break;
        }

        return ExitStatus::LINE_SEARCH_FAILED;
      }
    }

    line_search_profiler.stop();

    // If full step was accepted, reset full-step rejected counter
    if (α == α_max) {
      full_step_rejected_counter = 0;
    }

    // xₖ₊₁ = xₖ + αₖpₖˣ
    // yₖ₊₁ = yₖ + αₖpₖʸ
    x += α * step.p_x;
    y += α * step.p_y;

    // Update autodiff for Jacobians and Hessian
    f = matrices.f(x);
    A_e = matrices.A_e(x);
    g = matrices.g(x);
    H = matrices.H(x, y);

    ScopedProfiler next_iter_prep_profiler{next_iter_prep_prof};

    c_e = matrices.c_e(x);

    // Update the error estimate
    E_0 = error_estimate<Scalar>(g, A_e, c_e, y);

    next_iter_prep_profiler.stop();
    inner_iter_profiler.stop();

    if (options.diagnostics) {
      print_iteration_diagnostics(iterations, IterationType::NORMAL,
                                  inner_iter_profiler.current_duration(), E_0,
                                  f, c_e.template lpNorm<1>(), Scalar(0),
                                  Scalar(0), solver.hessian_regularization(), α,
                                  α_max, α_reduction_factor, α);
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

extern template SLEIPNIR_DLLEXPORT ExitStatus
sqp(const SQPMatrixCallbacks<double>& matrix_callbacks,
    std::span<std::function<bool(const IterationInfo<double>& info)>>
        iteration_callbacks,
    const Options& options, Eigen::Vector<double, Eigen::Dynamic>& x);

}  // namespace slp
