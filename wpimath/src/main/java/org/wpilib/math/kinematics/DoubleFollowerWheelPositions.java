// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

/** Represents the wheel positions of a double follower wheel odometry system.. */
public class DoubleFollowerWheelPositions {
  /** The distance traveled by the forward-facing wheel in meters. */
  public double x;

  /** The distance traveled by the left-facing wheel in meters. */
  public double y;

  /**
   * Constructs a DoubleFollowerWheelPositions.
   *
   * @param x The distance traveled by the forward-facing wheel in meters.
   * @param y The distance traveled by the left-facing wheel in meters.
   */
  public DoubleFollowerWheelPositions(double x, double y) {
    this.x = x;
    this.y = y;
  }

  /** Constructs a DoubleFollowerWheelPositions with zeros for all member fields. */
  public DoubleFollowerWheelPositions() {
    this(0, 0);
  }
}
