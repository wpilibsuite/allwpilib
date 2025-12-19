// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

#include <fmt/base.h>

#include "sleipnir/util/unreachable.hpp"

namespace slp {

/// Solver exit status. Negative values indicate failure.
enum class ExitStatus : int8_t {
  /// Solved the problem to the desired tolerance.
  SUCCESS = 0,
  /// The solver returned its solution so far after the user requested a stop.
  CALLBACK_REQUESTED_STOP = 1,
  /// The solver determined the problem to be overconstrained and gave up.
  TOO_FEW_DOFS = -1,
  /// The solver determined the problem to be locally infeasible and gave up.
  LOCALLY_INFEASIBLE = -2,
  /// The problem setup frontend determined the problem to have an empty
  /// feasible region.
  GLOBALLY_INFEASIBLE = -3,
  /// The linear system factorization failed.
  FACTORIZATION_FAILED = -4,
  /// The backtracking line search failed, and the problem isn't locally
  /// infeasible.
  LINE_SEARCH_FAILED = -5,
  /// The solver encountered nonfinite initial cost or constraints and gave up.
  NONFINITE_INITIAL_COST_OR_CONSTRAINTS = -6,
  /// The solver encountered diverging primal iterates xₖ and/or sₖ and gave up.
  DIVERGING_ITERATES = -7,
  /// The solver returned its solution so far after exceeding the maximum number
  /// of iterations.
  MAX_ITERATIONS_EXCEEDED = -8,
  /// The solver returned its solution so far after exceeding the maximum
  /// elapsed wall clock time.
  TIMEOUT = -9,
};

}  // namespace slp

/// Formatter for ExitStatus.
template <>
struct fmt::formatter<slp::ExitStatus> {
  /// Parse format string.
  ///
  /// @param ctx Format parse context.
  /// @return Format parse context iterator.
  constexpr auto parse(fmt::format_parse_context& ctx) {
    return m_underlying.parse(ctx);
  }

  /// Format ExitStatus.
  ///
  /// @tparam FmtContext Format context type.
  /// @param exit_status Exit status.
  /// @param ctx Format context.
  /// @return Format context iterator.
  template <typename FmtContext>
  auto format(const slp::ExitStatus& exit_status, FmtContext& ctx) const {
    using enum slp::ExitStatus;

    switch (exit_status) {
      case SUCCESS:
        return m_underlying.format("success", ctx);
      case CALLBACK_REQUESTED_STOP:
        return m_underlying.format("callback requested stop", ctx);
      case TOO_FEW_DOFS:
        return m_underlying.format("too few degrees of freedom", ctx);
      case LOCALLY_INFEASIBLE:
        return m_underlying.format("locally infeasible", ctx);
      case GLOBALLY_INFEASIBLE:
        return m_underlying.format("globally infeasible", ctx);
      case FACTORIZATION_FAILED:
        return m_underlying.format("factorization failed", ctx);
      case LINE_SEARCH_FAILED:
        return m_underlying.format("line search failed", ctx);
      case NONFINITE_INITIAL_COST_OR_CONSTRAINTS:
        return m_underlying.format("nonfinite initial cost or constraints",
                                   ctx);
      case DIVERGING_ITERATES:
        return m_underlying.format("diverging iterates", ctx);
      case MAX_ITERATIONS_EXCEEDED:
        return m_underlying.format("max iterations exceeded", ctx);
      case TIMEOUT:
        return m_underlying.format("timeout", ctx);
      default:
        slp::unreachable();
    }
  }

 private:
  fmt::formatter<const char*> m_underlying;
};
