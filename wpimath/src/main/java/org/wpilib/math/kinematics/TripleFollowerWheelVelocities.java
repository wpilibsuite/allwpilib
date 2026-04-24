package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

/**
 * Represents the wheel velocities for a triple follower wheel odometry setup.
 */
public class TripleFollowerWheelVelocities
    implements Interpolatable<TripleFollowerWheelVelocities> {
  /**
   * Velocity measured by the 1st forward-facing wheel in meters per second.
   */
  public double xWheel1;
  /**
   * Velocity measured by the 2nd forward-facing wheel in meters per second.
   */
  public double xWheel2;
  /**
   * Velocity measured by the left-facing wheel in meters per second.
   */
  public double yWheel;

  /**
   * Constructs a TripleFollowerWheelVelocities.
   * @param xWheel1 Velocity measured by the 1st forward-facing wheel in meters per second.
   * @param xWheel2 Velocity measured by the 2nd forward-facing wheel in meters per second.
   * @param yWheel Velocity measured by the left-facing wheel in meters per second.
   */
  public TripleFollowerWheelVelocities(double xWheel1, double xWheel2, double yWheel) {
    this.xWheel1 = xWheel1;
    this.xWheel2 = xWheel2;
    this.yWheel = yWheel;
  }

  @Override
  public TripleFollowerWheelVelocities interpolate(TripleFollowerWheelVelocities other, double t) {
    return new TripleFollowerWheelVelocities(
        MathUtil.lerp(this.xWheel1, other.xWheel1, t),
        MathUtil.lerp(this.xWheel2, other.xWheel2, t),
        MathUtil.lerp(this.yWheel, other.yWheel, t));
  }
}
