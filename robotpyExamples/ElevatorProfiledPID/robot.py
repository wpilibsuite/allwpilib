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
    kDt = 0.02
    kMaxVelocity = 1.75
    kMaxAcceleration = 0.75
    kP = 1.3
    kI = 0.0
    kD = 0.7
    kS = 1.1
    kG = 1.2
    kV = 1.3

    def __init__(self) -> None:
        super().__init__()
        self.joystick = wpilib.Joystick(1)
        self.encoder = wpilib.Encoder(1, 2)
        self.motor = wpilib.PWMSparkMax(1)

        # Create a PID controller whose setpoint's change is subject to maximum
        # velocity and acceleration constraints.
        self.constraints = wpimath.TrapezoidProfile.Constraints(
            self.kMaxVelocity, self.kMaxAcceleration
        )
        self.controller = wpimath.ProfiledPIDController(
            self.kP, self.kI, self.kD, self.constraints, self.kDt
        )
        self.feedforward = wpimath.ElevatorFeedforward(self.kS, self.kG, self.kV)

        self.encoder.setDistancePerPulse(1 / 360 * 2 * math.pi * 1.5)

    def teleopPeriodic(self) -> None:
        if self.joystick.getRawButtonPressed(2):
            self.controller.setGoal(5)
        elif self.joystick.getRawButtonPressed(3):
            self.controller.setGoal(0)

        # Run controller and update motor output
        self.motor.setVoltage(
            self.controller.calculate(self.encoder.getDistance())
            + self.feedforward.calculate(self.controller.getSetpoint().velocity)
        )
