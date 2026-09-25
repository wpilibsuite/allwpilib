#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
from wpilib import PeriodicOpMode
from opmode import robot

class MyTeleop(PeriodicOpMode):
    def __init__(self, robot) -> None:
        super().__init__()
        self.robot = robot

    def disabled_periodic(self):
       # Called periodically while the robot is disabled

    def start(self):
      # Called once when the robot is enabled. 

    def periodic(self):
       # Called periodically (set time interval) while the robot is enabled.

    def end(self):
       # Called when the robot is disabled (after previously being enabled).

    def close(self):
       # Called when the opmode is de-selected / no additional methods will be called.