// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.optimization;

import edu.wpi.first.math.Num;
import edu.wpi.first.math.Vector;
import java.util.function.Function;

/**
 * An implementation of the Simulated Annealing stochastic nonlinear optimization method for finding
 * the global minimum cost.
 */
public final class SimulatedAnnealing<T extends Num> {

  private final int m_epochs;
  private final double m_initialTemperature;
  private final Function<Vector<T>, Vector<T>> m_getNeighbor;
  private final Function<Vector<T>, Double> m_getCost;

  /**
   * Constructor for Simulated Annealing that can be used for the same functions but with different
   * initial states.
   *
   * @param epochs Number of iterations to run where a random neighbor state will be generated
   *     everytime and will become the new state with some probability defined by temperature and
   *     the cost delta.
   * @param initialTemperature The initial temperature that will inversely decrease and is used to
   *     generate the probability that a worse state is accepted during iteration.
   * @param getNeighbor Function to generate a random neighbor of current state vector which is
   *     defined as another state that is derived from the current one with some randomness.
   * @param getCost Function to get the double cost of a state vector.
   */
  public SimulatedAnnealing(
      int epochs,
      double initialTemperature,
      Function<Vector<T>, Vector<T>> getNeighbor,
      Function<Vector<T>, Double> getCost) {
    this.m_epochs = epochs;
    this.m_initialTemperature = initialTemperature;
    this.m_getNeighbor = getNeighbor;
    this.m_getCost = getCost;
  }

  /**
   * Runs the Simulated Annealing algorithm on a Vector that defines the state.
   *
   * @param initialGuess The initial state.
   * @return The optimized state vector.
   */
  public Vector<T> minimize(Vector<T> initialGuess) {
    double min = Double.MAX_VALUE;
    Vector<T> minState = initialGuess;

    int epoch = 0;
    double temperature;

    Vector<T> state = initialGuess;

    while (epoch < m_epochs) {
      temperature = m_initialTemperature / epoch;

      Vector<T> proposedState = m_getNeighbor.apply(state);
      double proposedStateCost = m_getCost.apply(proposedState);

      double deltaCost = proposedStateCost - m_getCost.apply(state);

      double acceptanceProbability = Math.exp(-deltaCost / temperature);

      if (deltaCost < 0 || Math.random() < acceptanceProbability) {
        state = proposedState;
      }

      if (proposedStateCost < min) {
        min = proposedStateCost;
        minState = proposedState;
      }

      epoch++;
    }
    return minState;
  }
}
