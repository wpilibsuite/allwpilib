// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Twist2d;

/**
 * Helper class that converts follower wheel velocities (vx, vy) and a gyro velocity (dtheta) into
 * chassis velocities (dx, dy, dtheta) and vice versa for a double follower wheel odometry setup.
 *
 * <p>Inverse kinematics converts a desired chassis velocity into wheel velocities, whereas forward
 * kinematics converts wheel velocities into a chassis velocity.
 *
 * <p>This class is primarily used for odometry -- determining the position of the robot on the
 * field using forward kinematics with wheel encoders and a gyro.
 */
public class DoubleFollowerWheelKinematics
    implements Kinematics<
        DoubleFollowerWheelPositions,
        DoubleFollowerWheelVelocities,
        DoubleFollowerWheelAccelerations> {
  private final double xWheelYPos;
  private final double yWheelXPos;

  /**
   * Constructs a DoubleFollowerWheelKinematics object.
   * @param xWheelYPos The distance from the center of the robot to the forward-facing wheel along
   *                   the y-axis in meters.
   * @param yWheelXPos The distance from the center of the robot to the left-facing wheel along the
   *                   x-axis in meters.
   */
  public DoubleFollowerWheelKinematics(double xWheelYPos, double yWheelXPos) {
    this.xWheelYPos = xWheelYPos;
    this.yWheelXPos = yWheelXPos;
  }

  @Override
  public ChassisVelocities toChassisVelocities(DoubleFollowerWheelVelocities wheelVelocities) {
    return new ChassisVelocities(
        wheelVelocities.vx + xWheelYPos * wheelVelocities.omega,
        wheelVelocities.vy - yWheelXPos * wheelVelocities.omega,
        wheelVelocities.omega);
  }

  @Override
  public DoubleFollowerWheelVelocities toWheelVelocities(ChassisVelocities chassisVelocities) {
    return new DoubleFollowerWheelVelocities(
        chassisVelocities.vx - xWheelYPos * chassisVelocities.omega,
        chassisVelocities.vy + yWheelXPos * chassisVelocities.omega,
        chassisVelocities.omega);
  }

  @Override
  public ChassisAccelerations toChassisAccelerations(
      DoubleFollowerWheelAccelerations wheelAccelerations) {
    return new ChassisAccelerations(
        wheelAccelerations.ax + xWheelYPos * wheelAccelerations.alpha,
        wheelAccelerations.ay - yWheelXPos * wheelAccelerations.alpha,
        wheelAccelerations.alpha);
  }

  @Override
  public DoubleFollowerWheelAccelerations toWheelAccelerations(
      ChassisAccelerations chassisAccelerations) {
    return new DoubleFollowerWheelAccelerations(
        chassisAccelerations.ax - xWheelYPos * chassisAccelerations.alpha,
        chassisAccelerations.ay + yWheelXPos * chassisAccelerations.alpha,
        chassisAccelerations.alpha);
  }

  @Override
  public Twist2d toTwist2d(DoubleFollowerWheelPositions start, DoubleFollowerWheelPositions end) {
    var deltaTheta = end.theta.minus(start.theta).getRadians();
    var deltaX = end.x - start.x + xWheelYPos * deltaTheta;
    var deltaY = end.y - start.y - yWheelXPos * deltaTheta;
    return new Twist2d(deltaX, deltaY, deltaTheta);
  }

  @Override
  public DoubleFollowerWheelPositions copy(DoubleFollowerWheelPositions positions) {
    return new DoubleFollowerWheelPositions(
        positions.x, positions.y, new Rotation2d(positions.theta.getRadians()));
  }

  @Override
  public void copyInto(
      DoubleFollowerWheelPositions positions, DoubleFollowerWheelPositions output) {}

  @Override
  public DoubleFollowerWheelPositions interpolate(
      DoubleFollowerWheelPositions startValue, DoubleFollowerWheelPositions endValue, double t) {
    return null;
  }
}
