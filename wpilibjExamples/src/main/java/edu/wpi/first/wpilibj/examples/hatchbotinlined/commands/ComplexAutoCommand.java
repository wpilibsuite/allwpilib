/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.hatchbotinlined.commands;

import edu.wpi.first.wpilibj2.command.InstantCommand;
import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;
import edu.wpi.first.wpilibj2.command.StartEndCommand;

import edu.wpi.first.wpilibj.examples.hatchbotinlined.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.examples.hatchbotinlined.subsystems.HatchSubsystem;

import static edu.wpi.first.wpilibj.examples.hatchbotinlined.Constants.AutoConstants.kAutoBackupDistanceInches;
import static edu.wpi.first.wpilibj.examples.hatchbotinlined.Constants.AutoConstants.kAutoDriveDistanceInches;
import static edu.wpi.first.wpilibj.examples.hatchbotinlined.Constants.AutoConstants.kAutoDriveSpeed;

/**
 * A complex auto command that drives forward, releases a hatch, and then drives backward.
 */
public class ComplexAutoCommand extends SequentialCommandGroup {
  /**
   * Creates a new ComplexAutoCommand.
   *
   * @param driveSubsystem The drive subsystem this command will run on
   * @param hatchSubsystem The hatch subsystem this command will run on
   */
  public ComplexAutoCommand(DriveSubsystem driveSubsystem, HatchSubsystem hatchSubsystem) {
    addCommands(
        // Drive forward up to the front of the cargo ship
        new StartEndCommand(
            // Start driving forward at the start of the command
            () -> driveSubsystem.arcadeDrive(kAutoDriveSpeed, 0),
            // Stop driving at the end of the command
            () -> driveSubsystem.arcadeDrive(0, 0), driveSubsystem
        )
            // Reset the encoders before starting
            .beforeStarting(driveSubsystem::resetEncoders)
            // End the command when the robot's driven distance exceeds the desired value
            .withInterrupt(
                () -> driveSubsystem.getAverageEncoderDistance() >= kAutoDriveDistanceInches),

        // Release the hatch
        new InstantCommand(hatchSubsystem::releaseHatch, hatchSubsystem),

        // Drive backward the specified distance
        new StartEndCommand(
            () -> driveSubsystem.arcadeDrive(-kAutoDriveSpeed, 0),
            () -> driveSubsystem.arcadeDrive(0, 0),
            driveSubsystem
        )
            .beforeStarting(driveSubsystem::resetEncoders)
            .withInterrupt(
                () -> driveSubsystem.getAverageEncoderDistance() <= -kAutoBackupDistanceInches)
    );
  }

}
