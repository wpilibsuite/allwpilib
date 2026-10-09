// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.coordinatesystem;

import org.wpilib.drive.MecanumDrive;
import org.wpilib.driverstation.Alliance;
import org.wpilib.driverstation.MatchState;
import org.wpilib.framework.TimedRobot;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisVelocities;

/**
 * Coordinate system changing using Alliance Color snippets for wpilib-docs.
 * https://docs.wpilib.org/en/stable/docs/software/basic-programming/coordinate-system.html
 */
public class Robot extends TimedRobot {
  /**
   * Applies driver input to mecanum drive using a field coordinate system that changes based on
   * alliance color.
   */
  public static void mecanumDrive(
      MecanumDrive robotDrive,
      double xSpeed,
      double ySpeed,
      double zRotation,
      Rotation2d gyroAngle) {
    // The origin is always blue. When our alliance is red, X and Y need to be inverted
    var alliance = MatchState.getAlliance();
    var invert = 1;
    if (alliance.isPresent() && alliance.get() == Alliance.RED) {
      invert = -1;
    }
    // Drive using the X, Y, and Z axes of the joystick.
    robotDrive.driveCartesian(xSpeed * invert, ySpeed * invert, zRotation, gyroAngle);
  }

  /**
   * Applies driver input to swerve using a field coordinate system that changes based on alliance
   * color.
   */
  public static ChassisVelocities driveAllianceRelative(
      double xSpeed, double ySpeed, double zRotation, Rotation2d gyroAngle) {
    // The origin is always blue. When our alliance is red, X and Y need to be inverted
    var alliance = MatchState.getAlliance();
    var invert = 1;
    if (alliance.isPresent() && alliance.get() == Alliance.RED) {
      invert = -1;
    }

    // Create field-relative ChassisVelocities for controlling Swerve
    var chassisVelocities =
        new ChassisVelocities(xSpeed * invert, ySpeed * invert, zRotation)
            .toRobotRelative(gyroAngle);
    return chassisVelocities;
  }
}
