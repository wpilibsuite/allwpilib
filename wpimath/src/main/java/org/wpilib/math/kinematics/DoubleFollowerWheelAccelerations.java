package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

/**
 * Represents the wheel accelerations and angular acceleration for a double follower wheel odometry
 * setup.
 */
public class DoubleFollowerWheelAccelerations
    implements Interpolatable<DoubleFollowerWheelAccelerations> {
  /** Acceleration measured by the forward-facing wheels in meters per second squared. */
  public double ax;

  /** Acceleration measured by the left-facing wheel in meters per second squared. */
  public double ay;

  /** Gyro angular acceleration in radians per second squared. */
  public double alpha;

  /**
   * Constructs a DoubleFollowerWheelAccelerations.
   *
   * @param ax Acceleration measured by the forward-facing wheels in meters per second squared.
   * @param ay Acceleration measured by the left-facing wheel in meters per second squared.
   * @param alpha Gyro angular acceleration in radians per second squared.
   */
  public DoubleFollowerWheelAccelerations(double ax, double ay, double alpha) {
    this.ax = ax;
    this.ay = ay;
    this.alpha = alpha;
  }

  @Override
  public DoubleFollowerWheelAccelerations interpolate(
      DoubleFollowerWheelAccelerations endValue, double t) {
    return new DoubleFollowerWheelAccelerations(
        MathUtil.lerp(this.ax, endValue.ax, t),
        MathUtil.lerp(this.ay, endValue.ay, t),
        MathUtil.lerp(this.alpha, endValue.alpha, t));
  }
}
