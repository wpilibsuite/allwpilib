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
    Runs the motors with split arcade steering and an Xbox controller.
    """

    def __init__(self):
        """Robot initialization function"""
        super().__init__()

        leftMotor = wpilib.PWMSparkMax(0)
        rightMotor = wpilib.PWMSparkMax(1)
        self.robotDrive = wpilib.DifferentialDrive(leftMotor, rightMotor)
        self.driverController = wpilib.NiDsXboxController(0)

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        rightMotor.setInverted(True)

    def teleopPeriodic(self):
        # Drive with split arcade style
        # That means that the Y axis of the left stick moves forward
        # and backward, and the X of the right stick turns left and right.
        self.robotDrive.arcadeDrive(
            -self.driverController.getLeftY(), -self.driverController.getRightX()
        )
