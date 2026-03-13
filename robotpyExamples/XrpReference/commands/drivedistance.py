#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2

from subsystems.drivetrain import Drivetrain


class DriveDistance(commands2.Command):
    def __init__(self, velocity: float, inches: float, drive: Drivetrain) -> None:
        """Creates a new DriveDistance. This command will drive your your robot for a desired distance at
        a desired velocity.

        :param velocity:  The velocity at which the robot will drive
        :param inches: The number of inches the robot will drive
        :param drive:  The drivetrain subsystem on which this command will run
        """

        self.distance = inches
        self.velocity = velocity
        self.drive = drive
        self.addRequirements(drive)

    def initialize(self) -> None:
        """Called when the command is initially scheduled."""
        self.drive.arcadeDrive(0, 0)
        self.drive.resetEncoders()

    def execute(self) -> None:
        """Called every time the scheduler runs while the command is scheduled."""
        self.drive.arcadeDrive(self.velocity, 0)

    def end(self, interrupted: bool) -> None:
        """Called once the command ends or is interrupted."""
        self.drive.arcadeDrive(0, 0)

    def isFinished(self) -> bool:
        """Returns true when the command should end."""
        # Compare distance travelled from start to desired distance
        return abs(self.drive.getAverageDistanceInch()) >= self.distance
