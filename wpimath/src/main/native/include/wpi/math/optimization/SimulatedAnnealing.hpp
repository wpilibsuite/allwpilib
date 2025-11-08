// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>
#include <functional>
#include <limits>
#include <random>

namespace frc {

/**
 * An implementation of the Simulated Annealing stochastic nonlinear
 * optimization method.
 *
 * Solving optimization problems involves tweaking decision variables to try to
 * minimize some cost function. Simulated annealing is good for solving
 * optimization problems with many local minima and a very large search space
 * (it’s a heuristic solver rather than an exact solver like, say, SQP or
 * interior-point method). Simulated annealing is a popular choice for solving
 * the traveling salesman problem (see TravelingSalesman).
 *
 * @see <a
 * href="https://en.wikipedia.org/wiki/Simulated_annealing">https://en.wikipedia.org/wiki/Simulated_annealing</a>
 * @tparam State The type of the state to optimize.
 */
template <typename State>
class SimulatedAnnealing {
 public:
  /**
   * Constructor for Simulated Annealing that can be used for the same functions
   * but with different initial states.
   *
   * @param initialTemperature The initial temperature. Higher temperatures make
   *     it more likely a worse state will be accepted during iteration, helping
   *     to avoid local minima. The temperature is decreased over time.
   * @param neighbor Function that generates a random neighbor of the current
   *     state.
   * @param cost Function that returns the scalar cost of a state.
   */
  constexpr SimulatedAnnealing(double initialTemperature,
                               std::function<State(const State&)> neighbor,
                               std::function<double(const State&)> cost)
      : m_initialTemperature{initialTemperature},
        m_neighbor{neighbor},
        m_cost{cost} {}

  /**
   * Runs the Simulated Annealing algorithm.
   *
   * @param initialGuess The initial state.
   * @param iterations Number of iterations to run the solver.
   * @return The optimized state.
   */
  State Solve(const State& initialGuess, int iterations) {
    State minState = initialGuess;
    double minCost = std::numeric_limits<double>::infinity();

    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<> distr{0.0, 1.0};

    State state = initialGuess;
    double cost = m_cost(state);

    for (int i = 0; i < iterations; ++i) {
      double temperature = m_initialTemperature / i;

      State proposedState = m_neighbor(state);
      double proposedCost = m_cost(proposedState);
      double deltaCost = proposedCost - cost;

      double acceptanceProbability = std::exp(-deltaCost / temperature);

      // If cost went down or random number exceeded acceptance probability,
      // accept the proposed state
      if (deltaCost < 0 || acceptanceProbability >= distr(gen)) {
        state = proposedState;
        cost = proposedCost;
      }

      // If proposed cost is less than minimum, the proposed state becomes the
      // new minimum
      if (proposedCost < minCost) {
        minState = proposedState;
        minCost = proposedCost;
      }
    }

    return minState;
  }

 private:
  double m_initialTemperature;
  std::function<State(const State&)> m_neighbor;
  std::function<double(const State&)> m_cost;
};

}  // namespace frc
