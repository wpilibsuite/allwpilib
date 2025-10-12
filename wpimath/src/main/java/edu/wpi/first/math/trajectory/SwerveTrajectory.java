package edu.wpi.first.math.trajectory;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import java.util.Arrays;
import java.util.stream.Stream;

/** A trajectory for swerve drive robots with drivetrain-specific interpolation. */
public class SwerveTrajectory extends Trajectory<SwerveSample> {
  private final SwerveDriveKinematics kinematics;

  /**
   * Constructs a SwerveTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public SwerveTrajectory(SwerveDriveKinematics kinematics, SwerveSample[] samples) {
    super(samples);
    this.kinematics = kinematics;
  }

  public SwerveTrajectory(SwerveDriveKinematics kinematics, TrajectorySample<?>[] samples) {
    this(
        kinematics,
        Arrays.stream(samples)
            .map(s -> new SwerveSample(s, kinematics))
            .toArray(SwerveSample[]::new));
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

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, t);

    return new SwerveSample(interpolated, kinematics);
  }

  @Override
  public SwerveTrajectory transformBy(Transform2d transform) {
    return new SwerveTrajectory(
        kinematics,
        Arrays.stream(samples).map(s -> s.transform(transform)).toArray(SwerveSample[]::new));
  }

  @Override
  public SwerveTrajectory concatenate(Trajectory<SwerveSample> other) {
    if (other.samples.length < 1) {
      return this;
    }

    var withNewTimestamp =
        Arrays.stream(other.samples)
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
            .toArray(SwerveSample[]::new);

    var combinedSamples =
        Stream.concat(Arrays.stream(samples), Arrays.stream(withNewTimestamp))
            .toArray(SwerveSample[]::new);

    return new SwerveTrajectory(kinematics, combinedSamples);
  }

  @Override
  public SwerveTrajectory relativeTo(Pose2d other) {
    return new SwerveTrajectory(
        kinematics,
        Arrays.stream(samples).map(s -> s.relativeTo(other)).toArray(SwerveSample[]::new));
  }
}
