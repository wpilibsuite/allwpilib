// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3;

import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rebuiltcmdv3.constants.DriveConstants;
import org.wpilib.math.estimator.SwerveDrivePoseEstimator;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rectangle2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.SwerveModulePosition;

@Logged
public class PoseEstimator {
  private final SwerveDrivePoseEstimator poseEstimator =
      new SwerveDrivePoseEstimator(
          DriveConstants.KINEMATICS,
          Rotation2d.kZero,
          new SwerveModulePosition[] {
            new SwerveModulePosition(),
            new SwerveModulePosition(),
            new SwerveModulePosition(),
            new SwerveModulePosition()
          },
          Pose2d.kZero);

  /**
   * Updates the pose estimator with the current gyro heading and swerve module positions.
   *
   * @param gyroHeading The current gyro heading
   * @param modulePositions The current module positions
   */
  public void odometryUpdate(Rotation2d gyroHeading, SwerveModulePosition... modulePositions) {
    poseEstimator.update(gyroHeading, modulePositions);
  }

  /**
   * Updates the pose estimator with a vision measurement.
   *
   * @param estimatedPose The estimated pose from vision
   * @param timestamp The timestamp of the vision measurement
   */
  public void visionUpdate(Pose2d estimatedPose, double timestamp) {
    poseEstimator.addVisionMeasurement(estimatedPose, timestamp);
  }

  /**
   * Returns the current estimated pose of the robot.
   *
   * @return The current estimated pose
   */
  public Pose2d getEstimatedPose() {
    return poseEstimator.getEstimatedPosition();
  }

  /**
   * Checks if the current estimated pose is within a given rectangular zone.
   *
   * @param bounds The rectangular zone to check against
   * @return True if the pose is within the zone, false otherwise
   */
  public boolean inZone(Rectangle2d bounds) {
    return bounds.contains(poseEstimator.getEstimatedPosition().getTranslation());
  }
}
