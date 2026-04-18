package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

public class TripleFollowerWheelAccelerations
    implements Interpolatable<TripleFollowerWheelAccelerations> {
  public double xWheel1;
  public double xWheel2;
  public double yWheel;

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
