#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib

# define ports for digital communication with light controller
kAlliancePort = 0
kEnabledPort = 1
kAutonomousPort = 2
kAlertPort = 3


class MyRobot(wpilib.TimedRobot):
    def __init__(self):
        super().__init__()

        self.allianceOutput = wpilib.DigitalOutput(kAlliancePort)
        self.enabledOutput = wpilib.DigitalOutput(kEnabledPort)
        self.autonomousOutput = wpilib.DigitalOutput(kAutonomousPort)
        self.alertOutput = wpilib.DigitalOutput(kAlertPort)

    def robotPeriodic(self):
        setAlliance = False
        alliance = wpilib.MatchState.getAlliance()
        if alliance:
            setAlliance = alliance == wpilib.Alliance.RED

        # pull alliance port high if on red alliance, pull low if on blue alliance
        self.allianceOutput.set(setAlliance)

        # pull enabled port high if enabled, low if disabled
        self.enabledOutput.set(wpilib.RobotState.isEnabled())

        # pull auto port high if in autonomous, low if in teleop (or disabled)
        self.autonomousOutput.set(wpilib.RobotState.isAutonomous())

        # pull alert port high if match time remaining is between 30 and 25 seconds
        matchTime = wpilib.MatchState.getMatchTime()
        self.alertOutput.set(30 >= matchTime >= 25)
