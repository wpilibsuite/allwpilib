# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import wpilib
import wpimath

FULL_RANGE = 1.3
EXPECTED_ZERO = 0.0


class MyRobot(wpilib.TimedRobot):
    def __init__(self):
        super().__init__()

        # 2nd parameter is the range of values. This sensor will output between
        # 0 and the passed in value.
        # 3rd parameter is the physical value where you want "0" to be. How
        # to measure this is fairly easy. Set the value to 0, place the mechanism
        # where you want "0" to be, and observe the value on the dashboard. That
        # is the value to enter for the 3rd parameter.
        self.dutyCycleEncoder = wpilib.DutyCycleEncoder(0, FULL_RANGE, EXPECTED_ZERO)

        # If you know the frequency of your sensor, uncomment the following
        # method, and set the method to the frequency of your sensor.
        # This will result in more stable readings from the sensor.
        self.dutyCycleEncoder.setAssumedFrequency(967.8)

    def robotPeriodic(self):
        # Connected can be checked, and uses the frequency of the encoder
        connected = self.dutyCycleEncoder.isConnected()

        # Duty Cycle Frequency in Hz
        frequency = self.dutyCycleEncoder.getFrequency()

        # Output of encoder
        output = self.dutyCycleEncoder.get()

        # Shift the output slightly so the sensor can go a bit negative before
        # wrapping. Usually 10% or so is fine.
        percentOfRange = FULL_RANGE * 0.1
        shiftedOutput = wpimath.inputModulus(
            output, 0 - percentOfRange, FULL_RANGE - percentOfRange
        )

        wpilib.SmartDashboard.putBoolean("Connected", connected)
        wpilib.SmartDashboard.putNumber("Frequency", frequency)
        wpilib.SmartDashboard.putNumber("Output", output)
        wpilib.SmartDashboard.putNumber("ShiftedOutput", shiftedOutput)
