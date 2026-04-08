#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#


import wpilib
import wpilib.cameraserver


class MyRobot(wpilib.TimedRobot):
    def __init__(self):
        super().__init__()
        # Your image processing code will be launched via a stub that will set up logging and initialize NetworkTables
        # to talk to your robot code.
        # https://robotpy.readthedocs.io/en/stable/vision/roborio.html#important-notes

        wpilib.CameraServer.launch("vision.py:main")
