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
    Sample program displaying the value of a quadrature encoder through telemetry. Quadrature
    Encoders are digital sensors which can detect the amount the encoder has rotated since starting
    as well as the direction in which the encoder shaft is rotating. However, encoders can not tell
    you the absolute position of the encoder shaft (ie, it considers where it starts to be the zero
    position, no matter where it starts), and so can only tell you how much the encoder has rotated
    since starting. Depending on the precision of an encoder, it will have fewer or greater ticks per
    revolution; the number of ticks per revolution will affect the conversion between ticks and
    distance, as specified by DistancePerPulse. One of the most common uses of encoders is in the
    drivetrain, so that the distance that the robot drives can be precisely controlled during the
    autonomous mode.
    """

    def __init__(self):
        """Robot initialization function"""
        super().__init__()

        self.encoder = wpilib.Encoder(2, 3, False, wpilib.Encoder.EncodingType.X4)

        # Defines how far the mechanism attached to the encoder moves per pulse. In
        # this case, we assume that a 360 count encoder is directly
        # attached to a 3 inch diameter (1.5inch radius) wheel,
        # and that we want to measure distance in inches.
        self.encoder.set_distance_per_pulse(1.0 / 360.0 * 2.0 * math.pi * 1.5)

    def teleop_periodic(self):
        wpilib.Telemetry.log("Encoder Distance", self.encoder.get_distance())
        wpilib.Telemetry.log("Encoder Rate", self.encoder.get_rate())
