#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

#
# Example that shows how to connect to a XRP from RobotPy
#
# Requirements
# ------------
#
# You must have the robotpy-xrp package installed. This is best done via:
#
#    # Windows
#    py -3 -m pip install robotpy[commands2,sim,xrp]
#
#    # Linux/macOS
#    pip3 install robotpy[commands2,sim,xrp]
#
# Run the program
# ---------------
#
# Use the dedicated XRP command:
#
#    # Windows
#    py -3 -m robotpy run-xrp
#
#    # Linux/macOS
#    python -m robotpy run-xrp
#
# If your XRP isn't at the default address, use --host/--port:
#
#    python -m robotpy run-xrp --host 192.168.42.1 --port 3540
#
# By default the WPILib simulation GUI will be displayed. To disable the display
# you can add the --nogui option.
#

from typing import override

import commands2
import commands2.button
import tunables
import wpilib
import xrp

from commands.arcadedrive import ArcadeDrive
from commands.autonomousdistance import AutonomousDistance
from commands.autonomoustime import AutonomousTime

from subsystems.arm import Arm
from subsystems.drivetrain import Drivetrain


class MyRobot(commands2.TimedCommandRobot):
    """
    Command v2 robots are encouraged to inherit from TimedCommandRobot, which
    has an implementation of robot_periodic which runs the scheduler for you
    """

    autonomous_command: commands2.Command | None = None

    def __init__(self) -> None:
        """
        This function is run when the robot is first started up and should be used for any
        initialization code.
        """
        super().__init__()

        self.drivetrain = Drivetrain()
        self.onboard_io = xrp.XRPOnBoardIO()
        self.arm = Arm()

        # Assumes a gamepad plugged into channnel 0
        self.controller = wpilib.Joystick(0)

        # Create a tunable selector for autonomous routines.
        self.chooser = tunables.Selectable()

        self._configure_button_bindings()

    def _configure_button_bindings(self):
        """Use this method to define your button->command mappings. Buttons can be created by
        instantiating a :class:`.GenericHID` or one of its subclasses (:class`.Joystick or
        :class:`.XboxController`), and then passing it to a :class:`.JoystickButton`.
        """

        # Default command is arcade drive. This will run unless another command
        # is scheduled over it
        self.drivetrain.set_default_command(
            ArcadeDrive(
                self.drivetrain,
                lambda: -self.controller.get_raw_axis(1),
                lambda: -self.controller.get_raw_axis(2),
            )
        )

        # Example of how to use the onboard IO
        onboard_button_a = commands2.button.Trigger(
            self.onboard_io.get_user_button_pressed
        )
        onboard_button_a.on_true(
            commands2.PrintCommand("USER Button Pressed")
        ).on_false(commands2.PrintCommand("USER Button Released"))

        joystick_a_button = commands2.button.JoystickButton(self.controller, 1)
        joystick_a_button.on_true(
            commands2.InstantCommand(lambda: self.arm.set_angle(45.0), self.arm)
        )
        joystick_a_button.on_false(
            commands2.InstantCommand(lambda: self.arm.set_angle(0.0), self.arm)
        )

        joystick_b_button = commands2.button.JoystickButton(self.controller, 2)
        joystick_b_button.on_true(
            commands2.InstantCommand(lambda: self.arm.set_angle(90.0), self.arm)
        )
        joystick_b_button.on_false(
            commands2.InstantCommand(lambda: self.arm.set_angle(0.0), self.arm)
        )

        # Set up autonomous options.
        self.chooser.add_default(
            "Auto Routine Distance", AutonomousDistance(self.drivetrain)
        )
        self.chooser.add("Auto Routine Time", AutonomousTime(self.drivetrain))
        tunables.publish("Autonomous", self.chooser)

    @override
    def disabled_start(self) -> None:
        """This function is called each time Disabled mode starts."""

    @override
    def disabled_periodic(self) -> None:
        """This function is called periodically when disabled"""

    @override
    def autonomous_start(self) -> None:
        """This autonomous runs the autonomous command selected by the chooser."""
        self.autonomous_command = self.chooser.get_selected()
        self.autonomous_command.schedule()

    @override
    def autonomous_periodic(self) -> None:
        """This function is called periodically during autonomous"""

    @override
    def teleop_start(self) -> None:
        # This makes sure that the autonomous stops running when
        # teleop starts running. If you want the autonomous to
        # continue until interrupted by another command, remove
        # this line or comment it out.
        if self.autonomous_command:
            self.autonomous_command.cancel()

    @override
    def teleop_periodic(self) -> None:
        """This function is called periodically during teleoperated mode"""

    @override
    def utility_start(self) -> None:
        # Cancels all running commands at the start of utility mode
        commands2.CommandScheduler.get_instance().cancel_all()
