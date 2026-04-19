#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2

from commands.drivedistance import DriveDistance
from commands.turndegrees import TurnDegrees
from subsystems.drivetrain import Drivetrain


class AutonomousDistance(commands2.SequentialCommandGroup):
    def __init__(self, drive: Drivetrain) -> None:
        """Creates a new Autonomous Drive based on distance. This will drive out for a specified distance,
        turn around and drive back.

        :param drivetrain: The drivetrain subsystem on which this command will run
        """
        super().__init__()

        self.addCommands(
            DriveDistance(-0.5, 10, drive),
            TurnDegrees(-0.5, 180, drive),
            DriveDistance(-0.5, 10, drive),
            TurnDegrees(0.5, 180, drive),
        )
