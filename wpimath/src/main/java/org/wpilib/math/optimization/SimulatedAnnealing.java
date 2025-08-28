// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import org.wpilib.math.path.TravelingSalesman;

import java.util.function.Function;
import java.util.function.ToDoubleFunction;

/**
 * An implementation of the Simulated Annealing stochastic nonlinear optimization method.
 *
 * <p>Solving optimization problems involves tweaking decision variables to try to minimize some
 * cost function. Simulated annealing is good for solving optimization problems with many local
 * minima and a very large search space (it’s a heuristic solver rather than an exact solver like,
 * say, SQP or interior-point method). Simulated annealing is a popular choice for solving the
 * traveling salesman problem (see {@link TravelingSalesman}).
 *
 * @see <a
 *     href="https://en.wikipedia.org/wiki/Simulated_annealing">https://en.wikipedia.org/wiki/Simulated_annealing</a>
 * @param <State> The type of the state to optimize.
 */
public final class SimulatedAnnealing<State> {
  private final double m_initialTemperature;
  private final Function<State, State> m_neighbor;
  private final ToDoubleFunction<State> m_cost;

  /**
   * Constructor for Simulated Annealing that can be used for the same functions but with different
   * initial states.
   *
   * @param initialTemperature The initial temperature. Higher temperatures make it more likely a
   *     worse state will be accepted during iteration, helping to avoid local minima. The
   *     temperature is decreased over time.
   * @param neighbor Function that generates a random neighbor of the current state.
   * @param cost Function that returns the scalar cost of a state.
   */
  public SimulatedAnnealing(
      double initialTemperature, Function<State, State> neighbor, ToDoubleFunction<State> cost) {
    m_initialTemperature = initialTemperature;
    m_neighbor = neighbor;
    m_cost = cost;
  }

  /**
   * Runs the Simulated Annealing algorithm.
   *
   * @param initialGuess The initial state.
   * @param iterations Number of iterations to run the solver.
   * @return The optimized stater.
   */
  public State solve(State initialGuess, int iterations) {
    State minState = initialGuess;
    double minCost = Double.MAX_VALUE;

    State state = initialGuess;
    double cost = m_cost.applyAsDouble(state);

    for (int i = 0; i < iterations; ++i) {
      double temperature = m_initialTemperature / i;

      State proposedState = m_neighbor.apply(state);
      double proposedCost = m_cost.applyAsDouble(proposedState);
      double deltaCost = proposedCost - cost;

      double acceptanceProbability = Math.exp(-deltaCost / temperature);

      // If cost went down or random number exceeded acceptance probability,
      // accept the proposed state
      if (deltaCost < 0 || acceptanceProbability >= Math.random()) {
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
}
