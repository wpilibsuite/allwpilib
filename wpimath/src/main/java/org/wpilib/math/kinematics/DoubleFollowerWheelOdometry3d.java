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

  /**
   * Constructs a DoubleFollowerWheelOdometry3d object.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current wheel position readings.
   * @param initialPose The starting position of the robot on the field.
   */
  public DoubleFollowerWheelOdometry3d(
      double xWheelYPos,
      double yWheelXPos,
      Rotation3d gyroAngle,
      DoubleFollowerWheelPositions wheelPositions,
      Pose3d initialPose) {
    super(gyroAngle, initialPose);
    m_xWheelYPos = xWheelYPos;
    m_yWheelXPos = yWheelXPos;
    m_previousAngle = gyroAngle;
    m_previousWheelPositions = wheelPositions;
  }

  /**
   * Constructs a DoubleFollowerWheelOdometry3d object with the default pose at the origin.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current wheel position readings.
   */
  public DoubleFollowerWheelOdometry3d(
      double xWheelYPos,
      double yWheelXPos,
      Rotation3d gyroAngle,
      DoubleFollowerWheelPositions wheelPositions) {
    this(xWheelYPos, yWheelXPos, gyroAngle, wheelPositions, Pose3d.kZero);
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
    final var deltaRotation = gyroAngle.relativeTo(m_previousAngle);
    final var deltaTheta = deltaRotation.toRotation2d().getRadians();
    final var deltaX = wheelPositions.x - m_previousWheelPositions.x + m_xWheelYPos * deltaTheta;
    final var deltaY = wheelPositions.y - m_previousWheelPositions.y - m_yWheelXPos * deltaTheta;
    m_previousAngle = gyroAngle;
    m_previousWheelPositions.x = wheelPositions.x;
    m_previousWheelPositions.y = wheelPositions.y;
    return update(gyroAngle, new Twist2d(deltaX, deltaY, deltaTheta));
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
