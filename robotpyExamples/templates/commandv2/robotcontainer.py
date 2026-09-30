# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import wpilib
import commands2
from commands2.button import Trigger, CommandNiDsXboxController
import constants
from subsystems.examplesubsystem import ExampleSubsystem
from commands.autos import Autos
from commands.examplecommand import ExampleCommand

class RobotContainer:

    def __init__(self) -> None:
        self.example_subsystem = ExampleSubsystem()

        self.driver_controller = CommandNiDsXboxController(constants.K_DRIVER_CONTROLLER_PORT)
        self.configure_button_bindings()

    def configure_button_bindings(self):
        Trigger(self.example_subsystem.example_condition).onTrue(ExampleCommand(self.example_subsystem))
        self.driver_controller.y().whileTrue(self.example_subsystem.example_method_command())

    def get_autonomous_command(self):
        # An example command will be run in autonomous
        return Autos.example_auto(self.example_subsystem)
