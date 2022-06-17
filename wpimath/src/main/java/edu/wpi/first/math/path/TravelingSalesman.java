package edu.wpi.first.math.path;

import edu.wpi.first.math.Num;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.optimization.SimulatedAnnealing;
import java.util.function.BiFunction;

public class TravelingSalesman<T extends Num> {
  private final BiFunction<Pose2d, Pose2d, Double> costFunction;
  private final Pose2d[] poses;

  public TravelingSalesman(BiFunction<Pose2d, Pose2d, Double> costFunction, Pose2d[] poses) {
    this.costFunction = costFunction;
    this.poses = poses;
  }

  public Vector<T> solve(int iterations) {
    SimulatedAnnealing<T> simulatedAnnealing =
        new SimulatedAnnealing<>(iterations, 1, this::getNeighbor, this::evaluate);

    Vector<T> initial = new Vector<T>(() -> poses.length);
    for (int i = 0; i < poses.length; ++i) {
      initial.set(i, 0, i);
    }

    return simulatedAnnealing.minimize(initial);
  }

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

  public double evaluate(Vector<T> state) {
    double sum = 0;
    for (int i = 0; i < state.getNumRows(); ++i) {
      sum +=
          costFunction.apply(
              poses[(int) state.get(i, 0)],
              poses[(int) (state.get((i + 1) % state.getNumRows(), 0))]);
    }
    return sum;
  }
}
