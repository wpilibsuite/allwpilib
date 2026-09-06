#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import commands2

class RobotContainer:
    def __init__(self) -> None:
        super().__init__()
        self.configure_bindings()

    def configure_bindings(self) -> None:

    def get_autonomous_command(self) -> commands2.Command:
        return commands2.PrintCommand("No autonomous command configured")