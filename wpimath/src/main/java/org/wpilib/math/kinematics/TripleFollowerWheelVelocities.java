package org.wpilib.math.kinematics;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.util.MathUtil;

public class TripleFollowerWheelVelocities
    implements Interpolatable<TripleFollowerWheelVelocities> {
  public double xWheel1;
  public double xWheel2;
  public double yWheel;

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
