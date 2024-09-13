// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.path;

import edu.wpi.first.math.Num;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.optimization.SimulatedAnnealing;
import java.util.Arrays;
import java.util.Collections;
import java.util.function.ToDoubleBiFunction;

/**
 * Given a list of poses, this class finds the shortest possible route that visits each pose exactly
 * once and returns to the origin pose.
 *
 * @see <a
 *     href="https://en.wikipedia.org/wiki/Travelling_salesman_problem">https://en.wikipedia.org/wiki/Travelling_salesman_problem</a>
 */
public class TravelingSalesman {
  // Default cost is 2D distance between poses
  private final ToDoubleBiFunction<Pose2d, Pose2d> m_cost;

  /**
   * Constructs a traveling salesman problem solver with a cost function defined as the 2D distance
   * between poses.
   */
  public TravelingSalesman() {
    this((Pose2d a, Pose2d b) -> Math.hypot(a.getX() - b.getX(), a.getY() - b.getY()));
  }

  /**
   * Constructs a traveling salesman problem solver with a user-provided cost function.
   *
   * @param cost Function that returns the cost between two poses. The sum of the costs for every
   *     pair of poses is minimized.
   */
  public TravelingSalesman(ToDoubleBiFunction<Pose2d, Pose2d> cost) {
    m_cost = cost;
  }

  /**
   * Finds the path through every pose that minimizes the cost. The first pose in the returned array
   * is the first pose that was passed in.
   *
   * @param <Poses> A Num defining the length of the path and the number of poses.
   * @param poses An array of Pose2ds the path must pass through.
   * @param iterations The number of times the solver attempts to find a better random neighbor.
   * @return The optimized path as an array of Pose2ds.
   */
  public <Poses extends Num> Pose2d[] solve(Pose2d[] poses, int iterations) {
    var solver =
        new SimulatedAnnealing<>(
            1.0,
            this::neighbor,
            // Total cost is sum of all costs between adjacent pose pairs in path
            (Vector<Poses> state) -> {
              double sum = 0.0;
              for (int i = 0; i < state.getNumRows(); ++i) {
                sum +=
                    m_cost.applyAsDouble(
                        poses[(int) state.get(i, 0)],
                        poses[(int) state.get((i + 1) % poses.length, 0)]);
              }
              return sum;
            });

    var initial = new Vector<Poses>(() -> poses.length);
    for (int i = 0; i < poses.length; ++i) {
      initial.set(i, 0, i);
    }

    var indices = solver.solve(initial, iterations);

    var solution = new Pose2d[poses.length];
    for (int i = 0; i < poses.length; ++i) {
      solution[i] = poses[(int) indices.get(i, 0)];
    }

    // Rotate solution list until solution[0] = poses[0]
    Collections.rotate(Arrays.asList(solution), -Arrays.asList(solution).indexOf(poses[0]));

    return solution;
  }

  /**
   * A random neighbor is generated to try to replace the current one.
   *
   * @param state A vector that is a list of indices that defines the path through the path array.
   * @return Generates a random neighbor of the current state by flipping a random range in the path
   *     array.
   */
  private <Poses extends Num> Vector<Poses> neighbor(Vector<Poses> state) {
    var proposedState = new Vector<>(state);

    int rangeStart = (int) (Math.random() * (state.getNumRows() - 1));
    int rangeEnd = (int) (Math.random() * (state.getNumRows() - 1));
    if (rangeEnd < rangeStart) {
      int temp = rangeEnd;
      rangeEnd = rangeStart;
      rangeStart = temp;
    }

    for (int i = rangeStart; i <= (rangeStart + rangeEnd) / 2; ++i) {
      double temp = proposedState.get(i, 0);
      proposedState.set(i, 0, state.get(rangeEnd - (i - rangeStart), 0));
      proposedState.set(rangeEnd - (i - rangeStart), 0, temp);
    }

    return proposedState;
  }
}
