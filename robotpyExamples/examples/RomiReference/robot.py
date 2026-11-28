#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

#
# Example that shows how to connect to a ROMI from RobotPy
#
# Requirements
# ------------
#
# You must have the robotpy-halsim-ws package installed. This is best done via:
#
#    # Windows
#    py -3 -m pip install robotpy[commands2,sim]
#
#    # Linux/macOS
#    pip3 install robotpy[commands2,sim]
#
# Run the program
# ---------------
#
# Use the dedicated ROMI command:
#
#    # Windows
#    py -3 -m robotpy run-romi
#
#    # Linux/macOS
#    python -m robotpy run-romi
#
# If your ROMI isn't at the default address, use --host/--port:
#
#    python -m robotpy run-romi --host 10.0.0.2 --port 3300
#
# By default the WPILib simulation GUI will be displayed. To disable the display
# you can add the --nogui option.
#

from typing import override

import commands2
import commands2.button
import telemetry
import tunables
import wpilib
import romi

from commands.arcadedrive import ArcadeDrive
from commands.autonomousdistance import AutonomousDistance
from commands.autonomoustime import AutonomousTime

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
        self.onboard_io = romi.OnBoardIO(
            romi.OnBoardIO.ChannelMode.INPUT, romi.OnBoardIO.ChannelMode.INPUT
        )

        # Assumes a gamepad plugged into channnel 0
        self.controller = wpilib.Joystick(0)

        # Create a tunable selector for autonomous routines.
        self.chooser = tunables.Selectable()

        # NOTE: The I/O pin functionality of the 5 exposed I/O pins depends on the hardware "overlay"
        # that is specified when launching the wpilib-ws server on the Romi raspberry pi.
        # By default, the following are available (listed in order from inside of the board to outside):
        # - DIO 8 (mapped to Arduino pin 11, closest to the inside of the board)
        # - Analog In 0 (mapped to Analog Channel 6 / Arduino Pin 4)
        # - Analog In 1 (mapped to Analog Channel 2 / Arduino Pin 20)
        # - PWM 2 (mapped to Arduino Pin 21)
        # - PWM 3 (mapped to Arduino Pin 22)
        #
        # Your subsystem configuration should take the overlays into account

        self._configure_button_bindings()

    def _configure_button_bindings(self):
        """Use this method to define your button->command mappings. Buttons can be created by
        instantiating a :class:`.GenericHID` or one of its subclasses (:class`.Joystick or
        :class:`.XboxController`), and then passing it to a :class:`.JoystickButton`.
        """

        # Default command is arcade drive. This will run unless another command
        # is scheduler over it
        self.drivetrain.set_default_command(
            ArcadeDrive(
                self.drivetrain,
                lambda: -self.controller.get_raw_axis(1),
                lambda: -self.controller.get_raw_axis(2),
            )
        )

        # Example of how to use the onboard IO
        onboard_button_a = commands2.button.Trigger(
            self.onboard_io.get_button_a_pressed
        )
        onboard_button_a.on_true(commands2.PrintCommand("Button A Pressed")).on_false(
            commands2.PrintCommand("Button A Released")
        )

        # Set up autonomous options.
        self.chooser.add_default(
            "Auto Routine Distance", AutonomousDistance(self.drivetrain)
        )
        self.chooser.add("Auto Routine Time", AutonomousTime(self.drivetrain))
        tunables.publish("Autonomous", self.chooser)

    @override
    def robot_periodic(self) -> None:
        super().robot_periodic()
        telemetry.log("drivetrain", self.drivetrain)

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
