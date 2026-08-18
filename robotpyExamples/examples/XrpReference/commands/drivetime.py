#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2
import wpilib

from subsystems.drivetrain import Drivetrain


class DriveTime(commands2.Command):
    """Creates a new DriveTime. This command will drive your robot for a desired velocity and time."""

    def __init__(self, velocity: float, time: float, drive: Drivetrain) -> None:
        """Creates a new DriveTime. This command will drive your robot for a desired velocity and time.

        :param velocity: The velocity which the robot will drive. Negative is in reverse.
        :param time:  How much time to drive in seconds
        :param drive: The drivetrain subsystem on which this command will run
        """

        self.velocity = velocity
        self.duration = time
        self.drive = drive
        self.add_requirements(drive)

        self.start_time = 0.0

    def initialize(self) -> None:
        """Called when the command is initially scheduled."""
        self.start_time = wpilib.Timer.get_timestamp()
        self.drive.arcade_drive(0, 0)

    def execute(self) -> None:
        """Called every time the scheduler runs while the command is scheduled."""
        self.drive.arcade_drive(self.velocity, 0)

    def end(self, interrupted: bool) -> None:
        """Called once the command ends or is interrupted."""
        self.drive.arcade_drive(0, 0)

    def is_finished(self) -> bool:
        """Returns true when the command should end"""
        return wpilib.Timer.get_timestamp() - self.start_time >= self.duration
