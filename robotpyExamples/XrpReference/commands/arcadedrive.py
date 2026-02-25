#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import typing
import commands2
from subsystems.drivetrain import Drivetrain


class ArcadeDrive(commands2.Command):
    def __init__(
        self,
        drive: Drivetrain,
        xaxisSpeedSupplier: typing.Callable[[], float],
        zaxisRotateSupplier: typing.Callable[[], float],
    ) -> None:
        """Creates a new ArcadeDrive. This command will drive your robot according to the speed supplier
        lambdas. This command does not terminate.

        :param drivetrain:  The drivetrain subsystem on which this command will run
        :param xaxisSpeedSupplier:     Callable supplier of forward/backward speed
        :param zaxisRotateSupplier:    Callable supplier of rotational speed
        """

        self.drive = drive
        self.xaxisSpeedSupplier = xaxisSpeedSupplier
        self.zaxisRotateSupplier = zaxisRotateSupplier

        self.addRequirements(self.drive)

    def execute(self) -> None:
        self.drive.arcadeDrive(self.xaxisSpeedSupplier(), self.zaxisRotateSupplier())
