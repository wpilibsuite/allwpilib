#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
from wpilib import PeriodicOpMode
from opmode.robot import Robot

class MyAuto(PeriodicOpMode):
    # The robot instance is passed into the opmode via the constructor.
    def __init__(self, robot) -> None:
        super().__init__()
        self.robot = robot
    # This method runs periodically, using the same period as the Robot instance.
    # Additional periodic methods may be configured with addPeriodic(),
    # which can have periods that differ from the main Robot instance.

    
    # This method runs periodically, using the same period as the Robot instance.
    # Additional periodic methods may be configured with addPeriodic(),
    # which can have periods that differ from the main Robot instance.
    
    def periodic(self):
        # Put custom auto code here