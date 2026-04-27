#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib


class MyRobot(wpilib.TimedRobot):
    """
    This is a demo program showing the use of OpenCV to do vision processing. The image is acquired
    from the USB camera, then a rectangle is put on the image and sent to the dashboard. OpenCV has
    many methods for different types of processing.
    """

    def __init__(self):
        super().__init__()
        wpilib.CameraServer.launch("vision.py:main")
