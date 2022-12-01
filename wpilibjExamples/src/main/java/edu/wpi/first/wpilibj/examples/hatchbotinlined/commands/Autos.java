// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.hatchbotinlined.commands;

import edu.wpi.first.wpilibj.examples.hatchbotinlined.Constants.AutoConstants;
import edu.wpi.first.wpilibj.examples.hatchbotinlined.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.examples.hatchbotinlined.subsystems.HatchSubsystem;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.FunctionalCommand;

/** Container for auto command factories. */
public final class Autos {
  /** A simple auto routine that drives forward a specified distance, and then stops. */
  public static Command simpleAuto(DriveSubsystem drive) {
    return new FunctionalCommand(
        // Reset encoders on command start
        drive::resetEncoders,
        // Drive forward while the command is executing
        () -> drive.arcadeDrive(AutoConstants.kAutoDriveSpeed, 0),
        // Stop driving at the end of the command
        interrupt -> drive.arcadeDrive(0, 0),
        // End the command when the robot's driven distance exceeds the desired value
        () -> drive.getAverageEncoderDistance() >= AutoConstants.kAutoDriveDistanceInches,
        // Require the drive subsystem
        drive);
  }

  /** A complex auto routine that drives forward, drops a hatch, and then drives backward. */
  public static Command complexAuto(DriveSubsystem driveSubsystem, HatchSubsystem hatchSubsystem) {
    return Commands.sequence(
        // Drive forward up to the front of the cargo ship
        new FunctionalCommand(
            // Reset encoders on command start
            driveSubsystem::resetEncoders,
            // Drive forward while the command is executing
            () -> driveSubsystem.arcadeDrive(AutoConstants.kAutoDriveSpeed, 0),
            // Stop driving at the end of the command
            interrupt -> driveSubsystem.arcadeDrive(0, 0),
            // End the command when the robot's driven distance exceeds the desired value
            () ->
                driveSubsystem.getAverageEncoderDistance()
                    >= AutoConstants.kAutoDriveDistanceInches,
            // Require the drive subsystem
            driveSubsystem),

        // Release the hatch
        Commands.runOnce(hatchSubsystem::releaseHatch, hatchSubsystem),

        // Drive backward the specified distance
        new FunctionalCommand(
            // Reset encoders on command start
            driveSubsystem::resetEncoders,
            // Drive backward while the command is executing
            () -> driveSubsystem.arcadeDrive(-AutoConstants.kAutoDriveSpeed, 0),
            // Stop driving at the end of the command
            interrupt -> driveSubsystem.arcadeDrive(0, 0),
            // End the command when the robot's driven distance exceeds the desired value
            () ->
                driveSubsystem.getAverageEncoderDistance()
                    <= AutoConstants.kAutoBackupDistanceInches,
            // Require the drive subsystem
            driveSubsystem));
  }

  private Autos() {
    throw new UnsupportedOperationException("This is a utility class!");
  }
}
