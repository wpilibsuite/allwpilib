#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#


import wpilib


class MyRobot(wpilib.TimedRobot):
    """
    This is a sample program that uses mecanum drive with a gyro sensor to maintain rotation vectors
    in relation to the starting orientation of the robot (field-oriented controls).
    """

    kFrontLeftChannel = 0
    kRearLeftChannel = 1
    kFrontRightChannel = 2
    kRearRightChannel = 3
    kIMUMountOrientation = wpilib.OnboardIMU.MountOrientation.kFlat
    kJoystickPort = 0

    def __init__(self) -> None:
        """Robot initialization function"""
        super().__init__()

        self.imu = wpilib.OnboardIMU(self.kIMUMountOrientation)
        self.joystick = wpilib.Joystick(self.kJoystickPort)

        frontLeft = wpilib.PWMSparkMax(self.kFrontLeftChannel)
        rearLeft = wpilib.PWMSparkMax(self.kRearLeftChannel)
        frontRight = wpilib.PWMSparkMax(self.kFrontRightChannel)
        rearRight = wpilib.PWMSparkMax(self.kRearRightChannel)

        frontRight.setInverted(True)
        rearRight.setInverted(True)

        self.robotDrive = wpilib.MecanumDrive(
            frontLeft, rearLeft, frontRight, rearRight
        )

    def teleopPeriodic(self) -> None:
        self.robotDrive.driveCartesian(
            -self.joystick.getY(),
            -self.joystick.getX(),
            -self.joystick.getZ(),
            self.imu.getRotation2d(),
        )
