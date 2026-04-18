package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Twist2d;

public class TripleFollowerWheelKinematics
    implements Kinematics<
        TripleFollowerWheelPositions,
        TripleFollowerWheelVelocities,
        TripleFollowerWheelAccelerations> {
  // TODO: IDK what to name these
  private final double xWheel1YPos;
  private final double xWheel2YPos;
  private final double yWheelXPos;

  public TripleFollowerWheelKinematics(double xWheel1YPos, double xWheel2YPos, double yWheelXPos) {
    this.xWheel1YPos = xWheel1YPos;
    this.xWheel2YPos = xWheel2YPos;
    this.yWheelXPos = yWheelXPos;

    if (Math.signum(xWheel1YPos) == Math.signum(xWheel2YPos)) {
      throw new IllegalArgumentException(
          "The x axis wheels must be on opposite sides of the robot");
    }
  }

  @Override
  public ChassisVelocities toChassisVelocities(TripleFollowerWheelVelocities wheelVelocities) {
    var omega = (wheelVelocities.xWheel1 - wheelVelocities.xWheel2) / (xWheel2YPos - xWheel1YPos);
    var vx = (wheelVelocities.xWheel1 + wheelVelocities.xWheel2) / 2;
    var vy = wheelVelocities.yWheel - yWheelXPos * omega;
    return new ChassisVelocities(vx, vy, omega);
  }

  @Override
  public TripleFollowerWheelVelocities toWheelVelocities(ChassisVelocities chassisVelocities) {
    return new TripleFollowerWheelVelocities(
        chassisVelocities.vx - chassisVelocities.omega * xWheel1YPos,
        chassisVelocities.vx - chassisVelocities.omega * xWheel2YPos,
        chassisVelocities.vy + chassisVelocities.omega * yWheelXPos);
  }

  @Override
  public ChassisAccelerations toChassisAccelerations(
      TripleFollowerWheelAccelerations wheelAccelerations) {
    var alpha =
        (wheelAccelerations.xWheel1 - wheelAccelerations.xWheel2) / (xWheel2YPos - xWheel1YPos);
    var ax = (wheelAccelerations.xWheel1 + wheelAccelerations.xWheel2) / 2;
    var ay = wheelAccelerations.yWheel - yWheelXPos * alpha;
    return new ChassisAccelerations(ax, ay, alpha);
  }

  @Override
  public TripleFollowerWheelAccelerations toWheelAccelerations(
      ChassisAccelerations chassisAccelerations) {
    return new TripleFollowerWheelAccelerations(
        chassisAccelerations.ax - chassisAccelerations.alpha * xWheel1YPos,
        chassisAccelerations.ax - chassisAccelerations.alpha * xWheel2YPos,
        chassisAccelerations.ay + chassisAccelerations.alpha * yWheelXPos);
  }

  @Override
  public Twist2d toTwist2d(TripleFollowerWheelPositions start, TripleFollowerWheelPositions end) {
    var deltaXWheel1 = end.xWheel1 - start.xWheel1;
    var deltaXWheel2 = end.xWheel2 - start.xWheel2;
    var deltaYWheel = end.yWheel - start.yWheel;

    var deltaTheta = (deltaXWheel1 - deltaXWheel2) / (xWheel2YPos - xWheel1YPos);
    var deltaX = (deltaXWheel1 + deltaXWheel2) / 2;
    var deltaY = deltaYWheel - yWheelXPos * deltaTheta;
    return new Twist2d(deltaX, deltaY, deltaTheta);
  }

  @Override
  public TripleFollowerWheelPositions copy(TripleFollowerWheelPositions positions) {
    return new TripleFollowerWheelPositions(positions.xWheel1, positions.xWheel2, positions.yWheel);
  }

  @Override
  public void copyInto(
      TripleFollowerWheelPositions positions, TripleFollowerWheelPositions output) {
    output.xWheel1 = positions.xWheel1;
    output.xWheel2 = positions.xWheel2;
    output.yWheel = positions.yWheel;
  }

  @Override
  public TripleFollowerWheelPositions interpolate(
      TripleFollowerWheelPositions startValue, TripleFollowerWheelPositions endValue, double t) {
    return startValue.interpolate(endValue, t);
  }
}
