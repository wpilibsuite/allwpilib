package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.MecanumDriveWheelSpeeds;
import java.util.ArrayList;
import java.util.List;

/** A trajectory for mecanum drive robots with drivetrain-specific interpolation. */
public class MecanumTrajectory extends Trajectory<MecanumSample> {

  /**
   * Constructs a MecanumTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public MecanumTrajectory(List<MecanumSample> samples) {
    super(samples);
  }

  /**
   * Linearly interpolates between two samples.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  @Override
  public MecanumSample interpolate(MecanumSample start, MecanumSample end, double t) {
    return new MecanumSample(
        Seconds.of(MathUtil.lerp(start.timestamp.in(Seconds), end.timestamp.in(Seconds), t)),
        start.pose.interpolate(end.pose, t),
        start.velocity.interpolate(end.velocity, t),
        start.acceleration.interpolate(end.acceleration, t),
        new MecanumDriveWheelSpeeds(
            MathUtil.lerp(start.speeds.frontLeft, end.speeds.frontLeft, t),
            MathUtil.lerp(start.speeds.frontRight, end.speeds.frontRight, t),
            MathUtil.lerp(start.speeds.rearLeft, end.speeds.rearLeft, t),
            MathUtil.lerp(start.speeds.rearRight, end.speeds.rearRight, t)));
  }

  @Override
  public MecanumTrajectory transformBy(Transform2d transform) {
    return new MecanumTrajectory(samples.stream().map(s -> s.transform(transform)).toList());
  }

  @Override
  public MecanumTrajectory concatenate(Trajectory<MecanumSample> other) {
    if (other.samples.isEmpty()) {
      return this;
    }

    var combinedSamples = new ArrayList<>(this.samples);
    combinedSamples.addAll(
        other.samples.stream()
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
            .toList());

    return new MecanumTrajectory(combinedSamples);
  }

  @Override
  public MecanumTrajectory relativeTo(Pose2d other) {
    return new MecanumTrajectory(samples.stream().map(s -> s.relativeTo(other)).toList());
  }

  @Override
  public MecanumTrajectory reversed() {
    var reversedSamples = new ArrayList<MecanumSample>();
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

    return new MecanumTrajectory(reversedSamples);
  }
}

