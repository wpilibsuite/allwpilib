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
    Sample program displaying the value of a quadrature encoder on the SmartDashboard. Quadrature
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

        self.encoder = wpilib.Encoder(1, 2, False, wpilib.Encoder.EncodingType.k4X)

        # Defines the number of samples to average when determining the rate.
        # On a quadrature encoder, values range from 1-255;
        # larger values result in smoother but potentially
        # less accurate rates than lower values.
        self.encoder.setSamplesToAverage(5)

        # Defines how far the mechanism attached to the encoder moves per pulse. In
        # this case, we assume that a 360 count encoder is directly
        # attached to a 3 inch diameter (1.5inch radius) wheel,
        # and that we want to measure distance in inches.
        self.encoder.setDistancePerPulse(1.0 / 360.0 * 2.0 * math.pi * 1.5)

        # Defines the lowest rate at which the encoder will
        # not be considered stopped, for the purposes of
        # the GetStopped() method. Units are in distance / second,
        # where distance refers to the units of distance
        # that you are using, in this case inches.
        self.encoder.setMinRate(1.0)

    def teleopPeriodic(self):
        wpilib.SmartDashboard.putNumber("Encoder Distance", self.encoder.getDistance())
        wpilib.SmartDashboard.putNumber("Encoder Rate", self.encoder.getRate())
