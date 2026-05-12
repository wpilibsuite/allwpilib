// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Twist2d;

/**
 * Class for triple follower wheel odometry. Odometry allows you to track the robot's position on
 * the field over the course of a match using readings from 3 encoders and a gyroscope.
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class TripleFollowerWheelOdometry extends Odometry<TripleFollowerWheelPositions> {
  private final double m_x1WheelYPos;
  private final double m_x2WheelYPos;
  private final double m_yWheelXPos;

  private final TripleFollowerWheelPositions m_previousWheelPositions;

  /**
   * Constructs an DoubleFollowerWheelOdometry object.
   *
   * @param x1WheelYPos The y-position of the first forward-facing wheel relative to the center of
   *     the robot in meters.
   * @param x2WheelYPos The y-position of the second forward-facing wheel relative to the center of
   *     the robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The current wheel position readings.
   * @param initialPose The starting position of the robot on the field.
   */
  public TripleFollowerWheelOdometry(
      double x1WheelYPos,
      double x2WheelYPos,
      double yWheelXPos,
      Rotation2d gyroAngle,
      TripleFollowerWheelPositions modulePositions,
      Pose2d initialPose) {
    super(gyroAngle, initialPose);
    if (Math.abs(x1WheelYPos - x2WheelYPos) < 0.1) {
      throw new IllegalArgumentException(
          "x1WheelYPos and x2WheelYPos must not be too close to each other");
    }
    m_x1WheelYPos = x1WheelYPos;
    m_x2WheelYPos = x2WheelYPos;
    m_yWheelXPos = yWheelXPos;
    m_previousWheelPositions = modulePositions;
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param x1 The distance traveled by the first forward-facing wheel in meters.
   * @param x2 The distance traveled by the second forward-facing wheel in meters.
   * @param y The distance traveled by the left-facing wheel in meters.
   * @param pose The position on the field that your robot is at.
   */
  public void resetPosition(Rotation2d gyroAngle, double x1, double x2, double y, Pose2d pose) {
    resetPosition(gyroAngle, new TripleFollowerWheelPositions(x1, x2, y), pose);
  }

  @Override
  public void resetPosition(
      Rotation2d gyroAngle, TripleFollowerWheelPositions wheelPositions, Pose2d pose) {
    m_previousWheelPositions.x1 = wheelPositions.x1;
    m_previousWheelPositions.x2 = wheelPositions.x2;
    m_previousWheelPositions.y = wheelPositions.y;
    resetPosition(gyroAngle, pose);
  }

  /**
   * Updates the robot's position on the field using forward kinematics and integration of the pose
   * over time. This method takes in an angle parameter which is used instead of the angular rate
   * that is calculated from forward kinematics, in addition to the current distance measurement at
   * each wheel.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param x1 The distance traveled by the first forward-facing wheel in meters.
   * @param x2 The distance traveled by the second forward-facing wheel in meters.
   * @param y The distance traveled by the left-facing wheel in meters.
   * @return The new pose of the robot.
   */
  public Pose2d update(Rotation2d gyroAngle, double x1, double x2, double y) {
    return update(gyroAngle, new TripleFollowerWheelPositions(x1, x2, y));
  }

  @Override
  public Pose2d update(Rotation2d gyroAngle, TripleFollowerWheelPositions wheelPositions) {
    final var deltaXWheel1 = wheelPositions.x1 - m_previousWheelPositions.x1;
    final var deltaXWheel2 = wheelPositions.x2 - m_previousWheelPositions.x2;
    final var deltaYWheel = wheelPositions.y - m_previousWheelPositions.y;

    final var deltaTheta = (deltaXWheel1 - deltaXWheel2) / (m_x2WheelYPos - m_x1WheelYPos);
    final var deltaX = (deltaXWheel1 + deltaXWheel2) / 2;
    final var deltaY = deltaYWheel - deltaTheta * m_yWheelXPos;
    m_previousWheelPositions.x1 = wheelPositions.x1;
    m_previousWheelPositions.x2 = wheelPositions.x2;
    m_previousWheelPositions.y = wheelPositions.y;
    return update(gyroAngle, new Twist2d(deltaX, deltaY, deltaTheta));
  }

  /**
   * Converts measured wheel velocities to chassis velocities using inverse kinematics.
   *
   * @param vx1 The velocity of the first forward-facing wheels, in meters per second.
   * @param vx2 The velocity of the second forward-facing wheel, in meters per second.
   * @param vy The velocity of the sideways-facing wheel, in meters per second.
   * @return The velocity of the chassis.
   */
  public ChassisVelocities toChassisVelocities(double vx1, double vx2, double vy) {
    var omega = (vx1 - vx2) / (m_x2WheelYPos - m_x1WheelYPos);
    return new ChassisVelocities((vx1 + vx2) / 2, vy - omega * m_yWheelXPos, omega);
  }
}
