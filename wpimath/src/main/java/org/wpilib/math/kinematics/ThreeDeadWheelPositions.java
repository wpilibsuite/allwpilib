// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

/** Represents the wheel positions of a triple follower wheel odometry system. */
public class ThreeDeadWheelPositions {
  /** The distance traveled by the first forward-facing wheel in meters. */
  public double x1;

  /** The distance traveled by the second forward-facing wheel in meters. */
  public double x2;

  /** The distance traveled by the left-facing wheel in meters. */
  public double y;

  /**
   * Constructs a ThreeDeadWheelPositions.
   *
   * @param x1 The distance traveled by the first forward-facing wheel in meters.
   * @param x2 The distance traveled by the second forward-facing wheel in meters.
   * @param y The distance traveled by the left-facing wheel in meters.
   */
  public ThreeDeadWheelPositions(double x1, double x2, double y) {
    this.x1 = x1;
    this.x2 = x2;
    this.y = y;
  }

  /** Constructs a ThreeDeadWheelPositions with zeros for all member fields. */
  public ThreeDeadWheelPositions() {
    this(0, 0, 0);
  }
}
