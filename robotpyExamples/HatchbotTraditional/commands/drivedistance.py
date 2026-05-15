#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2

from subsystems.drivesubsystem import DriveSubsystem


class DriveDistance(commands2.Command):
    def __init__(self, inches: float, velocity: float, drive: DriveSubsystem) -> None:
        self.distance = inches
        self.velocity = velocity
        self.drive = drive
        self.addRequirements(drive)

    def initialize(self) -> None:
        self.drive.resetEncoders()
        self.drive.arcadeDrive(self.velocity, 0)

    def execute(self) -> None:
        self.drive.arcadeDrive(self.velocity, 0)

    def end(self, interrupted: bool) -> None:
        self.drive.arcadeDrive(0, 0)

    def isFinished(self) -> bool:
        return abs(self.drive.getAverageEncoderDistance()) >= self.distance
