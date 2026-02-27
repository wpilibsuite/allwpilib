#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2
import wpilib

from subsystems.drivetrain import Drivetrain


class DriveTime(commands2.Command):
    """Creates a new DriveTime. This command will drive your robot for a desired speed and time."""

    def __init__(self, speed: float, time: float, drive: Drivetrain) -> None:
        """Creates a new DriveTime. This command will drive your robot for a desired speed and time.

        :param speed: The speed which the robot will drive. Negative is in reverse.
        :param time:  How much time to drive in seconds
        :param drive: The drivetrain subsystem on which this command will run
        """

        self.speed = speed
        self.duration = time
        self.drive = drive
        self.addRequirements(drive)

        self.startTime = 0.0

    def initialize(self) -> None:
        """Called when the command is initially scheduled."""
        self.startTime = wpilib.Timer.getFPGATimestamp()
        self.drive.arcadeDrive(0, 0)

    def execute(self) -> None:
        """Called every time the scheduler runs while the command is scheduled."""
        self.drive.arcadeDrive(self.speed, 0)

    def end(self, interrupted: bool) -> None:
        """Called once the command ends or is interrupted."""
        self.drive.arcadeDrive(0, 0)

    def isFinished(self) -> bool:
        """Returns true when the command should end"""
        return wpilib.Timer.getFPGATimestamp() - self.startTime >= self.duration
