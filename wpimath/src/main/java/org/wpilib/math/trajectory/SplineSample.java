// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.wpilib.units.Units.Seconds;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.units.measure.Time;

public class SplineSample extends TrajectorySample {
  public final double curvature;

  /** Constructs a SplineSample. */
  public SplineSample(
      double timestamp, Pose2d pose, double velocity, double acceleration, double curvature) {
    super(
        timestamp,
        pose,
        new ChassisSpeeds(velocity, 0.0, velocity * curvature),
        new ChassisAccelerations(acceleration, 0.0, acceleration * curvature));
    this.curvature = curvature;
  }

  /** Constructs a SplineSample. */
  public SplineSample(
      double timestamp,
      Pose2d pose,
      ChassisSpeeds velocity,
      ChassisAccelerations acceleration,
      double curvature) {
    super(timestamp, pose, velocity, acceleration);
    this.curvature = curvature;
  }

  /**
   * Constructs a SplineSample from another TrajectorySample, assuming the other sample's linear
   * velocity is not zero.
   */
  public SplineSample(TrajectorySample sample) {
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
  public SplineSample withNewTimestamp(double timestamp) {
    return new SplineSample(timestamp, pose, velocity, acceleration, curvature);
  }
}
