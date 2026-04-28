// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Twist2d;

public class DoubleFollowerWheelOdometry3d extends Odometry3d<DoubleFollowerWheelPositions> {
  private final double m_xWheelYPos;
  private final double m_yWheelXPos;

  private Rotation3d m_previousAngle;
  private final DoubleFollowerWheelPositions m_previousWheelPositions;

  public DoubleFollowerWheelOdometry3d(
      double xWheelYPos,
      double yWheelXPos,
      Rotation3d gyroAngle,
      DoubleFollowerWheelPositions modulePositions,
      Pose3d initialPose) {
    super(gyroAngle, initialPose);
    m_xWheelYPos = xWheelYPos;
    m_yWheelXPos = yWheelXPos;
    m_previousAngle = gyroAngle;
    m_previousWheelPositions = modulePositions;
  }

  @Override
  public void resetPosition(
      Rotation3d gyroAngle, DoubleFollowerWheelPositions wheelPositions, Pose3d pose) {
    m_previousAngle = gyroAngle;
    m_previousWheelPositions.x = wheelPositions.x;
    m_previousWheelPositions.y = wheelPositions.y;
    resetPosition(gyroAngle, pose);
  }

  @Override
  public Pose3d update(Rotation3d gyroAngle, DoubleFollowerWheelPositions wheelPositions) {
    var deltaAngle = gyroAngle.relativeTo(m_previousAngle);
    var deltaTheta = deltaAngle.toRotation2d().getRadians();
    var deltaX = wheelPositions.x - m_previousWheelPositions.x + m_xWheelYPos * deltaTheta;
    var deltaY = wheelPositions.y - m_previousWheelPositions.y - m_yWheelXPos * deltaTheta;
    var twist = new Twist2d(deltaX, deltaY, deltaTheta);
    m_previousAngle = gyroAngle;
    m_previousWheelPositions.x = wheelPositions.x;
    m_previousWheelPositions.y = wheelPositions.y;
    return update(gyroAngle, twist);
  }

  /**
   * Converts measured wheel velocities to chassis velocities using inverse kinematics.
   *
   * @param omega The angular velocity of the robot as reported by the gyro, in radians per second.
   * @param vx The velocity of the forward-facing wheel, in meters per second.
   * @param vy The velocity of the sideways-facing wheel, in meters per second.
   * @return The velocity of the chassis.
   */
  public ChassisVelocities toChassisVelocities(double omega, double vx, double vy) {
    return new ChassisVelocities(vx + m_xWheelYPos * omega, vy - m_yWheelXPos * omega, omega);
  }
}
