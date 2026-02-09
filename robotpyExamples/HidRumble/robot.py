#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib


class MyRobot(wpilib.TimedRobot):
    """
    This is a demo program showing the use of GenericHID's rumble feature.
    """

    def __init__(self):
        """Robot initialization function"""
        super().__init__()

        self.hid = wpilib.NiDsXboxController(0)

    def autonomousInit(self):
        # Turn on rumble at the start of auto
        self.hid.setRumble(wpilib.NiDsXboxController.RumbleType.kLeftRumble, 1.0)
        self.hid.setRumble(wpilib.NiDsXboxController.RumbleType.kRightRumble, 1.0)

    def disabledInit(self):
        # Stop the rumble when entering disabled
        self.hid.setRumble(wpilib.NiDsXboxController.RumbleType.kLeftRumble, 0.0)
        self.hid.setRumble(wpilib.NiDsXboxController.RumbleType.kRightRumble, 0.0)
