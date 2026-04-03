#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import typing

import commands2
import wpilib

from robotcontainer import RobotContainer


class MyRobot(commands2.TimedCommandRobot):
    """
    Command v2 robots are encouraged to inherit from TimedCommandRobot, which
    has an implementation of robot_periodic which runs the scheduler for you
    """

    autonomous_command: typing.Optional[commands2.Command] = None

    def __init__(self) -> None:
        """
        This function is run when the robot is first started up and should be used for any
        initialization code.
        """
        super().__init__()

        # Instantiate our RobotContainer.  This will perform all our button bindings, and put our
        # autonomous chooser on the dashboard.
        self.container = RobotContainer()

        # Start recording to data log
        wpilib.DataLogManager.start()

        # Record DS control and joystick data.
        # Change to `false` to not record joystick data.
        wpilib.DriverStation.start_data_log(wpilib.DataLogManager.get_log(), True)

    def disabled_enter(self) -> None:
        """This function is called each time the robot enters Disabled mode."""

    def disabled_periodic(self) -> None:
        """This function is called periodically when disabled"""

    def autonomous_enter(self) -> None:
        """This autonomous runs the autonomous command selected by your RobotContainer class."""
        self.autonomous_command = self.container.get_autonomous_command()

        if self.autonomous_command:
            self.autonomous_command.schedule()

    def autonomous_periodic(self) -> None:
        """This function is called periodically during autonomous"""

    def teleop_enter(self) -> None:
        # This makes sure that the autonomous stops running when
        # teleop starts running. If you want the autonomous to
        # continue until interrupted by another command, remove
        # this line or comment it out.
        if self.autonomous_command:
            self.autonomous_command.cancel()

    def teleop_periodic(self) -> None:
        """This function is called periodically during operator control"""

    def utility_enter(self) -> None:
        # Cancels all running commands at the start of utility mode
        commands2.CommandScheduler.get_instance().cancel_all()
