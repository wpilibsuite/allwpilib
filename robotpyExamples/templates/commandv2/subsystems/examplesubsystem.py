# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import commands2
from commands2.cmd import runOnce
import wpilib
from commands2 import subsystem

class ExampleSubsystem(subsystem.Subsystem):
    # Creates a new ExampleSubsystem
    def __init__(self) -> None:
        super().__init__()

    def example_method_command(self) -> commands2.Command:

        return runOnce(lambda: #one-time action goes here
        )

    # An example method with a boolean state of the subsystem (ex. digital sensor)
    def example_condition(self) -> bool:
        return False

    # This method will be called once per scheduler run
    def periodic(self):

    # This method will be called once per scheduler run during simulation
    def simulation_periodic(self):
