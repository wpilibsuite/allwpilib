// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.math.geometry.Twist2d;

/**
 * Helper class that converts a chassis velocity (dx, dy, and dtheta components) into individual
 * wheel speeds.
 */
public class HDriveKinematics {
  public final double trackWidthMeters;

  /** Constructor for the kinematics of an HDrive. 
   * @param trackWidthMeters The width of the wheel base in meters for the differential drive/
  */
  public HDriveKinematics(double trackWidthMeters) {
    this.trackWidthMeters = trackWidthMeters;
    MathSharedStore.reportUsage(MathUsageId.kKinematics_HDrive, 1);
  }

  /**
   * Takes in wheel speeds and returns the system chasis speeds.
   *
   * @param wheelSpeeds The speeds of the left right and lateral wheels.
   * @return The chasis speeds of the robot.
   */
  public ChassisSpeeds toChassisSpeeds(HDriveWheelSpeeds wheelSpeeds) {
    return new ChassisSpeeds(
        (wheelSpeeds.leftMetersPerSecond + wheelSpeeds.rightMetersPerSecond) / 2,
        wheelSpeeds.lateralMetersPerSecond,
        (wheelSpeeds.rightMetersPerSecond - wheelSpeeds.leftMetersPerSecond) / trackWidthMeters);
  }

  /**
   * Converts chasis speeds to the individual wheel speeds of the drive.
   *
   * @param chassisSpeeds The speed of the robot vx, vy, and omega.
   * @return The individual wheel speeds of the robot.
   */
  public HDriveWheelSpeeds toWheelSpeeds(ChassisSpeeds chassisSpeeds) {
    return new HDriveWheelSpeeds(
        chassisSpeeds.vxMetersPerSecond
            - trackWidthMeters / 2 * chassisSpeeds.omegaRadiansPerSecond,
        chassisSpeeds.vxMetersPerSecond
            + trackWidthMeters / 2 * chassisSpeeds.omegaRadiansPerSecond,
        chassisSpeeds.vyMetersPerSecond);
  }

  /**
   * Performs forward kinematics to return the resulting Twist2d from the given left and right side
   * distance deltas. This method is often used for odometry -- determining the robot's position on
   * the field using changes in the distance driven by each wheel on the robot.
   *
   * @param leftDistanceMeters The distance measured by the left side encoder.
   * @param rightDistanceMeters The distance measured by the right side encoder.
   * @param lateralDistanceMeters The distance measured by the lateral wheels encoder.
   * @param trackWidthMeters The width of the wheel base in meters for the differential drive.
   * @return The resulting Twist2d.
   */
  public Twist2d toTwist2d(
      double leftDistanceMeters,
      double rightDistanceMeters,
      double lateralDistanceMeters,
      double trackWidthMeters) {
    return new Twist2d(
        (leftDistanceMeters + rightDistanceMeters) / 2,
        lateralDistanceMeters,
        (rightDistanceMeters - leftDistanceMeters) / trackWidthMeters);
  }
}
