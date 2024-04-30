// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <future>
#include <span>

#include "sleipnir/optimization/SolverStatus.hpp"
#include "sleipnir/util/FunctionRef.hpp"
#include "sleipnir/util/SmallVector.hpp"

namespace sleipnir {

/**
 * The result of a multistart solve.
 *
 * @tparam DecisionVariables The type containing the decision variable initial
 *   guess.
 */
template <typename DecisionVariables>
struct MultistartResult {
  SolverStatus status;
  DecisionVariables variables;
};

/**
 * Solves an optimization problem from different starting points in parallel,
 * then returns the solution with the lowest cost.
 *
 * Each solve is performed on a separate thread. Solutions from successful
 * solves are always preferred over solutions from unsuccessful solves, and cost
 * (lower is better) is the tiebreaker between successful solves.
 *
 * @tparam DecisionVariables The type containing the decision variable initial
 *   guess.
 * @param solve A user-provided function that takes a decision variable initial
 *   guess and returns a MultistartResult.
 * @param initialGuesses A list of decision variable initial guesses to try.
 */
template <typename DecisionVariables>
MultistartResult<DecisionVariables> Multistart(
    function_ref<MultistartResult<DecisionVariables>(const DecisionVariables&)>
        solve,
    std::span<const DecisionVariables> initialGuesses) {
  small_vector<std::future<MultistartResult<DecisionVariables>>> futures;
  futures.reserve(initialGuesses.size());

  for (const auto& initialGuess : initialGuesses) {
    futures.emplace_back(std::async(std::launch::async, solve, initialGuess));
  }

  small_vector<MultistartResult<DecisionVariables>> results;
  results.reserve(futures.size());

  for (auto& future : futures) {
    results.emplace_back(future.get());
  }

  return *std::min_element(
      results.cbegin(), results.cend(), [](const auto& a, const auto& b) {
        // Prioritize successful solve
        if (a.status.exitCondition == SolverExitCondition::kSuccess &&
            b.status.exitCondition != SolverExitCondition::kSuccess) {
          return true;
        }

        // Otherwise prioritize solution with lower cost
        return a.status.cost < b.status.cost;
      });
}

}  // namespace sleipnir
