#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math
import wpilib


class MyRobot(wpilib.TimedRobot):
    def __init__(self):
        super().__init__()

        # Initializes an encoder on DIO pins 0 and 1
        # Defaults to 4X decoding and non-inverted
        self.encoder = wpilib.Encoder(0, 1)

        # Initializes an encoder on DIO pins 0 and 1
        # 2X encoding and non-inverted
        self.encoder2x = wpilib.Encoder(0, 1, False, wpilib.Encoder.EncodingType.X2)

        # Configures the encoder to return a distance of 4 for every 256 pulses
        # Also changes the units of getRate
        self.encoder.setDistancePerPulse(4.0 / 256.0)
        # Configures the encoder to consider itself stopped after .1 seconds
        self.encoder.setMaxPeriod(0.1)
        # Configures the encoder to consider itself stopped when its rate is below 10
        self.encoder.setMinRate(10)
        # Reverses the direction of the encoder
        self.encoder.setReverseDirection(True)
        # Configures an encoder to average its period measurement over 5 samples
        # Can be between 1 and 127 samples
        self.encoder.setSamplesToAverage(5)

    def teleopPeriodic(self):
        # Gets the distance traveled
        self.encoder.getDistance()

        # Gets the current rate of the encoder
        self.encoder.getRate()

        # Gets whether the encoder is stopped
        self.encoder.getStopped()

        # Gets the last direction in which the encoder moved
        self.encoder.getDirection()

        # Gets the current period of the encoder
        self.encoder.getPeriod()

        # Resets the encoder to read a distance of zero
        self.encoder.reset()
