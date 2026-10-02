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
        x_axis_velocity_supplier: typing.Callable[[], float],
        z_axis_rotate_supplier: typing.Callable[[], float],
    ) -> None:
        """Creates a new ArcadeDrive. This command will drive your robot according to the velocity supplier
        lambdas. This command does not terminate.

        :param drivetrain:  The drivetrain subsystem on which this command will run
        :param x_axis_velocity_supplier:     Callable supplier of forward/backward velocity
        :param z_axis_rotate_supplier:       Callable supplier of rotational velocity
        """

        self.drive = drive
        self.x_axis_velocity_supplier = x_axis_velocity_supplier
        self.z_axis_rotate_supplier = z_axis_rotate_supplier

        self.add_requirements(self.drive)

    def execute(self) -> None:
        self.drive.arcade_drive(
            self.x_axis_velocity_supplier(), self.z_axis_rotate_supplier()
        )
