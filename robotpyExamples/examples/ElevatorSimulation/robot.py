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
        super().__init__()
        self.joystick = wpilib.Joystick(constants.JOYSTICK_PORT)
        self.elevator = Elevator()

    def robot_periodic(self) -> None:
        # Update the telemetry, including mechanism visualization, regardless of mode.
        self.elevator.update_telemetry()

    def simulation_periodic(self) -> None:
        # Update the simulation model.
        self.elevator.simulation_periodic()

    def teleop_periodic(self) -> None:
        if self.joystick.get_trigger():
            # Here, we set the constant setpoint of 0.75 meters.
            self.elevator.reach_goal(constants.SETPOINT)
        else:
            # Otherwise, we update the setpoint to 0.
            self.elevator.reach_goal(0.0)

    def disabled_init(self) -> None:
        # This just makes sure that our simulation code knows that the motor's off.
        self.elevator.stop()
