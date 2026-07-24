#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib


class MyRobot(wpilib.TimedRobot):
    def __init__(self):
        """Robot initialization function"""
        super().__init__()

        self.duty_cycle = wpilib.DutyCycle(0)

    def robot_periodic(self):
        # Duty Cycle Frequency in Hz
        frequency = self.duty_cycle.get_frequency()

        # Output of duty cycle, ranging from 0 to 1
        # 1 is fully on, 0 is fully off
        output = self.duty_cycle.get_output()

        wpilib.Telemetry.log("Frequency", frequency)
        wpilib.Telemetry.log("Duty Cycle", output)
