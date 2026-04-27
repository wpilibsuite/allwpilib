// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

/**
 * Represents the wheel velocities and angular velocity for a double follower wheel odometry setup.
 */
public class DoubleFollowerWheelVelocities
    implements Interpolatable<DoubleFollowerWheelVelocities> {
  /** Velocity measured by the forward-facing wheels in meters per second. */
  public double vx;

  /** Velocity measured by the left-facing wheel in meters per second. */
  public double vy;

  /** Gyro angular velocity in radians per second. */
  public double omega;

  /**
   * Constructs a DoubleFollowerWheelVelocities.
   *
   * @param vx Velocity measured by the forward-facing wheels in meters per second.
   * @param vy Velocity measured by the left-facing wheel in meters per second.
   * @param omega Gyro angular velocity in radians per second.
   */
  public DoubleFollowerWheelVelocities(double vx, double vy, double omega) {
    this.vx = vx;
    this.vy = vy;
    this.omega = omega;
  }

  @Override
  public DoubleFollowerWheelVelocities interpolate(
      DoubleFollowerWheelVelocities endValue, double t) {
    return new DoubleFollowerWheelVelocities(
        MathUtil.lerp(this.vx, endValue.vx, t),
        MathUtil.lerp(this.vy, endValue.vy, t),
        MathUtil.lerp(this.omega, endValue.omega, t));
  }
}
