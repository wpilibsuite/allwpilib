#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib

import constants
from subsystems.elevator import Elevator


class MyRobot(wpilib.TimedRobot):
    """This is a sample program to demonstrate the use of elevator simulation."""

    def __init__(self) -> None:
        super().__init__(0.020)
        self.joystick = wpilib.Joystick(constants.kJoystickPort)
        self.elevator = Elevator()

    def robotPeriodic(self) -> None:
        # Update the telemetry, including mechanism visualization, regardless of mode.
        self.elevator.updateTelemetry()

    def simulationPeriodic(self) -> None:
        # Update the simulation model.
        self.elevator.simulationPeriodic()

    def teleopInit(self) -> None:
        self.elevator.reset()

    def teleopPeriodic(self) -> None:
        if self.joystick.getTrigger():
            # Here, we set the constant setpoint of 10 meters.
            self.elevator.reachGoal(constants.kSetpoint)
        else:
            # Otherwise, we update the setpoint to 1 meter.
            self.elevator.reachGoal(constants.kLowerkSetpoint)

    def disabledInit(self) -> None:
        # This just makes sure that our simulation code knows that the motor's off.
        self.elevator.stop()
