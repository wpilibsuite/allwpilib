/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.hatchbotinlined.commands;

import edu.wpi.first.wpilibj2.command.FunctionalCommand;
import edu.wpi.first.wpilibj2.command.InstantCommand;
import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;

import edu.wpi.first.wpilibj.examples.hatchbotinlined.Constants.AutoConstants;
import edu.wpi.first.wpilibj.examples.hatchbotinlined.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.examples.hatchbotinlined.subsystems.HatchSubsystem;

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
        new FunctionalCommand(
            // Reset encoders on command start
            driveSubsystem::resetEncoders,
            // Drive forward while the command is executing
            () -> driveSubsystem.arcadeDrive(AutoConstants.kAutoDriveSpeed, 0),
            // Stop driving at the end of the command
            interrupt -> driveSubsystem.arcadeDrive(0, 0),
            // End the command when the robot's driven distance exceeds the desired value
            () -> driveSubsystem.getAverageEncoderDistance()
                >= AutoConstants.kAutoDriveDistanceInches,
            // Require the drive subsystem
            driveSubsystem
        ),

        // Release the hatch
        new InstantCommand(hatchSubsystem::releaseHatch, hatchSubsystem),

        // Drive backward the specified distance
        new FunctionalCommand(
            // Reset encoders on command start
            driveSubsystem::resetEncoders,
            // Drive backward while the command is executing
            () -> driveSubsystem.arcadeDrive(-AutoConstants.kAutoDriveSpeed, 0),
            // Stop driving at the end of the command
            interrupt -> driveSubsystem.arcadeDrive(0, 0),
            // End the command when the robot's driven distance exceeds the desired value
            () -> driveSubsystem.getAverageEncoderDistance()
                <= AutoConstants.kAutoBackupDistanceInches,
            // Require the drive subsystem
            driveSubsystem
        )
    );
  }

}
