package edu.wpi.first.wpilibj.examples.hatchbot.commands;

import edu.wpi.first.wpilibj.examples.hatchbot.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.examples.hatchbot.subsystems.HatchSubsystem;
import edu.wpi.first.wpilibj.experimental.command.InstantCommand;
import edu.wpi.first.wpilibj.experimental.command.SequentialCommandGroup;
import edu.wpi.first.wpilibj.experimental.command.StartEndCommand;

import static edu.wpi.first.wpilibj.examples.hatchbot.Constants.kAutoBackupDistanceInches;
import static edu.wpi.first.wpilibj.examples.hatchbot.Constants.kAutoDriveDistanceInches;

/**
 * A complex auto command that drives forward, releases a hatch, and then drives backward.
 */
public class ComplexAutoCommand extends SequentialCommandGroup {

  public ComplexAutoCommand(DriveSubsystem driveSubsystem, HatchSubsystem hatchSubsystem) {
    addCommands(
        // Drive forward up to the front of the cargo ship
        new StartEndCommand(
            // Start driving forward at the start of the command
            () -> driveSubsystem.arcadeDrive(.5, 0),
            // Stop driving at the end of the command
            () -> driveSubsystem.arcadeDrive(0, 0), driveSubsystem
        )
            // Reset the encoders before starting
            .beforeStarting(driveSubsystem::resetEncoders)
            // End the command when the robot's driven distance exceeds the desired value
            .interruptOn(() -> driveSubsystem.getAverageEncoderDistance() >= kAutoDriveDistanceInches),

        // Release the hatch
        new InstantCommand(hatchSubsystem::releaseHatch),

        // Drive backward the specified distance
        new StartEndCommand(
            () -> driveSubsystem.arcadeDrive(-.5, 0),
            () -> driveSubsystem.arcadeDrive(0, 0),
            driveSubsystem
        )
            .beforeStarting(driveSubsystem::resetEncoders)
            .interruptOn(
                () -> driveSubsystem.getAverageEncoderDistance() <= -kAutoBackupDistanceInches)
    );
  }
}
