// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.util.MathSharedStore;

/**
 * Class for triple follower wheel odometry. Odometry allows you to track the robot's position on
 * the field over the course of a match using readings from 3 follower wheel encoders.
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class TripleFollowerWheelOdometry extends Odometry<TripleFollowerWheelPositions> {
  /**
   * Constructs a TripleFollowerWheelOdometry object.
   *
   * @param kinematics The triple follower wheel kinematics.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   * @param initialPose The starting position of the robot on the field.
   */
  public TripleFollowerWheelOdometry(
      TripleFollowerWheelKinematics kinematics,
      Rotation2d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose2d initialPose) {
    super(kinematics, gyroAngle, wheelPositions, initialPose);

    MathSharedStore.reportUsage("TripleFollowerWheelOdometry", "");
  }

  /**
   * Updates the robot's position on the field using forward kinematics and integration if the pose
   * over time.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param x1 The distance traveled by the 1st forward-facing wheel in meters.
   * @param x2 The distance traveled by the 2nd forward-facing wheel in meters.
   * @param y The distance traveled by the left-facing wheel in meters.
   * @return The new pose of the robot.
   */
  public Pose2d update(Rotation2d gyroAngle, double x1, double x2, double y) {
    return super.update(gyroAngle, new TripleFollowerWheelPositions(x1, x2, y));
  }
}
