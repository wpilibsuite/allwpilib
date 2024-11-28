// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Translation3d;

/**
 * Class for swerve drive odometry. Odometry allows you to track the robot's position on the field
 * over a course of a match using readings from your swerve drive encoders and swerve azimuth
 * encoders.
 *
 * <p>This class is meant to be an easy replacement for {@link SwerveDriveOdometry}, only requiring
 * the addition of appropriate conversions between 2D and 3D versions of geometry classes. (See
 * {@link Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class SwerveDriveOdometry3d extends Odometry3d<SwerveModulePosition[]> {
  private final int m_numModules;

  /**
   * Constructs a SwerveDriveOdometry3d object.
   *
   * @param kinematics The swerve drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The wheel positions reported by each module.
   * @param initialPose The starting position of the robot on the field.
   */
  public SwerveDriveOdometry3d(
      SwerveDriveKinematics kinematics,
      Rotation3d gyroAngle,
      SwerveModulePosition[] modulePositions,
      Pose3d initialPose) {
    super(kinematics, gyroAngle, modulePositions, initialPose);

    m_numModules = modulePositions.length;

    MathSharedStore.reportUsage(MathUsageId.kOdometry_SwerveDrive, 1);
  }

  /**
   * Constructs a SwerveDriveOdometry3d object with the default pose at the origin.
   *
   * @param kinematics The swerve drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The wheel positions reported by each module.
   */
  public SwerveDriveOdometry3d(
      SwerveDriveKinematics kinematics,
      Rotation3d gyroAngle,
      SwerveModulePosition[] modulePositions) {
    this(kinematics, gyroAngle, modulePositions, Pose3d.kZero);
  }

  @Override
  public void resetPosition(
      Rotation3d gyroAngle, SwerveModulePosition[] modulePositions, Pose3d pose) {
    if (modulePositions.length != m_numModules) {
      throw new IllegalArgumentException(
          "Number of modules is not consistent with number of wheel locations provided in "
              + "constructor");
    }
    super.resetPosition(gyroAngle, modulePositions, pose);
  }

  @Override
  public Pose3d update(Rotation3d gyroAngle, SwerveModulePosition[] modulePositions) {
    if (modulePositions.length != m_numModules) {
      throw new IllegalArgumentException(
          "Number of modules is not consistent with number of wheel locations provided in "
              + "constructor");
    }
    return super.update(gyroAngle, modulePositions);
  }
}
