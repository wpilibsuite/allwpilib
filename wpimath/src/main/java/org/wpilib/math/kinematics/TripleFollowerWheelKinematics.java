// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Twist2d;

/**
 * Helper class that converts follower wheel velocities (vx1, vx2, vy) into chassis velocities (dx,
 * dy, dtheta) and vice versa for a triple follower wheel odometry setup.
 *
 * <p>Inverse kinematics converts a desired chassis velocity into wheel velocities, whereas forward
 * kinematics converts wheel velocities into a chassis velocity.
 *
 * <p>This class is primarily used for odometry -- determining the position of the robot on the
 * field using forward kinematics with wheel encoders and a gyro.
 */
public class TripleFollowerWheelKinematics
    implements Kinematics<
        TripleFollowerWheelPositions,
        TripleFollowerWheelVelocities,
        TripleFollowerWheelAccelerations> {
  // TODO: IDK what to name these
  private final double xWheel1YPos;
  private final double xWheel2YPos;
  private final double yWheelXPos;

  /**
   * Constructs a TripleFollowerWheelKinematics object.
   *
   * @param xWheel1YPos The distance from the center of the robot to the 1st forward-facing wheel
   *     along the y-axis in meters.
   * @param xWheel2YPos The distance from the center of the robot to the 2nd forward-facing wheel
   *     along the y-axis in meters.
   * @param yWheelXPos The distance from the center of the robot to the left-facing wheel along the
   *     x-axis in meters.
   */
  public TripleFollowerWheelKinematics(double xWheel1YPos, double xWheel2YPos, double yWheelXPos) {
    this.xWheel1YPos = xWheel1YPos;
    this.xWheel2YPos = xWheel2YPos;
    this.yWheelXPos = yWheelXPos;
  }

  @Override
  public ChassisVelocities toChassisVelocities(TripleFollowerWheelVelocities wheelVelocities) {
    var omega = (wheelVelocities.vx1 - wheelVelocities.vx2) / (xWheel2YPos - xWheel1YPos);
    var vx = (wheelVelocities.vx1 + wheelVelocities.vx2) / 2;
    var vy = wheelVelocities.vy - yWheelXPos * omega;
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
    var alpha = (wheelAccelerations.ax1 - wheelAccelerations.ax2) / (xWheel2YPos - xWheel1YPos);
    var ax = (wheelAccelerations.ax1 + wheelAccelerations.ax2) / 2;
    var ay = wheelAccelerations.ay - yWheelXPos * alpha;
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
    var deltaXWheel1 = end.x1 - start.x1;
    var deltaXWheel2 = end.x2 - start.x2;
    var deltaYWheel = end.y - start.y;

    var deltaTheta = (deltaXWheel1 - deltaXWheel2) / (xWheel2YPos - xWheel1YPos);
    var deltaX = (deltaXWheel1 + deltaXWheel2) / 2;
    var deltaY = deltaYWheel - yWheelXPos * deltaTheta;
    return new Twist2d(deltaX, deltaY, deltaTheta);
  }

  @Override
  public TripleFollowerWheelPositions copy(TripleFollowerWheelPositions positions) {
    return new TripleFollowerWheelPositions(positions.x1, positions.x2, positions.y);
  }

  @Override
  public void copyInto(
      TripleFollowerWheelPositions positions, TripleFollowerWheelPositions output) {
    output.x1 = positions.x1;
    output.x2 = positions.x2;
    output.y = positions.y;
  }

  @Override
  public TripleFollowerWheelPositions interpolate(
      TripleFollowerWheelPositions startValue, TripleFollowerWheelPositions endValue, double t) {
    return startValue.interpolate(endValue, t);
  }
}
