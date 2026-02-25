#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib


class MyRobot(wpilib.TimedRobot):
    """
    This is a sample program demonstrating how to communicate to a light controller from the robot
    code using the roboRIO's I2C port.
    """

    PORT = wpilib.I2C.Port.kPort0
    DEVICE_ADDRESS = 4

    def __init__(self):
        """Robot initialization function"""
        super().__init__()

        self.arduino = wpilib.I2C(self.PORT, self.DEVICE_ADDRESS)

    def writeString(self, s: str):
        # Creates a char array from the input string
        chars = s.encode("ascii")

        # Writes bytes over I2C
        self.arduino.writeBulk(chars)

    def robotPeriodic(self):
        # Creates a string to hold current robot state information, including
        # alliance, enabled state, operation mode, and match time. The message
        # is sent in format "AEM###" where A is the alliance color, (R)ed or
        # (B)lue, E is the enabled state, (E)nabled or (D)isabled, M is the
        # operation mode, (A)utonomous or (T)eleop, and ### is the zero-padded
        # time remaining in the match.
        #
        # For example, "RET043" would indicate that the robot is on the red
        # alliance, enabled in teleop mode, with 43 seconds left in the match.
        allianceString = "U"
        alliance = wpilib.DriverStation.getAlliance()
        if alliance is not None:
            allianceString = (
                "R" if alliance == wpilib.DriverStation.Alliance.kRed else "B"
            )

        enabledString = "E" if wpilib.DriverStation.isEnabled() else "D"
        autoString = "A" if wpilib.DriverStation.isAutonomous() else "T"
        matchTime = wpilib.DriverStation.getMatchTime()

        stateMessage = f"{allianceString}{enabledString}{autoString}{matchTime:03f}"

        self.writeString(stateMessage)
