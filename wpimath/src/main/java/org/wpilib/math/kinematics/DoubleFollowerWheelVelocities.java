package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

public class DoubleFollowerWheelVelocities
    implements Interpolatable<DoubleFollowerWheelVelocities> {
  public double vx;
  public double vy;
  public double omega;

  public DoubleFollowerWheelVelocities(double vx, double vy, double omega) {
    this.vx = vx;
    this.vy = vy;
    this.omega = omega;
  }

  @Override
  public DoubleFollowerWheelVelocities interpolate(
      DoubleFollowerWheelVelocities endValue, double t) {
    return new DoubleFollowerWheelVelocities(
        MathUtil.lerp(this.vx, endValue.vx, t),
        MathUtil.lerp(this.vy, endValue.vy, t),
        MathUtil.lerp(this.omega, endValue.omega, t));
  }
}
