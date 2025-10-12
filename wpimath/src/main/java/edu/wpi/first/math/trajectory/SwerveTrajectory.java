package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.SwerveModuleState;
import java.util.ArrayList;
import java.util.List;

/** A trajectory for swerve drive robots with drivetrain-specific interpolation. */
public class SwerveTrajectory extends Trajectory<SwerveSample> {

  /**
   * Constructs a SwerveTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public SwerveTrajectory(List<SwerveSample> samples) {
    super(samples);
  }

  /**
   * Interpolates between two samples using Euler integration.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  @Override
  public SwerveSample interpolate(SwerveSample start, SwerveSample end, double t) {
    if (start.states.length != end.states.length) {
      throw new IllegalArgumentException(
          "Cannot interpolate SwerveSample with different number of states: "
              + start.states.length
              + " vs "
              + end.states.length);
    }
    SwerveModuleState[] newStates = new SwerveModuleState[start.states.length];
    for (int i = 0; i < start.states.length; i++) {
      newStates[i] = start.states[i].interpolate(end.states[i], t);
    }
    return new SwerveSample(
        Seconds.of(MathUtil.lerp(start.timestamp.in(Seconds), end.timestamp.in(Seconds), t)),
        start.pose.interpolate(end.pose, t),
        start.velocity.interpolate(end.velocity, t),
        start.acceleration.interpolate(end.acceleration, t),
        newStates);
  }

  @Override
  public SwerveTrajectory transformBy(Transform2d transform) {
    return new SwerveTrajectory(samples.stream().map(s -> s.transform(transform)).toList());
  }

  @Override
  public SwerveTrajectory concatenate(Trajectory<SwerveSample> other) {
    if (other.samples.isEmpty()) {
      return this;
    }

    var combinedSamples = new ArrayList<>(this.samples);
    combinedSamples.addAll(
        other.samples.stream()
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
            .toList());

    return new SwerveTrajectory(combinedSamples);
  }

  @Override
  public SwerveTrajectory relativeTo(Pose2d other) {
    return new SwerveTrajectory(samples.stream().map(s -> s.relativeTo(other)).toList());
  }

  @Override
  public SwerveTrajectory reversed() {
    var reversedSamples = new ArrayList<SwerveSample>();
    var lastTimestamp = this.duration;

    for (int i = samples.size() - 1; i >= 0; i--) {
      var sample = samples.get(i);
      var newTimestamp = lastTimestamp.minus(sample.timestamp);

      // Create a transform that rotates 180 degrees to reverse direction
      var reverseTransform = new Transform2d(0, 0, new Rotation2d(Math.PI));

      // Transform the sample and update timestamp
      var reversedSample = sample.transform(reverseTransform).withNewTimestamp(newTimestamp);
      reversedSamples.add(reversedSample);
    }

    return new SwerveTrajectory(reversedSamples);
  }
}
