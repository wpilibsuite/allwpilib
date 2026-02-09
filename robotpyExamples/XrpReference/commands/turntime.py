#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2
import wpilib

from subsystems.drivetrain import Drivetrain


class TurnTime(commands2.Command):
    """Creates a new TurnTime command. This command will turn your robot for a
    desired rotational speed and time.
    """

    def __init__(self, speed: float, time: float, drive: Drivetrain) -> None:
        """Creates a new TurnTime.

        :param speed: The speed which the robot will turn. Negative is in reverse.
        :param time:  How much time to turn in seconds
        :param drive: The drive subsystem on which this command will run
        """

        self.rotationalSpeed = speed
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
        self.drive.arcadeDrive(0, self.rotationalSpeed)

    def end(self, interrupted: bool) -> None:
        """Called once the command ends or is interrupted."""
        self.drive.arcadeDrive(0, 0)

    def isFinished(self) -> bool:
        """Returns true when the command should end"""
        return wpilib.Timer.getFPGATimestamp() - self.startTime >= self.duration
