package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

/** Represents the wheel accelerations for a triple follower wheel odometry setup. */
public class TripleFollowerWheelAccelerations
    implements Interpolatable<TripleFollowerWheelAccelerations> {
  /** Acceleration measured by the 1st forward-facing wheel in meters per second squared. */
  public double ax1;

  /** Acceleration measured by the 2nd forward-facing wheel in meters per second squared. */
  public double ax2;

  /** Acceleration measured by the left-facing wheel in meters per second squared. */
  public double ay;

  /**
   * Constructs a TripleFollowerWheelAccelerations.
   *
   * @param ax1 Acceleration measured by the 1st forward-facing wheel in meters per second squared.
   * @param ax2 Acceleration measured by the 2nd forward-facing wheel in meters per second squared.
   * @param ay Acceleration measured by the left-facing wheel in meters per second squared.
   */
  public TripleFollowerWheelAccelerations(double ax1, double ax2, double ay) {
    this.ax1 = ax1;
    this.ax2 = ax2;
    this.ay = ay;
  }

  @Override
  public TripleFollowerWheelAccelerations interpolate(
      TripleFollowerWheelAccelerations other, double t) {
    return new TripleFollowerWheelAccelerations(
        MathUtil.lerp(this.ax1, other.ax1, t),
        MathUtil.lerp(this.ax2, other.ax2, t),
        MathUtil.lerp(this.ay, other.ay, t));
  }
}
