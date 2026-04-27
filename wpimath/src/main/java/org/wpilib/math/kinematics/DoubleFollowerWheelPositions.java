// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

/** Represents the wheel positions and gyro yaw for a double follower wheel odometry setup. */
public class DoubleFollowerWheelPositions implements Interpolatable<DoubleFollowerWheelPositions> {
  /** Distance measured by the forward-facing wheels in meters. */
  public double x;

  /** Distance measured by the left-facing wheel in meters. */
  public double y;

  /** Gyro yaw angle. */
  public Rotation2d theta;

  /**
   * Constructs a DoubleFollowerWheelPositions.
   *
   * @param x Distance measured by the forward-facing wheels in meters.
   * @param y Distance measured by the left-facing wheel in meters.
   * @param theta Gyro yaw angle.
   */
  public DoubleFollowerWheelPositions(double x, double y, Rotation2d theta) {
    this.x = x;
    this.y = y;
    this.theta = theta;
  }

  @Override
  public DoubleFollowerWheelPositions interpolate(DoubleFollowerWheelPositions other, double t) {
    return new DoubleFollowerWheelPositions(
        MathUtil.lerp(this.x, other.x, t),
        MathUtil.lerp(this.y, other.y, t),
        this.theta.interpolate(other.theta, t));
  }
}
