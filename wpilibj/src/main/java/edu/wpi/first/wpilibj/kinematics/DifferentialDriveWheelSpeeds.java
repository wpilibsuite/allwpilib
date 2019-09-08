/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.kinematics;

/**
 * Represents the wheel speeds for a differential drive drivetrain.
 */
@SuppressWarnings("MemberName")
public class DifferentialDriveWheelSpeeds {
  /**
   * Speed of the left side of the robot.
   */
  public double leftMetersPerSecond;

  /**
   * Speed of the right side of the robot.
   */
  public double rightMetersPerSecond;

  /**
   * Constructs a DifferentialDriveWheelSpeeds with zeros for left and right speeds.
   */
  public DifferentialDriveWheelSpeeds() {
  }

  /**
   * Constructs a DifferentialDriveWheelSpeeds.
   *
   * @param leftMetersPerSecond  The left speed.
   * @param rightMetersPerSecond The right speed.
   */
  public DifferentialDriveWheelSpeeds(double leftMetersPerSecond, double rightMetersPerSecond) {
    this.leftMetersPerSecond = leftMetersPerSecond;
    this.rightMetersPerSecond = rightMetersPerSecond;
  }

  /**
   * Normalizes the wheel speeds using some max attainable speed. Sometimes,
   * after inverse kinematics, the requested speed from a/several modules may be
   * above the max attainable speed for the driving motor on that module. To fix
   * this issue, one can "normalize" all the wheel speeds to make sure that all
   * requested module speeds are below the absolute threshold, while maintaining
   * the ratio of speeds between modules.
   *
   * @param attainableMaxSpeedMetersPerSecond The absolute max speed that a wheel can reach.
   */
  public void normalize(double attainableMaxSpeedMetersPerSecond) {
    double realMaxSpeed = Math.max(Math.abs(leftMetersPerSecond), Math.abs(rightMetersPerSecond));

    if (realMaxSpeed > attainableMaxSpeedMetersPerSecond) {
      leftMetersPerSecond = leftMetersPerSecond / realMaxSpeed
          * attainableMaxSpeedMetersPerSecond;
      rightMetersPerSecond = rightMetersPerSecond / realMaxSpeed
          * attainableMaxSpeedMetersPerSecond;
    }
  }
}
