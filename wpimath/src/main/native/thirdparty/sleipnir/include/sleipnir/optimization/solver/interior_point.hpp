// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <limits>
#include <span>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <gch/small_vector.hpp>

#include "sleipnir/optimization/solver/exit_status.hpp"
#include "sleipnir/optimization/solver/interior_point_matrix_callbacks.hpp"
#include "sleipnir/optimization/solver/iteration_info.hpp"
#include "sleipnir/optimization/solver/options.hpp"
#include "sleipnir/optimization/solver/util/error_estimate.hpp"
#include "sleipnir/optimization/solver/util/filter.hpp"
#include "sleipnir/optimization/solver/util/fraction_to_the_boundary_rule.hpp"
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
//
// See docs/algorithms.md#Interior-point_method for a derivation of the
// interior-point method formulation being used.

namespace slp {

/// Finds the optimal solution to a nonlinear program using the interior-point
/// method.
///
/// A nonlinear program has the form:
///
/// ```
///      min_x f(x)
/// subject to cₑ(x) = 0
///            cᵢ(x) ≥ 0
/// ```
///
/// where f(x) is the cost function, cₑ(x) are the equality constraints, and
/// cᵢ(x) are the inequality constraints.
///
/// @tparam Scalar Scalar type.
/// @param[in] matrix_callbacks Matrix callbacks.
/// @param[in] iteration_callbacks The list of callbacks to call at the
///     beginning of each iteration.
/// @param[in] options Solver options.
/// @param[in,out] x The initial guess and output location for the decision
///     variables.
/// @return The exit status.
template <typename Scalar>
ExitStatus interior_point(
    const InteriorPointMatrixCallbacks<Scalar>& matrix_callbacks,
    std::span<std::function<bool(const IterationInfo<Scalar>& info)>>
        iteration_callbacks,
    const Options& options,
#ifdef SLEIPNIR_ENABLE_BOUND_PROJECTION
    const Eigen::ArrayX<bool>& bound_constraint_mask,
#endif
    Eigen::Vector<Scalar, Eigen::Dynamic>& x) {
  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
  using SparseMatrix = Eigen::SparseMatrix<Scalar>;
  using SparseVector = Eigen::SparseVector<Scalar>;

  /// Interior-point method step direction.
  struct Step {
    /// Primal step.
    DenseVector p_x;
    /// Equality constraint dual step.
    DenseVector p_y;
    /// Inequality constraint slack variable step.
    DenseVector p_s;
    /// Inequality constraint dual step.
    DenseVector p_z;
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
  solve_profilers.emplace_back("    ↳ cᵢ(x)");
  solve_profilers.emplace_back("    ↳ ∂cᵢ/∂x");

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
  auto& c_i_prof = solve_profilers[16];
  auto& A_i_prof = solve_profilers[17];

  InteriorPointMatrixCallbacks<Scalar> matrices{
      [&](const DenseVector& x) -> Scalar {
        ScopedProfiler prof{f_prof};
        return matrix_callbacks.f(x);
      },
      [&](const DenseVector& x) -> SparseVector {
        ScopedProfiler prof{g_prof};
        return matrix_callbacks.g(x);
      },
      [&](const DenseVector& x, const DenseVector& y,
          const DenseVector& z) -> SparseMatrix {
        ScopedProfiler prof{H_prof};
        return matrix_callbacks.H(x, y, z);
      },
      [&](const DenseVector& x) -> DenseVector {
        ScopedProfiler prof{c_e_prof};
        return matrix_callbacks.c_e(x);
      },
      [&](const DenseVector& x) -> SparseMatrix {
        ScopedProfiler prof{A_e_prof};
        return matrix_callbacks.A_e(x);
      },
      [&](const DenseVector& x) -> DenseVector {
        ScopedProfiler prof{c_i_prof};
        return matrix_callbacks.c_i(x);
      },
      [&](const DenseVector& x) -> SparseMatrix {
        ScopedProfiler prof{A_i_prof};
        return matrix_callbacks.A_i(x);
      }};
#else
  const auto& matrices = matrix_callbacks;
#endif

  solver_prof.start();
  setup_prof.start();

  Scalar f = matrices.f(x);
  DenseVector c_e = matrices.c_e(x);
  DenseVector c_i = matrices.c_i(x);

  int num_decision_variables = x.rows();
  int num_equality_constraints = c_e.rows();
  int num_inequality_constraints = c_i.rows();

  // Check for overconstrained problem
  if (num_equality_constraints > num_decision_variables) {
    if (options.diagnostics) {
      print_too_few_dofs_error(c_e);
    }

    return ExitStatus::TOO_FEW_DOFS;
  }

  SparseVector g = matrices.g(x);
  SparseMatrix A_e = matrices.A_e(x);
  SparseMatrix A_i = matrices.A_i(x);

  DenseVector s = DenseVector::Ones(num_inequality_constraints);
#ifdef SLEIPNIR_ENABLE_BOUND_PROJECTION
  // We set sʲ = cᵢʲ(x) for each bound inequality constraint index j
  s = bound_constraint_mask.select(c_i, s);
#endif
  DenseVector y = DenseVector::Zero(num_equality_constraints);
  DenseVector z = DenseVector::Ones(num_inequality_constraints);

  SparseMatrix H = matrices.H(x, y, z);

  // Ensure matrix callback dimensions are consistent
  slp_assert(g.rows() == num_decision_variables);
  slp_assert(A_e.rows() == num_equality_constraints);
  slp_assert(A_e.cols() == num_decision_variables);
  slp_assert(A_i.rows() == num_inequality_constraints);
  slp_assert(A_i.cols() == num_decision_variables);
  slp_assert(H.rows() == num_decision_variables);
  slp_assert(H.cols() == num_decision_variables);

  // Check whether initial guess has finite f(xₖ), cₑ(xₖ), and cᵢ(xₖ)
  if (!isfinite(f) || !c_e.allFinite() || !c_i.allFinite()) {
    return ExitStatus::NONFINITE_INITIAL_COST_OR_CONSTRAINTS;
  }

  int iterations = 0;

  // Barrier parameter minimum
  const Scalar μ_min = Scalar(options.tolerance) / Scalar(10);

  // Barrier parameter μ
  Scalar μ(0.1);

  // Fraction-to-the-boundary rule scale factor minimum
  constexpr Scalar τ_min(0.99);

  // Fraction-to-the-boundary rule scale factor τ
  Scalar τ = τ_min;

  Filter<Scalar> filter;

  // This should be run when the error estimate is below a desired threshold for
  // the current barrier parameter
  auto update_barrier_parameter_and_reset_filter = [&] {
    // Barrier parameter linear decrease power in "κ_μ μ". Range of (0, 1).
    constexpr Scalar κ_μ(0.2);

    // Barrier parameter superlinear decrease power in "μ^(θ_μ)". Range of (1,
    // 2).
    constexpr Scalar θ_μ(1.5);

    // Update the barrier parameter.
    //
    //   μⱼ₊₁ = max(εₜₒₗ/10, min(κ_μ μⱼ, μⱼ^θ_μ))
    //
    // See equation (7) of [2].
    using std::pow;
    μ = std::max(μ_min, std::min(κ_μ * μ, pow(μ, θ_μ)));

    // Update the fraction-to-the-boundary rule scaling factor.
    //
    //   τⱼ = max(τₘᵢₙ, 1 − μⱼ)
    //
    // See equation (8) of [2].
    τ = std::max(τ_min, Scalar(1) - μ);

    // Reset the filter when the barrier parameter is updated
    filter.reset();
  };

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

    // Check for local inequality constraint infeasibility
    if (is_inequality_locally_infeasible(A_i, c_i)) {
      if (options.diagnostics) {
        print_c_i_local_infeasibility_error(c_i);
      }

      return ExitStatus::LOCALLY_INFEASIBLE;
    }

    // Check for diverging iterates
    if (x.template lpNorm<Eigen::Infinity>() > Scalar(1e10) || !x.allFinite() ||
        s.template lpNorm<Eigen::Infinity>() > Scalar(1e10) || !s.allFinite()) {
      return ExitStatus::DIVERGING_ITERATES;
    }

    feasibility_check_profiler.stop();
    ScopedProfiler iter_callbacks_profiler{iter_callbacks_prof};

    // Call iteration callbacks
    for (const auto& callback : iteration_callbacks) {
      if (callback({iterations, x, g, H, A_e, A_i})) {
        return ExitStatus::CALLBACK_REQUESTED_STOP;
      }
    }

    iter_callbacks_profiler.stop();
    ScopedProfiler kkt_matrix_build_profiler{kkt_matrix_build_prof};

    // S = diag(s)
    // Z = diag(z)
    // Σ = S⁻¹Z
    const SparseMatrix Σ{s.cwiseInverse().asDiagonal() * z.asDiagonal()};

    // lhs = [H + AᵢᵀΣAᵢ  Aₑᵀ]
    //       [    Aₑ       0 ]
    //
    // Don't assign upper triangle because solver only uses lower triangle.
    const SparseMatrix top_left =
        H + (A_i.transpose() * Σ * A_i).template triangularView<Eigen::Lower>();
    triplets.clear();
    triplets.reserve(top_left.nonZeros() + A_e.nonZeros());
    for (int col = 0; col < H.cols(); ++col) {
      // Append column of H + AᵢᵀΣAᵢ lower triangle in top-left quadrant
      for (typename SparseMatrix::InnerIterator it{top_left, col}; it; ++it) {
        triplets.emplace_back(it.row(), it.col(), it.value());
      }
      // Append column of Aₑ in bottom-left quadrant
      for (typename SparseMatrix::InnerIterator it{A_e, col}; it; ++it) {
        triplets.emplace_back(H.rows() + it.row(), it.col(), it.value());
      }
    }
    SparseMatrix lhs(num_decision_variables + num_equality_constraints,
                     num_decision_variables + num_equality_constraints);
    lhs.setFromSortedTriplets(triplets.begin(), triplets.end());

    // rhs = −[∇f − Aₑᵀy − Aᵢᵀ(−Σcᵢ + μS⁻¹e + z)]
    //        [               cₑ                ]
    DenseVector rhs{x.rows() + y.rows()};
    rhs.segment(0, x.rows()) =
        -g + A_e.transpose() * y +
        A_i.transpose() * (-Σ * c_i + μ * s.cwiseInverse() + z);
    rhs.segment(x.rows(), y.rows()) = -c_e;

    kkt_matrix_build_profiler.stop();
    ScopedProfiler kkt_matrix_decomp_profiler{kkt_matrix_decomp_prof};

    Step step;
    Scalar α_max(1);
    Scalar α(1);
    Scalar α_z(1);

    // Solve the Newton-KKT system
    //
    // [H + AᵢᵀΣAᵢ  Aₑᵀ][ pˣ] = −[∇f − Aₑᵀy − Aᵢᵀ(−Σcᵢ + μS⁻¹e + z)]
    // [    Aₑ       0 ][−pʸ]    [               cₑ                ]
    if (solver.compute(lhs).info() != Eigen::Success) [[unlikely]] {
      return ExitStatus::FACTORIZATION_FAILED;
    }

    kkt_matrix_decomp_profiler.stop();
    ScopedProfiler kkt_system_solve_profiler{kkt_system_solve_prof};

    auto compute_step = [&](Step& step) {
      // p = [ pˣ]
      //     [−pʸ]
      DenseVector p = solver.solve(rhs);
      step.p_x = p.segment(0, x.rows());
      step.p_y = -p.segment(x.rows(), y.rows());

      // pˢ = cᵢ − s + Aᵢpˣ
      // pᶻ = −Σcᵢ + μS⁻¹e − ΣAᵢpˣ
      step.p_s = c_i - s + A_i * step.p_x;
      step.p_z = -Σ * c_i + μ * s.cwiseInverse() - Σ * A_i * step.p_x;
    };
    compute_step(step);

    kkt_system_solve_profiler.stop();
    ScopedProfiler line_search_profiler{line_search_prof};

    // αᵐᵃˣ = max(α ∈ (0, 1] : sₖ + αpₖˢ ≥ (1−τⱼ)sₖ)
    α_max = fraction_to_the_boundary_rule<Scalar>(s, step.p_s, τ);
    α = α_max;

    // If maximum step size is below minimum, report line search failure
    if (α < α_min) {
      return ExitStatus::LINE_SEARCH_FAILED;
    }

    // αₖᶻ = max(α ∈ (0, 1] : zₖ + αpₖᶻ ≥ (1−τⱼ)zₖ)
    α_z = fraction_to_the_boundary_rule<Scalar>(z, step.p_z, τ);

    // Loop until a step is accepted
    while (1) {
      DenseVector trial_x = x + α * step.p_x;
      DenseVector trial_y = y + α_z * step.p_y;
      DenseVector trial_z = z + α_z * step.p_z;

      Scalar trial_f = matrices.f(trial_x);
      DenseVector trial_c_e = matrices.c_e(trial_x);
      DenseVector trial_c_i = matrices.c_i(trial_x);

      // If f(xₖ + αpₖˣ), cₑ(xₖ + αpₖˣ), or cᵢ(xₖ + αpₖˣ) aren't finite, reduce
      // step size immediately
      if (!isfinite(trial_f) || !trial_c_e.allFinite() ||
          !trial_c_i.allFinite()) {
        // Reduce step size
        α *= α_reduction_factor;

        if (α < α_min) {
          return ExitStatus::LINE_SEARCH_FAILED;
        }
        continue;
      }

      DenseVector trial_s;
      if (options.feasible_ipm && c_i.cwiseGreater(Scalar(0)).all()) {
        // If the inequality constraints are all feasible, prevent them from
        // becoming infeasible again.
        //
        // See equation (19.30) in [1].
        trial_s = trial_c_i;
      } else {
        trial_s = s + α * step.p_s;
      }

      // Check whether filter accepts trial iterate
      if (filter.try_add(FilterEntry{trial_f, trial_s, trial_c_e, trial_c_i, μ},
                         α)) {
        // Accept step
        break;
      }

      Scalar prev_constraint_violation =
          c_e.template lpNorm<1>() + (c_i - s).template lpNorm<1>();
      Scalar next_constraint_violation =
          trial_c_e.template lpNorm<1>() +
          (trial_c_i - trial_s).template lpNorm<1>();

      // Second-order corrections
      //
      // If first trial point was rejected and constraint violation stayed the
      // same or went up, apply second-order corrections
      if (α == α_max &&
          next_constraint_violation >= prev_constraint_violation) {
        // Apply second-order corrections. See section 2.4 of [2].
        auto soc_step = step;

        Scalar α_soc = α;
        Scalar α_z_soc = α_z;
        DenseVector c_e_soc = c_e;

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
                  trial_c_e.template lpNorm<1>() +
                      (trial_c_i - trial_s).template lpNorm<1>(),
                  trial_s.dot(trial_z), μ, solver.hessian_regularization(),
                  α_soc, Scalar(1), α_reduction_factor, α_z_soc);
            }
          }};

