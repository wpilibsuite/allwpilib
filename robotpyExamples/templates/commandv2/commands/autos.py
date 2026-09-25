# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import wpilib
import commands2.command
from subsystems.examplesubsystem import ExampleSubsystem
from commands.examplecommand import ExampleCommand

class Autos:
    def example_auto(subsystem: ExampleSubsystem) -> commands2.Command:
        return commands2.SequentialCommandGroup(ExampleSubsystem.example_method_command(subsystem), ExampleCommand(subsystem))
