package edu.wpi.first.math.trajectory;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Stream;

/** A base trajectory class for general-purpose trajectory following. */
public class TrajectoryBase extends Trajectory<TrajectorySample.Base> {

  /**
   * Constructs a BaseTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  @JsonCreator
  public TrajectoryBase(@JsonProperty("samples") TrajectorySample.Base[] samples) {
    super(samples);
  }

  public TrajectoryBase(List<TrajectorySample.Base> samples) {
    this(samples.toArray(TrajectorySample.Base[]::new));
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
  public TrajectoryBase transformBy(Transform2d transform) {
    Pose2d firstPose = start().pose;
    Pose2d transformedFirstPose = firstPose.transformBy(transform);

    TrajectorySample.Base transformedFirstSample =
        new TrajectorySample.Base(
            start().timestamp, transformedFirstPose, start().velocity, start().acceleration);

    Stream<TrajectorySample.Base> transformedSamples =
        Arrays.stream(samples)
            .skip(1)
            .map(
                sample ->
                    new TrajectorySample.Base(
                        sample.timestamp,
                        transformedFirstPose.plus(sample.pose.minus(firstPose)),
                        sample.velocity,
                        sample.acceleration));

    return new TrajectoryBase(
        Stream.concat(Stream.of(transformedFirstSample), transformedSamples)
            .toArray(TrajectorySample.Base[]::new));
  }

  @Override
  public TrajectoryBase concatenate(Trajectory<TrajectorySample.Base> other) {
    if (other.samples.length < 1) {
      return this;
    }

    var withNewTimestamp =
        Arrays.stream(other.samples)
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
            .toArray(TrajectorySample.Base[]::new);

    var combinedSamples =
        Stream.concat(Arrays.stream(samples), Arrays.stream(withNewTimestamp))
            .toArray(TrajectorySample.Base[]::new);

    return new TrajectoryBase(combinedSamples);
  }

  @Override
  public TrajectoryBase relativeTo(Pose2d other) {
    return new TrajectoryBase(
        Arrays.stream(samples).map(s -> s.relativeTo(other)).toArray(TrajectorySample.Base[]::new));
  }
}
