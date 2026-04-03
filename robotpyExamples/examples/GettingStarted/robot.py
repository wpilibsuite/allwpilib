#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib


class MyRobot(wpilib.TimedRobot):
    def __init__(self):
        """
        This function is called upon program startup and
        should be used for any initialization code.
        """
        super().__init__()
        self.left_drive = wpilib.PWMSparkMax(0)
        self.right_drive = wpilib.PWMSparkMax(1)
        self.robot_drive = wpilib.DifferentialDrive(self.left_drive, self.right_drive)
        self.controller = wpilib.Gamepad(0)
        self.timer = wpilib.Timer()

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.right_drive.set_inverted(True)

    def autonomous_enter(self):
        """This function is called each time the robot enters autonomous mode."""
        self.timer.restart()

    def autonomous_periodic(self):
        """This function is called periodically during autonomous."""

        # Drive for two seconds
        if self.timer.get() < 2.0:
            # Drive forwards half velocity, make sure to turn input squaring off
            self.robot_drive.arcade_drive(0.5, 0, square_inputs=False)
        else:
            self.robot_drive.stop_motor()  # Stop robot

    def teleop_enter(self):
        """This function is called each time the robot enters teleoperated mode."""

    def teleop_periodic(self):
        """This function is called periodically during teleoperated mode."""
        self.robot_drive.arcade_drive(
            -self.controller.get_left_y(), -self.controller.get_right_x()
        )

    def utility_enter(self):
        """This function is called once each time the robot enters utility mode."""

    def utility_periodic(self):
        """This function is called periodically during utility mode."""
