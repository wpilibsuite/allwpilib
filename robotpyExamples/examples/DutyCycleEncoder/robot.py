#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

"""
This example shows how to use a duty cycle encoder for devices such as
an arm or elevator.
"""

import wpilib
import wpimath

FULL_RANGE = 1.3
EXPECTED_ZERO = 0.0


class MyRobot(wpilib.TimedRobot):
    def __init__(self):
        """Called once at the beginning of the robot program."""
        super().__init__()

        # 2nd parameter is the range of values. This sensor will output between
        # 0 and the passed in value.
        # 3rd parameter is the the physical value where you want "0" to be. How
        # to measure this is fairly easy. Set the value to 0, place the mechanism
        # where you want "0" to be, and observe the value on the dashboard, That
        # is the value to enter for the 3rd parameter.
        self.dutyCycleEncoder = wpilib.DutyCycleEncoder(0, FULL_RANGE, EXPECTED_ZERO)

        # If you know the frequency of your sensor, uncomment the following
        # method, and set the method to the frequency of your sensor.
        # This will result in more stable readings from the sensor.
        # Do note that occasionally the datasheet cannot be trusted
        # and you should measure this value. You can do so with either
        # an oscilloscope, or by observing the "Frequency" output
        # on the dashboard while running this sample. If you find
        # the value jumping between the 2 values, enter halfway between
        # those values. This number doesn't have to be perfect,
        # just having a fairly close value will make the output readings
        # much more stable.
        self.dutyCycleEncoder.setAssumedFrequency(967.8)

    def robotPeriodic(self):
        # Connected can be checked, and uses the frequency of the encoder
        connected = self.dutyCycleEncoder.isConnected()

        # Duty Cycle Frequency in Hz
        frequency = self.dutyCycleEncoder.getFrequency()

        # Output of encoder
        output = self.dutyCycleEncoder.get()

        # By default, the output will wrap around to the full range value
        # when the sensor goes below 0. However, for moving mechanisms this
        # is not usually ideal, as if 0 is set to a hard stop, its still
        # possible for the sensor to move slightly past. If this happens
        # The sensor will assume its now at the furthest away position,
        # which control algorithms might not handle correctly. Therefore
        # it can be a good idea to slightly shift the output so the sensor
        # can go a bit negative before wrapping. Usually 10% or so is fine.
        # This does not change where "0" is, so no calibration numbers need
        # to be changed.
        percentOfRange = FULL_RANGE * 0.1
        shiftedOutput = wpimath.inputModulus(
            output, 0 - percentOfRange, FULL_RANGE - percentOfRange
        )

        wpilib.SmartDashboard.putBoolean("Connected", connected)
        wpilib.SmartDashboard.putNumber("Frequency", frequency)
        wpilib.SmartDashboard.putNumber("Output", output)
        wpilib.SmartDashboard.putNumber("ShiftedOutput", shiftedOutput)
