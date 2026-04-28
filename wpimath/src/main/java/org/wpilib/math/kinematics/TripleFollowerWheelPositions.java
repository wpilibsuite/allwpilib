package org.wpilib.math.kinematics;

public class TripleFollowerWheelPositions {
  /**
   * The distance traveled by the first forward-facing wheel in meters.
   */
  public double x1;
  /**
   * The distance traveled by the second forward-facing wheel in meters.
   */
  public double x2;
  /**
   * The distance traveled by the left-facing wheel in meters.
   */
  public double y;

  /**
   * Constructs a TripleFollowerWheelPositions object.
   * @param x1 The distance traveled by the first forward-facing wheel in meters.
   * @param x2 The distance traveled by the second forward-facing wheel in meters.
   * @param y The distance traveled by the left-facing wheel in meters.
   */
  public TripleFollowerWheelPositions(double x1, double x2, double y) {
    this.x1 = x1;
    this.x2 = x2;
    this.y = y;
  }
}
