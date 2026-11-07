#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import override

import wpilib, wpilog


# Snippets that logs custom data to the WPILib data log for wpilib-docs.
# https://docs.wpilib.org/en/stable/docs/software/telemetry/datalog.html
class MyRobot(wpilib.TimedRobot):

    # Set up the custom entries.
    def __init__(self):
        super().__init__()

        # Starts recording to data log.
        wpilib.DataLogManager.start()

        # Set up custom log entries.
        log = wpilib.DataLogManager.get_log()
        self.boolean_log = wpilog.BooleanLogEntry(log, "/my/boolean")
        self.double_log = wpilog.DoubleLogEntry(log, "/my/double")
        self.string_log = wpilog.StringLogEntry(log, "/my/string")

    # Log the custom entries periodically.
    @override
    def teleop_periodic(self):
        self.boolean_log.append(True)
        self.double_log.append(3.5)
        self.string_log.append("example")
