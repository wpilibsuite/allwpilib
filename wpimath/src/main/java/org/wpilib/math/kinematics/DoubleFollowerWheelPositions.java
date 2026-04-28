package org.wpilib.math.kinematics;

public class DoubleFollowerWheelPositions {
  /**
   * The distance traveled by the forward-facing wheel in meters.
   */
  public double x;
  /**
   * The distance traveled by the left-facing wheel in meters.
   */
  public double y;

  /**
   * Constructs a DoubleFollowerWheelPositions object.
   * @param x The distance traveled by the forward-facing wheel in meters.
   * @param y The distance traveled by the left-facing wheel in meters.
   */
  public DoubleFollowerWheelPositions(double x, double y) {
    this.x = x;
    this.y = y;
  }
}
