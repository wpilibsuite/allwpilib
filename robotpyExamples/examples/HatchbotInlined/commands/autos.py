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
    def simple_auto(drive: subsystems.drivesubsystem.DriveSubsystem):
        """A simple auto routine that drives forward a specified distance, and then stops."""
        return commands2.FunctionalCommand(
            # Reset encoders on command start
            drive.reset_encoders,
            # Drive forward while the command is executing,
            lambda: drive.arcade_drive(constants.AUTO_DRIVE_VELOCITY, 0),
            # Stop driving at the end of the command
            lambda interrupt: drive.arcade_drive(0, 0),
            # End the command when the robot's driven distance exceeds the desired value
            lambda: drive.get_average_encoder_distance()
            >= constants.AUTO_DRIVE_DISTANCE_INCHES,
            # Require the drive subsystem
        )

    @staticmethod
    def complex_auto(
        drive_subsystem: subsystems.drivesubsystem.DriveSubsystem,
        hatch_subsystem: subsystems.hatchsubsystem.HatchSubsystem,
    ):
        """A complex auto routine that drives forward, drops a hatch, and then drives backward."""
        return commands2.cmd.sequence(
            # Drive forward up to the front of the cargo ship
            commands2.FunctionalCommand(
                # Reset encoders on command start
                drive_subsystem.reset_encoders,
                # Drive forward while the command is executing
                lambda: drive_subsystem.arcade_drive(constants.AUTO_DRIVE_VELOCITY, 0),
                # Stop driving at the end of the command
                lambda interrupt: drive_subsystem.arcade_drive(0, 0),
                # End the command when the robot's driven distance exceeds the desired value
                lambda: drive_subsystem.get_average_encoder_distance()
                >= constants.AUTO_DRIVE_DISTANCE_INCHES,
                # Require the drive subsystem
                drive_subsystem,
            ),
            # Release the hatch
            hatch_subsystem.release_hatch(),
            # Drive backward the specified distance
            commands2.FunctionalCommand(
                # Reset encoders on command start
                drive_subsystem.reset_encoders,
                # Drive backwards while the command is executing
                lambda: drive_subsystem.arcade_drive(-constants.AUTO_DRIVE_VELOCITY, 0),
                # Stop driving at the end of the command
                lambda interrupt: drive_subsystem.arcade_drive(0, 0),
                # End the command when the robot's driven distance exceeds the desired value
                lambda: abs(drive_subsystem.get_average_encoder_distance())
                >= constants.AUTO_BACKUP_DISTANCE_INCHES,
                # Require the drive subsystem
                drive_subsystem,
            ),
        )
