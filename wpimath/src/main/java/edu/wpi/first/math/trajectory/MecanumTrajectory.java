package edu.wpi.first.math.trajectory;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import java.util.Arrays;
import java.util.stream.Stream;

/** A trajectory for mecanum drive robots with drivetrain-specific interpolation. */
public class MecanumTrajectory extends Trajectory<MecanumSample> {
  private final MecanumDriveKinematics kinematics;

  /**
   * Constructs a MecanumTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public MecanumTrajectory(MecanumDriveKinematics kinematics, MecanumSample[] samples) {
    super(samples);
    this.kinematics = kinematics;
  }

  /**
   * Constructs a MecanumTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public MecanumTrajectory(MecanumDriveKinematics kinematics, TrajectorySample<?>[] samples) {
    this(
        kinematics,
        Arrays.stream(samples)
            .map(s -> new MecanumSample(s, kinematics))
            .toArray(MecanumSample[]::new));
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
    TrajectorySample.Base interpolated = TrajectorySample.kinematicInterpolate(start, end, t);

    return new MecanumSample(interpolated, kinematics);
  }

  @Override
  public MecanumTrajectory transformBy(Transform2d transform) {
    return new MecanumTrajectory(
        kinematics,
        Arrays.stream(samples).map(s -> s.transform(transform)).toArray(MecanumSample[]::new));
  }

  @Override
  public MecanumTrajectory concatenate(Trajectory<MecanumSample> other) {
    if (other.samples.length < 1) {
      return this;
    }

    var withNewTimestamp =
        Arrays.stream(other.samples)
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
            .toArray(MecanumSample[]::new);

    var combinedSamples =
        Stream.concat(Arrays.stream(samples), Arrays.stream(withNewTimestamp))
            .toArray(MecanumSample[]::new);

    return new MecanumTrajectory(kinematics, combinedSamples);
  }

  @Override
  public MecanumTrajectory relativeTo(Pose2d other) {
    return new MecanumTrajectory(
        kinematics,
        Arrays.stream(samples).map(s -> s.relativeTo(other)).toArray(MecanumSample[]::new));
  }
}
