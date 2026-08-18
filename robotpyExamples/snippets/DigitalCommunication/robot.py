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
    ALLIANCE_PORT = 0
    ENABLED_PORT = 1
    AUTONOMOUS_PORT = 2
    ALERT_PORT = 3

    def __init__(self):
        """Robot initialization function"""
        super().__init__()

        self.alliance_output = wpilib.DigitalOutput(self.ALLIANCE_PORT)
        self.enabled_output = wpilib.DigitalOutput(self.ENABLED_PORT)
        self.autonomous_output = wpilib.DigitalOutput(self.AUTONOMOUS_PORT)
        self.alert_output = wpilib.DigitalOutput(self.ALERT_PORT)

    def robot_periodic(self):
        set_alliance = False
        alliance = wpilib.MatchState.get_alliance()
        if alliance:
            set_alliance = alliance == wpilib.Alliance.RED

        # pull alliance port high if on red alliance, pull low if on blue alliance
        self.alliance_output.set(set_alliance)

        # pull enabled port high if enabled, low if disabled
        self.enabled_output.set(wpilib.RobotState.is_enabled())

        # pull auto port high if in autonomous, low if in teleop (or disabled)
        self.autonomous_output.set(wpilib.RobotState.is_autonomous())

        # pull alert port high if match time remaining is between 30 and 25 seconds
        match_time = wpilib.MatchState.get_match_time()
        self.alert_output.set(30 >= match_time >= 25)
