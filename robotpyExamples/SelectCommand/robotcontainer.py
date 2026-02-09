#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import enum
import commands2


class RobotContainer:
    """This class is where the bulk of the robot should be declared. Since Command-based is a
    "declarative" paradigm, very little robot logic should actually be handled in the :class:`.Robot`
    periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
    subsystems, commands, and button mappings) should be declared here.
    """

    # The enum used as keys for selecting the command to run.
    class CommandSelector(enum.Enum):
        ONE = enum.auto()
        TWO = enum.auto()
        THREE = enum.auto()

    # An example selector method for the selectcommand.
    def select(self) -> CommandSelector:
        """Returns the selector that will select which command to run.
        Can base this choice on logical conditions evaluated at runtime.
        """
        return self.CommandSelector.ONE

    def __init__(self) -> None:
        # An example selectcommand. Will select from the three commands based on the value returned
        # by the selector method at runtime. Note that selectcommand takes a generic type, so the
        # selector does not have to be an enum; it could be any desired type (string, integer,
        # boolean, double...)
        self.example_select_command = commands2.SelectCommand(
            # Maps selector values to commands
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

        # Configure the button bindings
        self.configureButtonBindings()

    def configureButtonBindings(self) -> None:
        """Use this method to define your button->command mappings. Buttons can be created by
        instantiating a {GenericHID} or one of its subclasses
        ({edu.wpi.first.wpilibj.Joystick} or {XboxController}), and then calling passing it to a
        {edu.wpi.first.wpilibj2.command.button.JoystickButton}.
        """

    def getAutonomousCommand(self) -> commands2.Command:
        """Use this to pass the autonomous command to the main {Robot} class.

        :returns: the command to run in autonomous
        """
        return self.example_select_command
