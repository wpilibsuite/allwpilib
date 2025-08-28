// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.MathSharedStore;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.Translation3d;

/**
 * Class for mecanum drive odometry. Odometry allows you to track the robot's position on the field
 * over a course of a match using readings from your mecanum wheel encoders.
 *
 * <p>This class is meant to be an easy replacement for {@link MecanumDriveOdometry}, only requiring
 * the addition of appropriate conversions between 2D and 3D versions of geometry classes. (See
 * {@link Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class MecanumDriveOdometry3d extends Odometry3d<MecanumDriveWheelPositions> {
  /**
   * Constructs a MecanumDriveOdometry3d object.
   *
   * @param kinematics The mecanum drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   * @param initialPose The starting position of the robot on the field.
   */
  public MecanumDriveOdometry3d(
      MecanumDriveKinematics kinematics,
      Rotation3d gyroAngle,
      MecanumDriveWheelPositions wheelPositions,
      Pose3d initialPose) {
    super(kinematics, gyroAngle, wheelPositions, initialPose);
    MathSharedStore.reportUsage("MecanumDriveOdometry3d", "");
  }

  /**
   * Constructs a MecanumDriveOdometry3d object with the default pose at the origin.
   *
   * @param kinematics The mecanum drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   */
  public MecanumDriveOdometry3d(
      MecanumDriveKinematics kinematics,
      Rotation3d gyroAngle,
      MecanumDriveWheelPositions wheelPositions) {
    this(kinematics, gyroAngle, wheelPositions, Pose3d.kZero);
  }
}
