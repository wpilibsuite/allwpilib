// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.geometry.Twist2d;

/**
 * Class for double follower wheel odometry. Odometry allows you to track the robot's position on
 * the field over the course of a match using readings from 2 encoders and a gyroscope.
 *
 * <p>This class is meant to be an easy replacement for {@link DifferentialDriveOdometry}, only
 * requiring the addition of appropriate conversions between 2D and 3D versions of geometry classes.
 * (See {@link Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class TwoDeadWheelOdometry3d extends Odometry3d<TwoDeadWheelPositions> {
  private final double m_xWheelYPos;
  private final double m_yWheelXPos;

  private Rotation3d m_previousAngle;
  private final TwoDeadWheelPositions m_previousWheelPositions;

  /**
   * Constructs a TwoDeadWheelOdometry3d object.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current wheel position readings.
   * @param initialPose The starting position of the robot on the field.
   */
  public TwoDeadWheelOdometry3d(
      double xWheelYPos,
      double yWheelXPos,
      Rotation3d gyroAngle,
      TwoDeadWheelPositions wheelPositions,
      Pose3d initialPose) {
    super(gyroAngle, initialPose);
    m_xWheelYPos = xWheelYPos;
    m_yWheelXPos = yWheelXPos;
    m_previousAngle = gyroAngle;
    m_previousWheelPositions = wheelPositions;
  }

  /**
   * Constructs a TwoDeadWheelOdometry3d object with the default pose at the origin.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current wheel position readings.
   */
  public TwoDeadWheelOdometry3d(
      double xWheelYPos,
      double yWheelXPos,
      Rotation3d gyroAngle,
      TwoDeadWheelPositions wheelPositions) {
    this(xWheelYPos, yWheelXPos, gyroAngle, wheelPositions, Pose3d.kZero);
  }

  @Override
  public void resetPosition(
      Rotation3d gyroAngle, TwoDeadWheelPositions wheelPositions, Pose3d pose) {
    m_previousAngle = gyroAngle;
    m_previousWheelPositions.x = wheelPositions.x;
    m_previousWheelPositions.y = wheelPositions.y;
    resetPosition(gyroAngle, pose);
  }

  @Override
  public Pose3d update(Rotation3d gyroAngle, TwoDeadWheelPositions wheelPositions) {
    final var deltaRotation = gyroAngle.relativeTo(m_previousAngle);
    final var deltaTheta = deltaRotation.toRotation2d().getRadians();
    final var deltaX = wheelPositions.x - m_previousWheelPositions.x + deltaTheta * m_xWheelYPos;
    final var deltaY = wheelPositions.y - m_previousWheelPositions.y - deltaTheta * m_yWheelXPos;
    m_previousAngle = gyroAngle;
    m_previousWheelPositions.x = wheelPositions.x;
    m_previousWheelPositions.y = wheelPositions.y;
    return update(gyroAngle, new Twist2d(deltaX, deltaY, deltaTheta));
  }

  /**
   * Converts measured wheel velocities to chassis velocities using inverse kinematics.
   *
   * @param vx    The velocity of the forward-facing wheel, in meters per second.
   * @param vy    The velocity of the sideways-facing wheel, in meters per second.
   * @param omega The angular velocity of the robot as reported by the gyro, in radians per second.
   * @return The velocity of the chassis.
   */
  public ChassisVelocities toChassisVelocities(double vx, double vy, double omega) {
    return new ChassisVelocities(vx + omega * m_xWheelYPos, vy - omega * m_yWheelXPos, omega);
  }
}
