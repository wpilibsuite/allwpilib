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
    desired rotational velocity and time.
    """

    def __init__(self, velocity: float, time: float, drive: Drivetrain) -> None:
        """Creates a new TurnTime.

        :param velocity: The velocity which the robot will turn. Negative is in reverse.
        :param time:  How much time to turn in seconds
        :param drive: The drive subsystem on which this command will run
        """

        self.rotational_velocity = velocity
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
        self.drive.arcade_drive(0, self.rotational_velocity)

    def end(self, interrupted: bool) -> None:
        """Called once the command ends or is interrupted."""
        self.drive.arcade_drive(0, 0)

    def is_finished(self) -> bool:
        """Returns true when the command should end"""
        return wpilib.Timer.get_timestamp() - self.start_time >= self.duration
