#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import override

import commands2
import commands2.button
import telemetry
import tunables
import wpilib

import constants

from commands.complexauto import ComplexAuto
from commands.drivedistance import DriveDistance
from commands.defaultdrive import DefaultDrive
from commands.grabhatch import GrabHatch
from commands.halvedrivevelocity import HalveDriveVelocity
from commands.releasehatch import ReleaseHatch

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

        # The driver's controller
        # self.driver_controller = wpilib.XboxController(constants.DRIVER_CONTROLLER_PORT)
        self.driver_controller = wpilib.Joystick(constants.DRIVER_CONTROLLER_PORT)

        # The robot's subsystems
        self.drive = DriveSubsystem()
        self.hatch = HatchSubsystem()

        # Autonomous routines

        # A simple auto routine that drives forward a specified distance, and then stops.
        self.simple_auto = DriveDistance(
            constants.AUTO_DRIVE_DISTANCE_INCHES,
            constants.AUTO_DRIVE_VELOCITY,
            self.drive,
        )

        # A complex auto routine that drives forward, drops a hatch, and then drives backward.
        self.complex_auto = ComplexAuto(self.drive, self.hatch)

        # Chooser
        self.chooser = tunables.Selectable()

        # Add commands to the autonomous command chooser
        self.chooser.add_default("Simple Auto", self.simple_auto)
        self.chooser.add("Complex Auto", self.complex_auto)

        # Put the chooser on the dashboard
        tunables.publish("Autonomous", self.chooser)

        self.configure_button_bindings()

        # set up default drive command
        self.drive.set_default_command(
            DefaultDrive(
                self.drive,
                lambda: -self.driver_controller.get_y(),
                lambda: self.driver_controller.get_x(),
            )
        )

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

        commands2.button.JoystickButton(self.driver_controller, 1).on_true(
            GrabHatch(self.hatch)
        )

        commands2.button.JoystickButton(self.driver_controller, 2).on_true(
            ReleaseHatch(self.hatch)
        )

        commands2.button.JoystickButton(self.driver_controller, 3).while_true(
            HalveDriveVelocity(self.drive)
        )

    @override
    def robot_periodic(self) -> None:
        super().robot_periodic()
        telemetry.log("Drivetrain", self.drive)
        telemetry.log("HatchSubsystem", self.hatch)

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
