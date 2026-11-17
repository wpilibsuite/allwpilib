#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import override

from commands2 import CommandScheduler, TimedCommandRobot

from sysidroutinebot import SysIdRoutineBot


class MyRobot(TimedCommandRobot):
    """
    The methods in this class are called automatically corresponding to each mode,
    as described in the TimedRobot documentation.
    """

    def __init__(self) -> None:
        """This function is run when the robot is first started up and should be used for any
        initialization code.
        """
        super().__init__()
        self.robot = SysIdRoutineBot()

        # Configure default commands and condition bindings on robot startup
        self.robot.configure_bindings()

        self.autonomous_command = None

    @override
    def disabled_init(self) -> None:
        """This function is called once each time the robot enters Disabled mode."""
        pass

    @override
    def disabled_periodic(self) -> None:
        pass

    @override
    def autonomous_init(self) -> None:
        self.autonomous_command = self.robot.get_autonomous_command()

        if self.autonomous_command is not None:
            CommandScheduler.get_instance().schedule(self.autonomous_command)

    @override
    def autonomous_periodic(self) -> None:
        """This function is called periodically during autonomous."""
        pass

    @override
    def teleop_init(self) -> None:
        # This makes sure that the autonomous stops running when
        # teleop starts running. If you want the autonomous to
        # continue until interrupted by another command, remove
        # this line or comment it out.
        if self.autonomous_command is not None:
            self.autonomous_command.cancel()

    @override
    def teleop_periodic(self) -> None:
        """This function is called periodically during operator control."""
        pass

    @override
    def utility_init(self) -> None:
        # Cancels all running commands at the start of utility mode.
        CommandScheduler.get_instance().cancel_all()

    @override
    def utility_periodic(self) -> None:
        """This function is called periodically during utility mode."""
        pass
