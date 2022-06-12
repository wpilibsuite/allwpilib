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
public class SimulatedAnnealing {
  /**
   * Runs the Simulated Annealing algorithm on a Vector that defines the state.
   *
   * @param initialGuess The initial state
   * @param epochs Number of iterations to run.
   * @param initialTemperature The initial temperature
   * @param getNeighbor Function to generate a random neighbor of current state vector.
   * @param getCost Function to get the double value of a state vector.
   * @param <T> Natural number defining the number of rows in the state vector.
   * @return The optimized state vector
   */
  public static <T extends Num> Vector<T> minimize(
      Vector<T> initialGuess,
      int epochs,
      double initialTemperature,
      Function<Vector<T>, Vector<T>> getNeighbor,
      Function<Vector<T>, Double> getCost) {
    double min = Double.MAX_VALUE;
    Vector<T> minState = initialGuess;

    int epoch = 0;
    double temperature;

    Vector<T> state = initialGuess;

    while (epoch++ < epochs) {
      temperature = initialTemperature / epoch;

      Vector<T> proposedState = getNeighbor.apply(state);
      double proposedStateCost = getCost.apply(proposedState);

      double deltaCost = proposedStateCost - getCost.apply(state);

      double acceptanceProbability = Math.exp(-deltaCost / temperature);

      if (deltaCost < 0 || Math.random() < acceptanceProbability) {
        state = proposedState;
      }

      if (proposedStateCost < min) {
        min = proposedStateCost;
        minState = proposedState;
      }
    }
    return minState;
  }
}
