package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

/** Represents the wheel positions for a triple follower wheel odometry setup. */
public class TripleFollowerWheelPositions implements Interpolatable<TripleFollowerWheelPositions> {
  /** Distance measured by the 1st forward-facing wheel in meters. */
  public double x1;

  /** Distance measured by the 2nd forward-facing wheel in meters. */
  public double x2;

  /** Distance measured by the left-facing wheel in meters. */
  public double y;

  /**
   * Constructs a TripleFollowerWheelPositions.
   *
   * @param x1 Distance measured by the 1st forward-facing wheel in meters.
   * @param x2 Distance measured by the 2nd forward-facing wheel in meters.
   * @param y Distance measured by the left-facing wheel in meters.
   */
  public TripleFollowerWheelPositions(double x1, double x2, double y) {
    this.x1 = x1;
    this.x2 = x2;
    this.y = y;
  }

  @Override
  public TripleFollowerWheelPositions interpolate(TripleFollowerWheelPositions other, double t) {
    return new TripleFollowerWheelPositions(
        MathUtil.lerp(this.x1, other.x1, t),
        MathUtil.lerp(this.x2, other.x2, t),
        MathUtil.lerp(this.y, other.y, t));
  }
}
