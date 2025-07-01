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

    Finally, short code snippets show how to invert the motor direction and how to use the motor
    safety for frc-docs.
    https://docs.wpilib.org/en/stable/docs/software/hardware-apis/motors/wpi-drive-classes.html
    """

    MOTOR_PORT = 0
    JOYSTICK_PORT = 0
    ENCODER_PORT_A = 0
    ENCODER_PORT_B = 1

    def __init__(self):
        """Robot initialization function"""
        super().__init__()

        self.motor = wpilib.PWMSparkMax(self.MOTOR_PORT)
        self.joystick = wpilib.Joystick(self.JOYSTICK_PORT)
        self.encoder = wpilib.Encoder(self.ENCODER_PORT_A, self.ENCODER_PORT_B)
        # Use set_distance_per_pulse to set the multiplier for get_distance
        # This is set up assuming a 6 inch wheel with a 360 CPR encoder.
        self.encoder.set_distance_per_pulse((math.pi * 6) / 360.0)

        # show motor inversion
        self.motor.set_inverted(True)

        # show motor safety features
        self.motor.set_safety_enabled(True)
        self.motor.set_safety_enabled(False)
        self.motor.set_expiration(0.1)
        self.motor.feed()

    def robot_periodic(self):
        """The robot_periodic function is called every control packet no matter the robot mode."""
        wpilib.Telemetry.log("Encoder", self.encoder.get_distance())

    def teleop_periodic(self):
        self.motor.set_throttle(self.joystick.get_y())
