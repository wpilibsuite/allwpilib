// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Twist2d;

public class DoubleFollowerWheelKinematics
    implements Kinematics<
        DoubleFollowerWheelPositions,
        DoubleFollowerWheelVelocities,
        DoubleFollowerWheelAccelerations> {
  private final double xWheelYPos;
  private final double yWheelXPos;

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
