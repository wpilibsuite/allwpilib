package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

/**
 * Represents the wheel accelerations for a triple follower wheel odometry setup.
 */
public class TripleFollowerWheelAccelerations
    implements Interpolatable<TripleFollowerWheelAccelerations> {
  /**
   * Acceleration measured by the 1st forward-facing wheel in meters per second squared.
   */
  public double xWheel1;
  /**
   * Acceleration measured by the 2nd forward-facing wheel in meters per second squared.
   */
  public double xWheel2;
  /**
   * Acceleration measured by the left-facing wheel in meters per second squared.
   */
  public double yWheel;

  /**
   * Constructs a TripleFollowerWheelAccelerations.
   * @param xWheel1 Acceleration measured by the 1st forward-facing wheel in meters per second squared.
   * @param xWheel2 Acceleration measured by the 2nd forward-facing wheel in meters per second squared.
   * @param yWheel Acceleration measured by the left-facing wheel in meters per second squared.
   */
  public TripleFollowerWheelAccelerations(double xWheel1, double xWheel2, double yWheel) {
    this.xWheel1 = xWheel1;
    this.xWheel2 = xWheel2;
    this.yWheel = yWheel;
  }

  @Override
  public TripleFollowerWheelAccelerations interpolate(
      TripleFollowerWheelAccelerations other, double t) {
    return new TripleFollowerWheelAccelerations(
        MathUtil.lerp(this.xWheel1, other.xWheel1, t),
        MathUtil.lerp(this.xWheel2, other.xWheel2, t),
        MathUtil.lerp(this.yWheel, other.yWheel, t));
  }
}
