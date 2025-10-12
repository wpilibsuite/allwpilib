package edu.wpi.first.math.trajectory;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import java.util.ArrayList;
import java.util.List;

/** A base trajectory class for general-purpose trajectory following. */
public class BaseTrajectory extends Trajectory<TrajectorySample.Base> {

  /**
   * Constructs a BaseTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public BaseTrajectory(List<TrajectorySample.Base> samples) {
    super(samples);
  }

  /**
   * Interpolates between two samples using constant-acceleration kinematic equations.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  @Override
  public TrajectorySample.Base interpolate(
      TrajectorySample.Base start, TrajectorySample.Base end, double t) {
    return TrajectorySample.kinematicInterpolate(start, end, t);
  }

  @Override
  public BaseTrajectory transformBy(Transform2d transform) {
    return new BaseTrajectory(samples.stream().map(s -> s.transform(transform)).toList());
  }

  @Override
  public BaseTrajectory concatenate(Trajectory<TrajectorySample.Base> other) {
    if (other.samples.isEmpty()) {
      return this;
    }

    var combinedSamples = new ArrayList<>(this.samples);
    combinedSamples.addAll(
        other.samples.stream()
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
            .toList());

    return new BaseTrajectory(combinedSamples);
  }

  @Override
  public BaseTrajectory relativeTo(Pose2d other) {
    return new BaseTrajectory(samples.stream().map(s -> s.relativeTo(other)).toList());
  }

  @Override
  public BaseTrajectory reversed() {
    var reversedSamples = new ArrayList<TrajectorySample.Base>();

    for (int i = samples.size() - 1; i >= 0; i--) {
      var sample = samples.get(i);
      var newTimestamp = this.duration.minus(sample.timestamp);

      // Create a transform that rotates 180 degrees to reverse direction
      var reverseTransform = new Transform2d(0, 0, new Rotation2d(Math.PI));

      // Transform the sample and update timestamp
      var reversedSample = sample.transform(reverseTransform).withNewTimestamp(newTimestamp);
      reversedSamples.add(reversedSample);
    }

    return new BaseTrajectory(reversedSamples);
  }
}

