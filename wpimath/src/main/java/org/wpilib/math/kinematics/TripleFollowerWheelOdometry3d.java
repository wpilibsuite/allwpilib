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
import org.wpilib.math.util.MathSharedStore;

/**
 * Class for triple follower wheel odometry. Odometry allows you to track the robot's position on
 * the field over the course of a match using readings from 3 follower wheel encoders.
 *
 * <p>This class is meant to be an easy replacement for {@link DoubleFollowerWheelOdometry}, only
 * requiring the addition of appropriate conversions between 2D and 3D versions of geometry classes.
 * (See {@link Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class TripleFollowerWheelOdometry3d extends Odometry3d<TripleFollowerWheelPositions> {
  /**
   * Constructs a TripleFollowerWheelOdometry3d object.
   *
   * @param kinematics The triple follower wheel kinematics.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   * @param initialPose The starting position of the robot on the field.
   */
  public TripleFollowerWheelOdometry3d(
      TripleFollowerWheelKinematics kinematics,
      Rotation3d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose3d initialPose) {
    super(kinematics, gyroAngle, wheelPositions, initialPose);

    MathSharedStore.reportUsage("TripleFollowerWheelOdometry3d", "");
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
  public Pose3d update(Rotation3d gyroAngle, double x1, double x2, double y) {
    return super.update(gyroAngle, new TripleFollowerWheelPositions(x1, x2, y));
  }
}
