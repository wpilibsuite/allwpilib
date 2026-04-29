#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib


class MyRobot(wpilib.TimedRobot):
    def __init__(self) -> None:
        super().__init__()

        # Initializes a duty cycle encoder on DIO port 0
        self.encoder = wpilib.DutyCycleEncoder(0)

        # Initializes a duty cycle encoder on DIO port 1 to return a value of 4 for
        # a full rotation, with the encoder reporting 0 half way through rotation (2 out of 4)
        self.encoderFR = wpilib.DutyCycleEncoder(1, 4.0, 2.0)

    def teleopPeriodic(self) -> None:
        # Gets the rotation
        self.encoder.get()

        # Gets if the encoder is connected
        self.encoder.isConnected()
