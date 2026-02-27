#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

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
        self.robot.configureBindings()

        self.autonomous_command = None

    def disabledInit(self) -> None:
        """This function is called once each time the robot enters Disabled mode."""
        pass

    def disabledPeriodic(self) -> None:
        pass

    def autonomousInit(self) -> None:
        self.autonomous_command = self.robot.getAutonomousCommand()

        if self.autonomous_command is not None:
            CommandScheduler.getInstance().schedule(self.autonomous_command)

    def autonomousPeriodic(self) -> None:
        """This function is called periodically during autonomous."""
        pass

    def teleopInit(self) -> None:
        # This makes sure that the autonomous stops running when
        # teleop starts running. If you want the autonomous to
        # continue until interrupted by another command, remove
        # this line or comment it out.
        if self.autonomous_command is not None:
            self.autonomous_command.cancel()

    def teleopPeriodic(self) -> None:
        """This function is called periodically during operator control."""
        pass

    def testInit(self) -> None:
        # Cancels all running commands at the start of test mode.
        CommandScheduler.getInstance().cancelAll()

    def testPeriodic(self) -> None:
        """This function is called periodically during test mode."""
        pass
