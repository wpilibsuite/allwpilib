#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import wpilib
import wpimath


class MyRobot(wpilib.TimedRobot):
    DT = 0.02
    MAX_VELOCITY = 1.75
    MAX_ACCELERATION = 0.75
    P = 1.3
    I = 0.0
    D = 0.7
    S = 1.1
    G = 1.2
    V = 1.3

    def __init__(self) -> None:
        super().__init__()
        self.joystick = wpilib.Joystick(1)
        self.encoder = wpilib.Encoder(2, 3)
        self.motor = wpilib.PWMSparkMax(1)

        # Create a PID controller whose setpoint's change is subject to maximum
        # velocity and acceleration constraints.
        self.constraints = wpimath.TrapezoidProfile.Constraints(
            self.MAX_VELOCITY, self.MAX_ACCELERATION
        )
        self.controller = wpimath.ProfiledPIDController(
            self.P, self.I, self.D, self.constraints, self.DT
        )
        self.feedforward = wpimath.ElevatorFeedforward(self.S, self.G, self.V)

        self.encoder.set_distance_per_pulse(1 / 360 * 2 * math.pi * 1.5)

    def teleop_periodic(self) -> None:
        if self.joystick.get_raw_button_pressed(2):
            self.controller.set_goal(5)
        elif self.joystick.get_raw_button_pressed(3):
            self.controller.set_goal(0)

        # Run controller and update motor output
        self.motor.set_voltage(
            self.controller.calculate(self.encoder.get_distance())
            + self.feedforward.calculate(self.controller.get_setpoint().velocity)
        )
