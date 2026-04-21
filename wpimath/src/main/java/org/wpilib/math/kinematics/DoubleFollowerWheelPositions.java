package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

public class DoubleFollowerWheelPositions implements Interpolatable<DoubleFollowerWheelPositions> {
  public double x;
  public double y;
  public Rotation2d gyroTheta;

  public DoubleFollowerWheelPositions(double x, double y, Rotation2d gyroTheta) {
    this.x = x;
    this.y = y;
    this.gyroTheta = gyroTheta;
  }

  @Override
  public DoubleFollowerWheelPositions interpolate(DoubleFollowerWheelPositions other, double t) {
    return new DoubleFollowerWheelPositions(
        MathUtil.lerp(this.x, other.x, t),
        MathUtil.lerp(this.y, other.y, t),
        this.gyroTheta.interpolate(other.gyroTheta, t));
  }
}
