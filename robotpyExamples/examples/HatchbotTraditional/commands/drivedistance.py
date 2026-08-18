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
        self.add_requirements(drive)

    def initialize(self) -> None:
        self.drive.reset_encoders()
        self.drive.arcade_drive(self.velocity, 0)

    def execute(self) -> None:
        self.drive.arcade_drive(self.velocity, 0)

    def end(self, interrupted: bool) -> None:
        self.drive.arcade_drive(0, 0)

    def is_finished(self) -> bool:
        return abs(self.drive.get_average_encoder_distance()) >= self.distance
