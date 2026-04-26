package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

/** Represents the wheel velocities for a triple follower wheel odometry setup. */
public class TripleFollowerWheelVelocities
    implements Interpolatable<TripleFollowerWheelVelocities> {
  /** Velocity measured by the 1st forward-facing wheel in meters per second. */
  public double vx1;

  /** Velocity measured by the 2nd forward-facing wheel in meters per second. */
  public double vx2;

  /** Velocity measured by the left-facing wheel in meters per second. */
  public double y;

  /**
   * Constructs a TripleFollowerWheelVelocities.
   *
   * @param vx1 Velocity measured by the 1st forward-facing wheel in meters per second.
   * @param vx2 Velocity measured by the 2nd forward-facing wheel in meters per second.
   * @param y Velocity measured by the left-facing wheel in meters per second.
   */
  public TripleFollowerWheelVelocities(double vx1, double vx2, double y) {
    this.vx1 = vx1;
    this.vx2 = vx2;
    this.y = y;
  }

  @Override
  public TripleFollowerWheelVelocities interpolate(TripleFollowerWheelVelocities other, double t) {
    return new TripleFollowerWheelVelocities(
        MathUtil.lerp(this.vx1, other.vx1, t),
        MathUtil.lerp(this.vx2, other.vx2, t),
        MathUtil.lerp(this.y, other.y, t));
  }
}
