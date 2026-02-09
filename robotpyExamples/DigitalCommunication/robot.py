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
    code using the roboRIO's DIO ports.
    """

    # define ports for digitalcommunication with light controller
    kAlliancePort = 0
    kEnabledPort = 1
    kAutonomousPort = 2
    kAlertPort = 3

    def __init__(self):
        """Robot initialization function"""
        super().__init__()

        self.allianceOutput = wpilib.DigitalOutput(self.kAlliancePort)
        self.enabledOutput = wpilib.DigitalOutput(self.kEnabledPort)
        self.autonomousOutput = wpilib.DigitalOutput(self.kAutonomousPort)
        self.alertOutput = wpilib.DigitalOutput(self.kAlertPort)

    def robotPeriodic(self):
        setAlliance = False
        alliance = wpilib.DriverStation.getAlliance()
        if alliance:
            setAlliance = alliance == wpilib.DriverStation.Alliance.kRed

        # pull alliance port high if on red alliance, pull low if on blue alliance
        self.allianceOutput.set(setAlliance)

        # pull enabled port high if enabled, low if disabled
        self.enabledOutput.set(wpilib.DriverStation.isEnabled())

        # pull auto port high if in autonomous, low if in teleop (or disabled)
        self.autonomousOutput.set(wpilib.DriverStation.isAutonomous())

        # pull alert port high if match time remaining is between 30 and 25 seconds
        matchTime = wpilib.DriverStation.getMatchTime()
        self.alertOutput.set(30 >= matchTime >= 25)
