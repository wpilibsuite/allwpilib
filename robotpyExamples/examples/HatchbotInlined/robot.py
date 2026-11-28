#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import override

import commands2
import commands2.button
import commands2.cmd
import telemetry
import tunables
import wpilib

import constants

import commands.autos

from subsystems.drivesubsystem import DriveSubsystem
from subsystems.hatchsubsystem import HatchSubsystem


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

        # The robot's subsystems
        self.drive_subsystem = DriveSubsystem()
        self.hatch_subsystem = HatchSubsystem()

        # Retained command handles

        # A simple auto routine that drives forward a specified distance, and then stops.
        self.simple_auto = commands.autos.Autos.simple_auto(self.drive_subsystem)

        # A complex auto routine that drives forward, drops a hatch, and then drives backward.
        self.complex_auto = commands.autos.Autos.complex_auto(
            self.drive_subsystem, self.hatch_subsystem
        )

        # The driver's controller
        self.driver_controller = commands2.button.CommandDualShock4Controller(
            constants.DRIVER_CONTROLLER_PORT
        )

        # Configure the button bindings
        self.configure_button_bindings()

        # Configure default commands
        # Set the default drive command to split-stick arcade drive
        self.drive_subsystem.set_default_command(
            # A split-stick arcade command, with forward/backward controlled by the left
            # hand, and turning controlled by the right.
            commands2.cmd.run(
                lambda: self.drive_subsystem.arcade_drive(
                    -self.driver_controller.get_left_y(),
                    -self.driver_controller.get_right_x(),
                ),
                self.drive_subsystem,
            )
        )

        # Chooser
        self.chooser = tunables.Selectable()

        # Add commands to the autonomous command chooser
        self.chooser.add_default("Simple Auto", self.simple_auto)
        self.chooser.add("Complex Auto", self.complex_auto)

        # Put the chooser on the dashboard
        tunables.publish("Autonomous", self.chooser)

        # Start recording to data log
        wpilib.DataLogManager.start()

        # Record DS control and joystick data.
        # Change to `false` to not record joystick data.
        wpilib.DriverStation.start_data_log(wpilib.DataLogManager.get_log(), True)

    def configure_button_bindings(self):
        """
        Use this method to define your button->command mappings. Buttons can be created by
        instantiating a wpilib.GenericHID or one of its subclasses (Joystick or XboxController),
        and then passing it to a JoystickButton.
        """

        # Grab the hatch when the Circle button is pressed.
        self.driver_controller.circle().on_true(self.hatch_subsystem.grab_hatch())

        # Release the hatch when the Square button is pressed.
        self.driver_controller.square().on_true(self.hatch_subsystem.release_hatch())

        # While holding R1, drive at half velocity
        self.driver_controller.r1().on_true(
            commands2.cmd.run_once(lambda: self.drive_subsystem.set_max_output(0.5))
        ).on_false(
            commands2.cmd.run_once(lambda: self.drive_subsystem.set_max_output(1))
        )

    @override
    def robot_periodic(self) -> None:
        super().robot_periodic()
        telemetry.log("Drivetrain", self.drive_subsystem)
        telemetry.log("HatchSubsystem", self.hatch_subsystem)

    @override
    def disabled_start(self) -> None:
        """This function is called each time Disabled mode starts."""

    @override
    def disabled_periodic(self) -> None:
        """This function is called periodically when disabled"""

    @override
    def autonomous_start(self) -> None:
        """This autonomous runs the autonomous command selected by your chooser."""
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
