// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

#include <string_view>

#include "sleipnir/util/symbol_exports.hpp"

namespace slp {

/**
 * Solver exit status. Negative values indicate failure.
 */
enum class ExitStatus : int8_t {
  /// Solved the problem to the desired tolerance.
  SUCCESS = 0,
  /// The solver returned its solution so far after the user requested a stop.
  CALLBACK_REQUESTED_STOP = 1,
  /// The solver determined the problem to be overconstrained and gave up.
  TOO_FEW_DOFS = -1,
  /// The solver determined the problem to be locally infeasible and gave up.
  LOCALLY_INFEASIBLE = -2,
  /// The linear system factorization failed.
  FACTORIZATION_FAILED = -3,
  /// The backtracking line search failed, and the problem isn't locally
  /// infeasible.
  LINE_SEARCH_FAILED = -4,
  /// The solver encountered nonfinite initial cost or constraints and gave up.
  NONFINITE_INITIAL_COST_OR_CONSTRAINTS = -5,
  /// The solver encountered diverging primal iterates xₖ and/or sₖ and gave up.
  DIVERGING_ITERATES = -6,
  /// The solver returned its solution so far after exceeding the maximum number
  /// of iterations.
  MAX_ITERATIONS_EXCEEDED = -7,
  /// The solver returned its solution so far after exceeding the maximum
  /// elapsed wall clock time.
  TIMEOUT = -8
};

/**
 * Returns user-readable message corresponding to the solver exit status.
 *
 * @param exit_status Solver exit status.
 */
SLEIPNIR_DLLEXPORT constexpr std::string_view to_message(
    const ExitStatus& exit_status) {
  using enum ExitStatus;

  switch (exit_status) {
    case SUCCESS:
      return "success";
    case CALLBACK_REQUESTED_STOP:
      return "callback requested stop";
    case TOO_FEW_DOFS:
      return "too few degrees of freedom";
    case LOCALLY_INFEASIBLE:
      return "locally infeasible";
    case FACTORIZATION_FAILED:
      return "factorization failed";
    case LINE_SEARCH_FAILED:
      return "line search failed";
    case NONFINITE_INITIAL_COST_OR_CONSTRAINTS:
      return "nonfinite initial cost or constraints";
    case DIVERGING_ITERATES:
      return "diverging iterates";
    case MAX_ITERATIONS_EXCEEDED:
      return "max iterations exceeded";
    case TIMEOUT:
      return "timeout";
    default:
      return "unknown";
  }
}

}  // namespace slp
