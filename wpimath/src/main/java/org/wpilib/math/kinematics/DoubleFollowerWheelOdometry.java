// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.util.MathSharedStore;

// TODO: Should I do an impl that doesn't extend Odometry<> since the super methods that take
//  in redundant gyro angles could be confusing?
//  But also that would be duplicated code for similar purposes
//  Maybe it's fine since the formulation would be different enough?
/**
 * Class for double follower wheel odometry. Odometry allows you to track the robot's position on
 * the field over the course of a match using readings from 2 encoders and a gyroscope.
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class DoubleFollowerWheelOdometry extends Odometry<DoubleFollowerWheelPositions> {
  /**
   * Constructs a DoubleFollowerWheelOdometry object.
   *
   * @param kinematics The double follower wheel kinematics.
   * @param wheelPositions The distances driven by each wheel and the angle reported by the
   *     gyroscope.
   * @param initialPose The starting position of the robot on the field.
   */
  public DoubleFollowerWheelOdometry(
      DoubleFollowerWheelKinematics kinematics,
      DoubleFollowerWheelPositions wheelPositions,
      Pose2d initialPose) {
    super(kinematics, wheelPositions.theta, wheelPositions, initialPose);

    MathSharedStore.reportUsage("DoubleFollowerWheelOdometry", "");
  }

  /**
   * Constructs a DoubleFollowerWheelOdometry object.
   *
   * @param kinematics The double follower wheel kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param x The distance traveled by the forward-facing wheel in meters.
   * @param y The distance traveled by the left-facing wheel in meters.
   * @param initialPose The starting position of the robot on the field.
   */
  public DoubleFollowerWheelOdometry(
      DoubleFollowerWheelKinematics kinematics,
      Rotation2d gyroAngle,
      double x,
      double y,
      Pose2d initialPose) {
    this(kinematics, new DoubleFollowerWheelPositions(x, y, gyroAngle), initialPose);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param wheelPositions The current encoder and gyroscope readings.
   * @param pose The position on the field that your robot is at.
   */
  public void resetPosition(DoubleFollowerWheelPositions wheelPositions, Pose2d pose) {
    super.resetPosition(wheelPositions.theta, wheelPositions, pose);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param x The distance traveled by the forward-facing wheel in meters.
   * @param y The distance traveled by the left-facing wheel in meters.
   * @param pose The position on the field that your robot is at.
   */
  public void resetPosition(Rotation2d gyroAngle, double x, double y, Pose2d pose) {
    super.resetPosition(gyroAngle, new DoubleFollowerWheelPositions(x, y, gyroAngle), pose);
  }

  /**
   * Updates the robot's position on the field using forward kinematics and integration of the pose
   * over time.
   *
   * @param wheelPositions The current encoder and gyroscope readings.
   * @return The new pose of the robot.
   */
  public Pose2d update(DoubleFollowerWheelPositions wheelPositions) {
    return super.update(wheelPositions.theta, wheelPositions);
  }

  /**
   * Updates the robot's position on the field using forward kinematics and integration of the pose
   * over time.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param x The distance traveled by the forward-facing wheel in meters.
   * @param y The distance traveled by the left-facing wheel in meters.
   * @return The new pose of the robot.
   */
  public Pose2d update(Rotation2d gyroAngle, double x, double y) {
    return update(new DoubleFollowerWheelPositions(x, y, gyroAngle));
  }
}
