#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import enum
import commands2


class RobotContainer:
    class CommandSelector(enum.Enum):
        ONE = enum.auto()
        TWO = enum.auto()
        THREE = enum.auto()

    def select(self) -> "RobotContainer.CommandSelector":
        # Returns the selector that will determine which command to run.
        # Can base this choice on logical conditions evaluated at runtime.
        return self.CommandSelector.ONE

    def __init__(self) -> None:
        # An example SelectCommand. Will select from the three commands based on
        # the value returned by the selector method at runtime.
        self.exampleSelectCommand = commands2.SelectCommand(
            {
                self.CommandSelector.ONE: commands2.PrintCommand(
                    "Command one was selected!"
                ),
                self.CommandSelector.TWO: commands2.PrintCommand(
                    "Command two was selected!"
                ),
                self.CommandSelector.THREE: commands2.PrintCommand(
                    "Command three was selected!"
                ),
            },
            self.select,
        )

    def getAutonomousCommand(self) -> commands2.Command:
        return self.exampleSelectCommand