          // Rebuild Newton-KKT rhs with updated constraint values.
          //
          // rhs = −[∇f − Aₑᵀy − Aᵢᵀ(−Σcᵢ + μS⁻¹e + z)]
          //        [              cₑˢᵒᶜ              ]
          //
          // where cₑˢᵒᶜ = αc(xₖ) + c(xₖ + αpₖˣ)
          c_e_soc = α_soc * c_e_soc + trial_c_e;
          rhs.bottomRows(y.rows()) = -c_e_soc;

          // Solve the Newton-KKT system
          compute_step(soc_step);

          // αˢᵒᶜ = max(α ∈ (0, 1] : sₖ + αpₖˢ ≥ (1−τⱼ)sₖ)
          α_soc = fraction_to_the_boundary_rule<Scalar>(s, soc_step.p_s, τ);
          trial_x = x + α_soc * soc_step.p_x;
          trial_s = s + α_soc * soc_step.p_s;

          // αₖᶻ = max(α ∈ (0, 1] : zₖ + αpₖᶻ ≥ (1−τⱼ)zₖ)
          α_z_soc = fraction_to_the_boundary_rule<Scalar>(z, soc_step.p_z, τ);
          trial_y = y + α_z_soc * soc_step.p_y;
          trial_z = z + α_z_soc * soc_step.p_z;

