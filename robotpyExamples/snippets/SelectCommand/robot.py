#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import override

import enum
import commands2


class MyRobot(commands2.TimedCommandRobot):
    """
    Command v2 robots are encouraged to inherit from TimedCommandRobot, which
    has an implementation of robot_periodic which runs the scheduler for you
    """

    # The enum used as keys for selecting the command to run.
    class CommandSelector(enum.Enum):
        ONE = enum.auto()
        TWO = enum.auto()
        THREE = enum.auto()

    autonomous_command: commands2.Command

    def __init__(self) -> None:
        """
        This function is run when the robot is first started up and should be used for any
        initialization code.
        """

        super().__init__()
        # An example selectcommand. Will select from the three commands based on the value returned
        # by the selector method at runtime. Note that selectcommand takes a generic type, so the
        # selector does not have to be an enum; it could be any desired type (string, integer,
        # boolean, double...)
        self.autonomous_command = commands2.SelectCommand(
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
        self.configure_button_bindings()

    def configure_button_bindings(self) -> None:
        """Use this method to define your button->command mappings. Buttons can be created by
        instantiating a {GenericHID} or one of its subclasses
        ({org.wpilib.driverstation.Joystick} or {XboxController}), and then calling passing it to a
        {org.wpilib.command2.button.JoystickButton}.
        """

    # An example selector method for the selectcommand.
    def select(self) -> CommandSelector:
        """Returns the selector that will select which command to run.
        Can base this choice on logical conditions evaluated at runtime.
        """
        return self.CommandSelector.ONE

    @override
    def disabled_start(self) -> None:
        """This function is called each time Disabled mode starts."""
        pass

    @override
    def disabled_periodic(self) -> None:
        """This function is called periodically when disabled"""
        pass

    @override
    def autonomous_start(self) -> None:
        """This autonomous runs your selected autonomous command."""
        self.autonomous_command.schedule()

    @override
    def autonomous_periodic(self) -> None:
        """This function is called periodically during autonomous"""
        pass

    @override
    def teleop_start(self) -> None:
        # This makes sure that the autonomous stops running when
        # teleop starts running. If you want the autonomous to
        # continue until interrupted by another command, remove
        # this line or comment it out.
        self.autonomous_command.cancel()

    @override
    def teleop_periodic(self) -> None:
        """This function is called periodically during teleoperated mode"""
        pass

    @override
    def utility_start(self) -> None:
        # Cancels all running commands at the start of utility mode
        commands2.CommandScheduler.get_instance().cancel_all()
