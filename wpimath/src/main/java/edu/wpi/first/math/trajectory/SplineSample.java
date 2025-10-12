package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.units.measure.Time;

public class SplineSample extends TrajectorySample<SplineSample> {
  public final double curvature;

  /** Constructs a SplineSample. */
  public SplineSample(double timestamp, Pose2d pose, double vel, double accel, double curvature) {
    super(
        Seconds.of(timestamp),
        pose,
        new ChassisSpeeds(vel, 0.0, vel * curvature),
        new ChassisAccelerations(accel, 0.0, accel * curvature));
    this.curvature = curvature;
  }

  /** Constructs a SplineSample. */
  public SplineSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds vel,
      ChassisAccelerations accel,
      double curvature) {
    super(timestamp, pose, vel, accel);
    this.curvature = curvature;
  }

  /**
   * Constructs a SplineSample from another TrajectorySample, assuming the other sample's linear
   * velocity is not zero.
   */
  public SplineSample(TrajectorySample<?> sample) {
    super(sample.timestamp, sample.pose, sample.velocity, sample.acceleration);
    this.curvature =
        sample.velocity.omega / (sample.velocity.vx == 0.0 ? 1E-9 : sample.velocity.vx);
  }

  public SplineSample() {
    this(0.0, Pose2d.kZero, 0.0, 0.0, 0.0);
  }

  @Override
  public SplineSample transform(Transform2d transform) {
    return new SplineSample(
        timestamp, pose.transformBy(transform), velocity, acceleration, curvature);
  }

  @Override
  public SplineSample relativeTo(Pose2d other) {
    return new SplineSample(timestamp, pose.relativeTo(other), velocity, acceleration, curvature);
  }

  @Override
  public SplineSample withNewTimestamp(Time timestamp) {
    return new SplineSample(timestamp, pose, velocity, acceleration, curvature);
  }
}
