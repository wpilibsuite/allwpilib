#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib

from constants import Constants
from subsystems.arm import Arm


class MyRobot(wpilib.TimedRobot):
    """This is a sample program to demonstrate the use of arm simulation with existing code."""

    def __init__(self) -> None:
        super().__init__()
        self.arm = Arm()
        self.joystick = wpilib.Joystick(Constants.kJoystickPort)

    def simulationPeriodic(self) -> None:
        self.arm.simulationPeriodic()

    def teleopInit(self) -> None:
        self.arm.loadPreferences()

    def teleopPeriodic(self) -> None:
        if self.joystick.getTrigger():
            # Here, we run PID control like normal.
            self.arm.reachSetpoint()
        else:
            # Otherwise, we disable the motor.
            self.arm.stop()

    def disabledInit(self) -> None:
        # This just makes sure that our simulation code knows that the motor's off.
        self.arm.stop()
