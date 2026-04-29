#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math
import wpilib

kMotorPort = 0
kJoystickPort = 0
kEncoderPortA = 0
kEncoderPortB = 1


class MyRobot(wpilib.TimedRobot):
    def __init__(self):
        super().__init__()

        self.motor = wpilib.PWMSparkMax(kMotorPort)
        self.joystick = wpilib.Joystick(kJoystickPort)
        self.encoder = wpilib.Encoder(kEncoderPortA, kEncoderPortB)
        # Use setDistancePerPulse to set the multiplier for getDistance
        # This is set up assuming a 6 inch wheel with a 360 CPR encoder.
        self.encoder.setDistancePerPulse((math.pi * 6) / 360.0)

    def robotPeriodic(self):
        wpilib.SmartDashboard.putNumber("Encoder", self.encoder.getDistance())

    def teleopPeriodic(self):
        self.motor.setThrottle(self.joystick.getY())
