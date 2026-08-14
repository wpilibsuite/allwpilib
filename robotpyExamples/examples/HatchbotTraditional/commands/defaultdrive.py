#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import typing
import commands2
from subsystems.drivesubsystem import DriveSubsystem


class DefaultDrive(commands2.Command):
    def __init__(
        self,
        drive: DriveSubsystem,
        forward: typing.Callable[[], float],
        rotation: typing.Callable[[], float],
    ) -> None:

        self.drive = drive
        self.forward = forward
        self.rotation = rotation

        self.add_requirements(self.drive)

    def execute(self) -> None:
        self.drive.arcade_drive(self.forward(), self.rotation())
