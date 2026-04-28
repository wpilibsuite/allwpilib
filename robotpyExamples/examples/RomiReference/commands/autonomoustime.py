#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2

from commands.drivetime import DriveTime
from commands.turntime import TurnTime
from subsystems.drivetrain import Drivetrain


class AutonomousTime(commands2.SequentialCommandGroup):
    def __init__(self, drive: Drivetrain) -> None:
        """Creates a new Autonomous Drive based on time. This will drive out for a period of time, turn
        around for time (equivalent to time to turn around) and drive forward again. This should mimic
        driving out, turning around and driving back.

        :param drivetrain: The drive subsystem on which this command will run

        """
        super().__init__()

        self.addCommands(
            DriveTime(-0.6, 2.0, drive),
            TurnTime(-0.5, 1.3, drive),
            DriveTime(-0.6, 2.0, drive),
            TurnTime(0.5, 1.3, drive),
        )
