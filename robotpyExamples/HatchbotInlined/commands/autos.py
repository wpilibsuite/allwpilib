#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2
import commands2.cmd

import subsystems.drivesubsystem
import subsystems.hatchsubsystem

import constants


class Autos:
    """Container for auto command factories."""

    def __init__(self) -> None:
        raise Exception("This is a utility class!")

    @staticmethod
    def simpleAuto(drive: subsystems.drivesubsystem.DriveSubsystem):
        """A simple auto routine that drives forward a specified distance, and then stops."""
        return commands2.FunctionalCommand(
            # Reset encoders on command start
            drive.resetEncoders,
            # Drive forward while the command is executing,
            lambda: drive.arcadeDrive(constants.kAutoDriveSpeed, 0),
            # Stop driving at the end of the command
            lambda interrupt: drive.arcadeDrive(0, 0),
            # End the command when the robot's driven distance exceeds the desired value
            lambda: drive.getAverageEncoderDistance()
            >= constants.kAutoDriveDistanceInches,
            # Require the drive subsystem
        )

    @staticmethod
    def complexAuto(
        driveSubsystem: subsystems.drivesubsystem.DriveSubsystem,
        hatchSubsystem: subsystems.hatchsubsystem.HatchSubsystem,
    ):
        """A complex auto routine that drives forward, drops a hatch, and then drives backward."""
        return commands2.cmd.sequence(
            # Drive forward up to the front of the cargo ship
            commands2.FunctionalCommand(
                # Reset encoders on command start
                driveSubsystem.resetEncoders,
                # Drive forward while the command is executing
                lambda: driveSubsystem.arcadeDrive(constants.kAutoDriveSpeed, 0),
                # Stop driving at the end of the command
                lambda interrupt: driveSubsystem.arcadeDrive(0, 0),
                # End the command when the robot's driven distance exceeds the desired value
                lambda: driveSubsystem.getAverageEncoderDistance()
                >= constants.kAutoDriveDistanceInches,
                # Require the drive subsystem
                driveSubsystem,
            ),
            # Release the hatch
            hatchSubsystem.releaseHatch(),
            # Drive backward the specified distance
            commands2.FunctionalCommand(
                # Reset encoders on command start
                driveSubsystem.resetEncoders,
                # Drive backwards while the command is executing
                lambda: driveSubsystem.arcadeDrive(-constants.kAutoDriveSpeed, 0),
                # Stop driving at the end of the command
                lambda interrupt: driveSubsystem.arcadeDrive(0, 0),
                # End the command when the robot's driven distance exceeds the desired value
                lambda: abs(driveSubsystem.getAverageEncoderDistance())
                >= constants.kAutoBackupDistanceInches,
                # Require the drive subsystem
                driveSubsystem,
            ),
        )
