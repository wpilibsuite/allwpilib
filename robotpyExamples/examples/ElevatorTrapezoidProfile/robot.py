#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
from wpimath import TrapezoidProfile

import examplesmartmotorcontroller
import wpimath


class MyRobot(wpilib.TimedRobot):
    kDt = 0.02

    def __init__(self):
        super().__init__()
        self.joystick = wpilib.Joystick(1)
        self.motor = examplesmartmotorcontroller.ExampleSmartMotorController(1)
        # Note: These gains are fake, and will have to be tuned for your robot.
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 1.5)

        # Create a motion profile with the given maximum velocity and maximum
        # acceleration constraints for the next setpoint.
        self.profile = TrapezoidProfile(TrapezoidProfile.Constraints(1.75, 0.75))

        self.goal = TrapezoidProfile.State()
        self.setpoint = TrapezoidProfile.State()

        # Note: These gains are fake, and will have to be tuned for your robot.
        self.motor.setPID(1.3, 0.0, 0.7)

    def teleopPeriodic(self):
        if self.joystick.getRawButtonPressed(2):
            self.goal = TrapezoidProfile.State(5, 0)
        elif self.joystick.getRawButtonPressed(3):
            self.goal = TrapezoidProfile.State(0, 0)

        # Retrieve the profiled setpoint for the next timestep. This setpoint moves
        # toward the goal while obeying the constraints.
        self.setpoint = self.profile.calculate(self.kDt, self.setpoint, self.goal)

        # Send setpoint to offboard controller PID
        self.motor.setSetPoint(
            examplesmartmotorcontroller.ExampleSmartMotorController.PIDMode.kPosition,
            self.setpoint.position,
            self.feedforward.calculate(self.setpoint.velocity) / 12,
        )
