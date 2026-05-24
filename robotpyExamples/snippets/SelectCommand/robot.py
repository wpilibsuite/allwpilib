#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import typing
import commands2

from robotcontainer import RobotContainer


class MyRobot(commands2.TimedCommandRobot):
    autonomousCommand: typing.Optional[commands2.Command] = None

    def __init__(self) -> None:
        super().__init__()
        # Instantiate our RobotContainer.  This will perform all our button bindings,
        # and put our autonomous chooser on the dashboard.
        self.container = RobotContainer()

    def robotPeriodic(self) -> None:
        # Runs the Scheduler. This is responsible for polling buttons, adding
        # newly-scheduled commands, running already-scheduled commands, removing
        # finished or interrupted commands, and running subsystem periodic() methods.
        commands2.CommandScheduler.getInstance().run()

    def autonomousInit(self) -> None:
        self.autonomousCommand = self.container.getAutonomousCommand()

        if self.autonomousCommand:
            self.autonomousCommand.schedule()

    def teleopInit(self) -> None:
        if self.autonomousCommand:
            self.autonomousCommand.cancel()

    def utilityInit(self) -> None:
        commands2.CommandScheduler.getInstance().cancelAll()