          trial_f = matrices.f(trial_x);
          trial_c_e = matrices.c_e(trial_x);
          trial_c_i = matrices.c_i(trial_x);

          // Constraint violation scale factor for second-order corrections
          constexpr Scalar κ_soc(0.99);

          // If constraint violation hasn't been sufficiently reduced, stop
          // making second-order corrections
          next_constraint_violation =
              trial_c_e.template lpNorm<1>() +
              (trial_c_i - trial_s).template lpNorm<1>();
          if (next_constraint_violation > κ_soc * prev_constraint_violation) {
            break;
          }

          // Check whether filter accepts trial iterate
          if (filter.try_add(
                  FilterEntry{trial_f, trial_s, trial_c_e, trial_c_i, μ}, α)) {
            step = soc_step;
            α = α_soc;
            α_z = α_z_soc;
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
        Scalar current_kkt_error =
            kkt_error<Scalar>(g, A_e, c_e, A_i, c_i, s, y, z, μ);

        trial_x = x + α_max * step.p_x;
        trial_s = s + α_max * step.p_s;

        trial_y = y + α_z * step.p_y;
        trial_z = z + α_z * step.p_z;

        trial_c_e = matrices.c_e(trial_x);
        trial_c_i = matrices.c_i(trial_x);

        Scalar next_kkt_error = kkt_error<Scalar>(
            matrices.g(trial_x), matrices.A_e(trial_x), matrices.c_e(trial_x),
            matrices.A_i(trial_x), trial_c_i, trial_s, trial_y, trial_z, μ);

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
    // sₖ₊₁ = sₖ + αₖpₖˢ
    // yₖ₊₁ = yₖ + αₖᶻpₖʸ
    // zₖ₊₁ = zₖ + αₖᶻpₖᶻ
    x += α * step.p_x;
    s += α * step.p_s;
    y += α_z * step.p_y;
    z += α_z * step.p_z;

    // A requirement for the convergence proof is that the primal-dual barrier
    // term Hessian Σₖ₊₁ does not deviate arbitrarily much from the primal
    // barrier term Hessian μSₖ₊₁⁻².
    //
    //   Σₖ₊₁ = μSₖ₊₁⁻²
    //   Sₖ₊₁⁻¹Zₖ₊₁ = μSₖ₊₁⁻²
    //   Zₖ₊₁ = μSₖ₊₁⁻¹
    //
    // We ensure this by resetting
    //
    //   zₖ₊₁ = clamp(zₖ₊₁, 1/κ_Σ μ/sₖ₊₁, κ_Σ μ/sₖ₊₁)
    //
    // for some fixed κ_Σ ≥ 1 after each step. See equation (16) of [2].
    for (int row = 0; row < z.rows(); ++row) {
      constexpr Scalar κ_Σ(1e10);
      z[row] =
          std::clamp(z[row], Scalar(1) / κ_Σ * μ / s[row], κ_Σ * μ / s[row]);
    }

    // Update autodiff for Jacobians and Hessian
    f = matrices.f(x);
    A_e = matrices.A_e(x);
    A_i = matrices.A_i(x);
    g = matrices.g(x);
    H = matrices.H(x, y, z);

    ScopedProfiler next_iter_prep_profiler{next_iter_prep_prof};

    c_e = matrices.c_e(x);
    c_i = matrices.c_i(x);

    // Update the error estimate
    E_0 = error_estimate<Scalar>(g, A_e, c_e, A_i, c_i, s, y, z, Scalar(0));

    // Update the barrier parameter if necessary
    if (E_0 > Scalar(options.tolerance)) {
      // Barrier parameter scale factor for tolerance checks
      constexpr Scalar κ_ε(10);

      // While the error estimate is below the desired threshold for this
      // barrier parameter value, decrease the barrier parameter further
      Scalar E_μ = error_estimate<Scalar>(g, A_e, c_e, A_i, c_i, s, y, z, μ);
      while (μ > μ_min && E_μ <= κ_ε * μ) {
        update_barrier_parameter_and_reset_filter();
        E_μ = error_estimate<Scalar>(g, A_e, c_e, A_i, c_i, s, y, z, μ);
      }
    }

    next_iter_prep_profiler.stop();
    inner_iter_profiler.stop();

    if (options.diagnostics) {
      print_iteration_diagnostics(
          iterations, IterationType::NORMAL,
          inner_iter_profiler.current_duration(), E_0, f,
          c_e.template lpNorm<1>() + (c_i - s).template lpNorm<1>(), s.dot(z),
          μ, solver.hessian_regularization(), α, α_max, α_reduction_factor,
          α_z);
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
interior_point(const InteriorPointMatrixCallbacks<double>& matrix_callbacks,
               std::span<std::function<bool(const IterationInfo<double>& info)>>
                   iteration_callbacks,
               const Options& options,
#ifdef SLEIPNIR_ENABLE_BOUND_PROJECTION
               const Eigen::ArrayX<bool>& bound_constraint_mask,
#endif
               Eigen::Vector<double, Eigen::Dynamic>& x);

}  // namespace slp
