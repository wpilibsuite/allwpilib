#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import Command
from wpilib import Gamepad
from commands.examplecommand import ExampleCommand
from subsystems.xrpdrivetrain import XRPDrivetrain

# This class is where the bulk of the robot should be declared. Since Command-based is a
# "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
# periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
# subsystems, commands, and button mappings) should be declared here.

class RobotContainer:
    # The robot's subsystems and commands are defined here
    def __init__(self) -> None:
        self.drivetrain = XRPDrivetrain()
        self.auto_command = ExampleCommand(self.drivetrain)

        # Configure the button bindings
        self.configure_button_bindings()

    # Use this function to define your button->command mappings. 
    def configure_button_bindings(self) -> None:

    # Use this to pass the autonomous command to the main Robot class
    # Returns the command torun in autonomous
    def get_autonomous_command(self) -> Command:
        # An ExampleCommand will run in autonomous
        return self.auto_command
