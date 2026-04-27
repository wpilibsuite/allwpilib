#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import wpimath

from examplesmartmotorcontroller import ExampleSmartMotorController


class MyRobot(wpilib.TimedRobot):
    kDt = 0.02

    def __init__(self) -> None:
        super().__init__()
        self.joystick = wpilib.Joystick(1)
        self.motor = ExampleSmartMotorController(1)
        # Note: These gains are fake, and will have to be tuned for your robot.
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 1, 1)

        # Create a motion profile with the given maximum voltage and characteristics kV, kA
        # These gains should match your feedforward kV, kA for best results.
        self.profile = wpimath.ExponentialProfileMeterVolts(
            wpimath.ExponentialProfileMeterVolts.Constraints.fromCharacteristics(
                10, 1, 1
            )
        )
        self.goal = wpimath.ExponentialProfileMeterVolts.State(0, 0)
        self.setpoint = wpimath.ExponentialProfileMeterVolts.State(0, 0)

        # Note: These gains are fake, and will have to be tuned for your robot.
        self.motor.setPID(1.3, 0.0, 0.7)

    def teleopPeriodic(self) -> None:
        if self.joystick.getRawButtonPressed(2):
            self.goal = wpimath.ExponentialProfileMeterVolts.State(5, 0)
        elif self.joystick.getRawButtonPressed(3):
            self.goal = wpimath.ExponentialProfileMeterVolts.State(0, 0)

        # Retrieve the profiled setpoint for the next timestep. This setpoint moves
        # toward the goal while obeying the constraints.
        next_state = self.profile.calculate(self.kDt, self.setpoint, self.goal)

        # Send setpoint to offboard controller PID
        self.motor.setSetpoint(
            ExampleSmartMotorController.PIDMode.kPosition,
            self.setpoint.position,
            self.feedforward.calculate(next_state.velocity) / 12.0,
        )

        self.setpoint = next_state
