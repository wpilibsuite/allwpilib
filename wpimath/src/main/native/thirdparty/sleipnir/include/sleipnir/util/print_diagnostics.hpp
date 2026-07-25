// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <ranges>
#include <string>
#include <utility>

#include <Eigen/Core>
#include <gch/small_vector.hpp>

#include "sleipnir/util/print.hpp"
#include "sleipnir/util/profiler.hpp"

namespace slp {

/// Iteration type.
enum class IterationType : uint8_t {
  /// Normal iteration.
  NORMAL,
  /// Second-order correction iteration.
  SECOND_ORDER_CORRECTION,
  /// Feasibility restoration iteration.
  FEASIBILITY_RESTORATION
};

/// Converts std::chrono::duration to a number of milliseconds rounded to three
/// decimals.
template <typename Rep, typename Period = std::ratio<1>>
constexpr double to_ms(const std::chrono::duration<Rep, Period>& duration) {
  using std::chrono::duration_cast;
  using std::chrono::microseconds;
  return duration_cast<microseconds>(duration).count() / 1e3;
}

/// Renders value as power of 10.
///
/// @tparam Scalar Scalar type.
/// @param value Value.
template <typename Scalar>
std::string power_of_10(Scalar value) {
  if (value == Scalar(0)) {
    return " 0";
  } else {
    using std::log10;
    int exponent = static_cast<int>(log10(value));

    if (exponent == 0) {
      return " 1";
    } else if (exponent == 1) {
      return "10";
    } else {
      // Gather exponent digits
      int n = std::abs(exponent);
      gch::small_vector<int> digits;
      do {
        digits.emplace_back(n % 10);
        n /= 10;
      } while (n > 0);

      std::string output = "10";

      // Append exponent
      if (exponent < 0) {
        output += "⁻";
      }
      constexpr std::array strs{"⁰", "¹", "²", "³", "⁴",
                                "⁵", "⁶", "⁷", "⁸", "⁹"};
      for (const auto& digit : digits | std::views::reverse) {
        output += strs[digit];
      }

      return output;
    }
  }
}

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
/// Prints error for too few degrees of freedom.
///
/// @tparam Scalar Scalar type.
/// @param c_e The problem's equality constraints cₑ(x) evaluated at the current
///     iterate.
template <typename Scalar>
void print_too_few_dofs_error(
    const Eigen::Vector<Scalar, Eigen::Dynamic>& c_e) {
  slp::println("The problem has too few degrees of freedom.");
  slp::println("Violated constraints (cₑ(x) = 0) in order of declaration:");
  for (int row = 0; row < c_e.rows(); ++row) {
    if (c_e[row] < Scalar(0)) {
      slp::println("  {}/{}: {} = 0", row + 1, c_e.rows(), c_e[row]);
    }
  }
}
#else
#define print_too_few_dofs_error(...)
#endif

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
/// Prints equality constraint local infeasibility error.
///
/// @tparam Scalar Scalar type.
/// @param c_e The problem's equality constraints cₑ(x) evaluated at the current
///     iterate.
template <typename Scalar>
void print_c_e_local_infeasibility_error(
    const Eigen::Vector<Scalar, Eigen::Dynamic>& c_e) {
  slp::println(
      "The problem is locally infeasible due to violated equality "
      "constraints.");
  slp::println("Violated constraints (cₑ(x) = 0) in order of declaration:");
  for (int row = 0; row < c_e.rows(); ++row) {
    if (c_e[row] < Scalar(0)) {
      slp::println("  {}/{}: {} = 0", row + 1, c_e.rows(), c_e[row]);
    }
  }
}
#else
#define print_c_e_local_infeasibility_error(...)
#endif

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
/// Prints inequality constraint local infeasibility error.
///
/// @tparam Scalar Scalar type.
/// @param c_i The problem's inequality constraints cᵢ(x) evaluated at the
///     current iterate.
template <typename Scalar>
void print_c_i_local_infeasibility_error(
    const Eigen::Vector<Scalar, Eigen::Dynamic>& c_i) {
  slp::println(
      "The problem is locally infeasible due to violated inequality "
      "constraints.");
  slp::println("Violated constraints (cᵢ(x) ≥ 0) in order of declaration:");
  for (int row = 0; row < c_i.rows(); ++row) {
    if (c_i[row] < Scalar(0)) {
      slp::println("  {}/{}: {} ≥ 0", row + 1, c_i.rows(), c_i[row]);
    }
  }
}
#else
#define print_c_i_local_infeasibility_error(...)
#endif

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
inline void print_bound_constraint_global_infeasibility_error(
    const std::span<const std::pair<Eigen::Index, Eigen::Index>>
        conflicting_lower_upper_bound_indices) {
  slp::println(
      "The problem is globally infeasible due to conflicting bound "
      "constraints:");
  for (const auto& [lower_bound_idx, upper_bound_idx] :
       conflicting_lower_upper_bound_indices) {
    slp::println(
        "  Inequality constraint {} gives a lower bound that is greater than "
        "the upper bound given by inequality constraint {}",
        lower_bound_idx, upper_bound_idx);
  }
}
#else
#define print_bound_constraint_global_infeasibility_error(...)
#endif

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
/// Prints diagnostics for the current iteration.
///
/// @tparam Scalar Scalar type.
/// @param iterations Number of iterations.
/// @param type The iteration's type.
/// @param time The iteration duration.
/// @param error The error.
/// @param cost The cost.
/// @param infeasibility The infeasibility.
/// @param complementarity The complementarity.
/// @param μ The barrier parameter.
/// @param δ The Hessian regularization factor.
/// @param γ The constraint Jacobian regularization factor.
/// @param full_primal_step_inf_norm The infinity norm of the full primal step.
/// @param full_dual_step_inf_norm The infinity norm of the full dual step.
/// @param primal_α The primal step size.
/// @param primal_α_max The max primal step size.
/// @param α_reduction_factor Factor by which primal_α is reduced during
///     backtracking.
/// @param dual_α The dual step size.
template <typename Scalar, typename Rep, typename Period = std::ratio<1>>
void print_iteration_diagnostics(int iterations, IterationType type,
                                 const std::chrono::duration<Rep, Period>& time,
                                 Scalar error, Scalar cost,
                                 Scalar infeasibility, Scalar complementarity,
                                 Scalar μ, Scalar δ, Scalar γ,
                                 Scalar full_primal_step_inf_norm,
                                 Scalar full_dual_step_inf_norm,
                                 Scalar primal_α, Scalar primal_α_max,
                                 Scalar α_reduction_factor, Scalar dual_α) {
  if (iterations % 20 == 0) {
    if (iterations == 0) {
      slp::println("┏{:━^119}┓", "");
    } else {
      slp::println("┢{:━^119}┪", "");
    }
    slp::println(
        "┃{:^4}   {:^9} {:^10} {:^11} {:^10} {:^8} {:^8} {:^5} {:^5} {:^8} "
        "{:^8} {:^8} {:^8} {:^2}┃",
        "iter", "duration", "error", "cost", "infeas.", "complem.", "μ", "δ",
        "γ", "|p_pr|", "|p_du|", "α_pr", "α_du", "↩");
    slp::println("┡{:━^119}┩", "");
  }

  // For the number of backtracks, we want x such that:
  //
  //   αᵐᵃˣrˣ = α
  //
  // where r ∈ (0, 1) is the reduction factor.
  //
  //   rˣ = α/αᵐᵃˣ
  //   ln(rˣ) = ln(α/αᵐᵃˣ)
  //   x ln(r) = ln(α/αᵐᵃˣ)
  //   x = ln(α/αᵐᵃˣ)/ln(r)
  using std::log;
  int backtracks =
      static_cast<int>(log(primal_α / primal_α_max) / log(α_reduction_factor));

  constexpr std::array ITERATION_TYPES{" ", "s", "r"};
  slp::println(
      "│{:4} {:1} {:9.3f} {:10.4e} {:11.4e} {:10.4e} {:8.2e} {:8.2e} {:<5} "
      "{:<5} {:8.2e} {:8.2e} {:8.2e} {:8.2e} {:2d}│",
      iterations, ITERATION_TYPES[std::to_underlying(type)], to_ms(time), error,
      cost, infeasibility, complementarity, μ, power_of_10(δ), power_of_10(γ),
      full_primal_step_inf_norm, full_dual_step_inf_norm, primal_α, dual_α,
      backtracks);
}
#else
#define print_iteration_diagnostics(...)
#endif

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
/// Prints bottom of iteration diagnostics table.
inline void print_bottom_iteration_diagnostics() {
  slp::println("└{:─^119}┘", "");
}
#else
#define print_bottom_iteration_diagnostics(...)
#endif

/// Renders histogram of the given normalized value.
///
/// @tparam Width Width of the histogram in characters.
/// @param value Normalized value from 0 to 1.
template <int Width>
  requires(Width > 0)
std::string histogram(double value) {
  value = std::clamp(value, 0.0, 1.0);

  double ipart;
  int fpart = static_cast<int>(std::modf(value * Width, &ipart) * 8);

  constexpr std::array strs{" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};
  std::string hist;

  int index = 0;
  while (index < ipart) {
    hist += strs[8];
    ++index;
  }
  if (fpart > 0) {
    hist += strs[fpart];
    ++index;
  }
  while (index < Width) {
    hist += strs[0];
    ++index;
  }

  return hist;
}

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
/// Prints solver diagnostics.
///
/// @param solve_profilers Solve profilers.
inline void print_solver_diagnostics(
    const gch::small_vector<SolveProfiler>& solve_profilers) {
  auto solve_duration = to_ms(solve_profilers[0].total_duration());

  slp::println("┏{:━^66}┓", "");
  slp::println("┃{:^21} {:^18} {:^10} {:^9} {:^4}┃", "time trace", "percentage",
               "total", "each", "runs");
  slp::println("┡{:━^66}┩", "");

  for (auto& profiler : solve_profilers) {
    double norm = solve_duration == 0.0
                      ? (&profiler == &solve_profilers[0] ? 1.0 : 0.0)
                      : to_ms(profiler.total_duration()) / solve_duration;
    slp::println("│{:<21} {:>6.2f}%▕{}▏ {:>10.3f} {:>9.3f} {:>4}│",
                 profiler.name(), norm * 100.0, histogram<9>(norm),
                 to_ms(profiler.total_duration()),
                 to_ms(profiler.average_duration()), profiler.num_solves());
  }

  slp::println("└{:─^66}┘", "");
}
#else
#define print_solver_diagnostics(...)
#endif

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
/// Prints setup diagnostics.
///
/// @param setup_profilers Setup profilers.
inline void print_setup_diagnostics(
    const gch::small_vector<SetupProfiler>& setup_profilers) {
  auto setup_duration = to_ms(setup_profilers[0].duration());

  // Print link to diagnostic output description
  slp::println(
      "See https://sleipnirgroup.github.io/Sleipnir/md_usage.html#output for "
      "diagnostic output description.\n");

  // Print heading
  slp::println("┏{:━^50}┓", "");
  slp::println("┃{:^21} {:^18} {:^9}┃", "time trace", "percentage", "duration");
  slp::println("┡{:━^50}┩", "");

  // Print setup profilers
  for (auto& profiler : setup_profilers) {
    double norm = setup_duration == 0.0
                      ? (&profiler == &setup_profilers[0] ? 1.0 : 0.0)
                      : to_ms(profiler.duration()) / setup_duration;
    slp::println("│{:<21} {:>6.2f}%▕{}▏ {:>9.3f}│", profiler.name(),
                 norm * 100.0, histogram<9>(norm), to_ms(profiler.duration()));
  }

  slp::println("└{:─^50}┘", "");
}
#else
#define print_setup_diagnostics(...)
#endif

}  // namespace slp
