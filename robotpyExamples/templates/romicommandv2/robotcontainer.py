#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import Command
from wpilib import Gamepad
from commands.examplecommand import ExampleCommand
from subsystems.romidrivetrain import RomiDrivetrain

# This class is where the bulk of the robot should be declared. Since Command-based is a
# "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
# periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
# subsystems, commands, and button mappings) should be declared here.

class RobotContainer:
    def __init__(self) -> None:
        # The robot's subsystems and commands are defined here
        self.romi_drivetrain = RomiDrivetrain()
        self.auto_command = ExampleCommand(self.romi_drivetrain)

        # Configure the button bindings
        self.configure_button_bindings()
        
    # An ExampleCommand will run in autonomous
    def getAutonomousCommand(self) -> Command:
        return self.auto_command
    
    # Use this method to define your button->command mappings. 
    def configure_button_bindings(self):