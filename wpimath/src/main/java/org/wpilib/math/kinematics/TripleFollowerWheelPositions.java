package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

/**
 * Represents the wheel positions for a triple follower wheel odometry setup.
 */
public class TripleFollowerWheelPositions implements Interpolatable<TripleFollowerWheelPositions> {
  /**
   * Distance measured by the 1st forward-facing wheel in meters.
   */
  public double xWheel1;
  /**
   * Distance measured by the 2nd forward-facing wheel in meters.
   */
  public double xWheel2;
  /**
   * Distance measured by the left-facing wheel in meters.
   */
  public double yWheel;

  /**
   * Constructs a TripleFollowerWheelPositions.
   * @param xWheel1 Distance measured by the 1st forward-facing wheel in meters.
   * @param xWheel2 Distance measured by the 2nd forward-facing wheel in meters.
   * @param yWheel Distance measured by the left-facing wheel in meters.
   */
  public TripleFollowerWheelPositions(double xWheel1, double xWheel2, double yWheel) {
    this.xWheel1 = xWheel1;
    this.xWheel2 = xWheel2;
    this.yWheel = yWheel;
  }

  @Override
  public TripleFollowerWheelPositions interpolate(TripleFollowerWheelPositions other, double t) {
    return new TripleFollowerWheelPositions(
        MathUtil.lerp(this.xWheel1, other.xWheel1, t),
        MathUtil.lerp(this.xWheel2, other.xWheel2, t),
        MathUtil.lerp(this.yWheel, other.yWheel, t));
  }
}
