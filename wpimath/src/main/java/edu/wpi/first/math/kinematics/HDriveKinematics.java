// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.math.geometry.Twist2d;

public class HDriveKinematics {
    public final double trackWidthMeters;

    public HDriveKinematics(double trackWidthMeters) {
        this.trackWidthMeters = trackWidthMeters;
        MathSharedStore.reportUsage(MathUsageId.kKinematics_HDrive, 1);
    }

    public ChassisSpeeds toChassisSpeeds(HDriveWheelSpeeds wheelSpeeds){
        return new ChassisSpeeds(
        (wheelSpeeds.leftMetersPerSecond + wheelSpeeds.rightMetersPerSecond) / 2,
        wheelSpeeds.lateralMetersPerSecond,
        (wheelSpeeds.rightMetersPerSecond - wheelSpeeds.leftMetersPerSecond) / trackWidthMeters);
    }

    public HDriveWheelSpeeds toWheelSpeeds(ChassisSpeeds chassisSpeeds){
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
   * @return The resulting Twist2d.
   */
  public Twist2d toTwist2d(double leftDistanceMeters, double rightDistanceMeters, double lateralDistanceMeters, double trackWidthMeters) {
    return new Twist2d(
        (leftDistanceMeters + rightDistanceMeters) / 2,
        lateralDistanceMeters,
        (rightDistanceMeters - leftDistanceMeters) / trackWidthMeters);
  }
}