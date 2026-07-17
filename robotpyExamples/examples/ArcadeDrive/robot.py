#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib


class MyRobot(wpilib.TimedRobot):
    """
    This is a demo program showing the use of the DifferentialDrive class.
    Runs the motors with arcade steering.
    """

    def __init__(self):
        """Robot initialization function"""
        super().__init__()

        left_motor = wpilib.PWMSparkMax(0)
        right_motor = wpilib.PWMSparkMax(1)
        self.robot_drive = wpilib.DifferentialDrive(left_motor, right_motor)
        self.stick = wpilib.Joystick(0)

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        right_motor.set_inverted(True)

    def teleop_periodic(self):
        # Drive with arcade drive.
        # That means that the Y axis drives forward
        # and backward, and the X turns left and right.
        self.robot_drive.arcade_drive(-self.stick.get_y(), -self.stick.get_x())
