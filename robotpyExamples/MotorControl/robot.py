#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib

import math


class MyRobot(wpilib.TimedRobot):
    """
    This sample program shows how to control a motor using a joystick. In the operator control part
    of the program, the joystick is read and the value is written to the motor.

    Joystick analog values range from -1 to 1 and motor controller inputs also range from -1 to 1
    making it easy to work together.

    In addition, the encoder value of an encoder connected to ports 0 and 1 is consistently sent
    to the Dashboard.
    """

    kMotorPort = 0
    kJoystickPort = 0
    kEncoderPortA = 0
    kEncoderPortB = 1

    def __init__(self):
        """Robot initialization function"""
        super().__init__()

        self.motor = wpilib.PWMSparkMax(self.kMotorPort)
        self.joystick = wpilib.Joystick(self.kJoystickPort)
        self.encoder = wpilib.Encoder(self.kEncoderPortA, self.kEncoderPortB)
        # Use SetDistancePerPulse to set the multiplier for GetDistance
        # This is set up assuming a 6 inch wheel with a 360 CPR encoder.
        self.encoder.setDistancePerPulse((math.pi * 6) / 360.0)

    def robotPeriodic(self):
        """The RobotPeriodic function is called every control packet no matter the robot mode."""
        wpilib.SmartDashboard.putNumber("Encoder", self.encoder.getDistance())

    def teleopPeriodic(self):
        self.motor.set(self.joystick.getY())
