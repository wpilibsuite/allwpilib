// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.path;

import edu.wpi.first.math.Num;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.optimization.SimulatedAnnealing;
import java.util.function.BiFunction;

/**
 * The traveling salesman problem is optimizing the cost of a cycle through a certain number of
 * poses.
 *
 * @param <T> A Num defining the length of the path and the number of poses.
 */
public class TravelingSalesman<T extends Num> {
  private final BiFunction<Pose2d, Pose2d, Double> m_costFunction;
  private final Pose2d[] m_poses;

  /**
   * Constructor for the Traveling Salesman solver with initial poses and a cost function that can
   * be either user made or a static one from this class.
   *
   * @param costFunction determines the cost between two poses and is then used for every pair to
   *     determine total cost of a path. This is what is optimized.
   * @param poses an array of Pose2d objects that define the points the path has to pass through.
   */
  public TravelingSalesman(BiFunction<Pose2d, Pose2d, Double> costFunction, Pose2d[] poses) {
    this.m_costFunction = costFunction;
    this.m_poses = poses.clone();
  }

  /**
   * Optimizes the path though every pose in the poses[] using the costFunction.
   *
   * @param iterations the number of times it's going to try to find a better random neighbor.
   * @return the optimized path as a vector that represents a list of indices.
   */
  public Vector<T> solve(int iterations) {
    SimulatedAnnealing<T> simulatedAnnealing =
        new SimulatedAnnealing<>(iterations, 1, this::getNeighbor, this::evaluate);

    Vector<T> initial = new Vector<T>(() -> m_poses.length);
    for (int i = 0; i < m_poses.length; ++i) {
      initial.set(i, 0, i);
    }

    return simulatedAnnealing.minimize(initial);
  }

  /**
   * A random neighbor is generated to try to replace the current one.
   *
   * @param state A vector that is a list of indices that defines the path through the path array.
   * @return Generates a random neighbor of the current state by flipping a random range in the path
   *     array.
   */
  private Vector<T> getNeighbor(Vector<T> state) {
    Vector<T> stateCopy = state.div(1);
    int rangeStart = (int) (Math.random() * state.getNumRows());
    int rangeEnd = (int) (Math.random() * state.getNumRows());
    if (rangeEnd < rangeStart) {
      int temp = rangeEnd;
      rangeEnd = rangeStart;
      rangeStart = temp;
    }
    for (int i = rangeStart; i <= (rangeStart + rangeEnd) / 2; ++i) {
      double temp = stateCopy.get(i, 0);
      stateCopy.set(i, 0, state.get(rangeEnd - (i - rangeStart), 0));
      stateCopy.set(rangeEnd - (i - rangeStart), 0, temp);
    }
    return stateCopy;
  }

  /**
   * Adds up all costs of the adjacent pairs in a path state for a total cost.
   *
   * @param state A vector that is a list of indices that defines the path through the path array.
   * @return double that defines the cost of this path by adding up all distances in path.
   */
  public double evaluate(Vector<T> state) {
    double sum = 0;
    for (int i = 0; i < state.getNumRows(); ++i) {
      sum +=
          m_costFunction.apply(
              m_poses[(int) state.get(i, 0)],
              m_poses[(int) (state.get((i + 1) % state.getNumRows(), 0))]);
    }
    return sum;
  }

  /** A static function that defines cost as the 2d distance between the Poses. */
  public static final BiFunction<Pose2d, Pose2d, Double> distanceCostFunction =
      (a, b) -> Math.sqrt(Math.pow(a.getX() - b.getX(), 2) + Math.pow(a.getY() - b.getY(), 2));
}
