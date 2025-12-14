#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import wpimath


class MyRobot(wpilib.TimedRobot):
    """
    This is a sample program to demonstrate how to use a soft potentiometer and a PID controller to
    reach and maintain position setpoints on an elevator mechanism.
    """

    kPotChannel = 1
    kMotorChannel = 7
    kJoystickChannel = 3

    # The elevator can move 1.5 meters from top to bottom
    kFullHeightMeters = 1.5

    # Bottom, middle, and top elevator setpoints
    kSetpointMeters = [0.2, 0.8, 1.4]

    # proportional, integral, and derivative velocity constants
    # DANGER: when tuning PID constants, high/inappropriate values for kP, kI,
    # and kD may cause dangerous, uncontrollable, or undesired behavior!
    kP = 0.7
    kI = 0.35
    kD = 0.25

    def __init__(self):
        super().__init__()

        self.pidController = wpimath.PIDController(self.kP, self.kI, self.kD)
        # Scaling is handled internally
        self.potentiometer = wpilib.AnalogPotentiometer(
            self.kPotChannel, self.kFullHeightMeters
        )
        self.elevatorMotor = wpilib.PWMSparkMax(self.kMotorChannel)
        self.joystick = wpilib.Joystick(self.kJoystickChannel)

    def teleopInit(self):
        # Move to the bottom setpoint when teleop starts
        self.index = 0
        self.pidController.setSetpoint(self.kSetpointMeters[self.index])

    def teleopPeriodic(self):
        # Read from the sensor
        position = self.potentiometer.get()

        # Run the PID Controller
        pidOut = self.pidController.calculate(position)

        # Apply PID output
        self.elevatorMotor.set(pidOut)

        # when the button is pressed once, the selected elevator setpoint is incremented
        if self.joystick.getTriggerPressed():
            # index of the elevator setpoint wraps around.
            self.index = (self.index + 1) % len(self.kSetpointMeters)
            print(f"index = {self.index}")
            self.pidController.setSetpoint(self.kSetpointMeters[self.index])
