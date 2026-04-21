package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

public class DoubleFollowerWheelAccelerations
    implements Interpolatable<DoubleFollowerWheelAccelerations> {
  public double ax;
  public double ay;
  public double alpha;

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
