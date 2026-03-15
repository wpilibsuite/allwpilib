#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib


class MyRobot(wpilib.TimedRobot):
    """
    This is a sample program to demonstrate how to use a gyro sensor to make a robot drive straight.
    This program uses a joystick to drive forwards and backwards while the gyro is used for direction
    keeping.
    """

    kAngleSetpoint = 0.0
    kP = 0.005  # proportional turning constant

    kLeftMotorPort = 0
    kRightMotorPort = 1
    kIMUMountOrientation = wpilib.OnboardIMU.MountOrientation.kFlat
    kJoystickPort = 0

    def __init__(self) -> None:
        """Robot initialization function"""
        super().__init__()

        self.leftDrive = wpilib.PWMSparkMax(self.kLeftMotorPort)
        self.rightDrive = wpilib.PWMSparkMax(self.kRightMotorPort)
        self.myRobot = wpilib.DifferentialDrive(self.leftDrive, self.rightDrive)
        self.imu = wpilib.OnboardIMU(self.kIMUMountOrientation)
        self.joystick = wpilib.Joystick(self.kJoystickPort)

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.rightDrive.setInverted(True)

    def teleopPeriodic(self) -> None:
        # The motor velocity is set from the joystick while the DifferentialDrive turning value is assigned
        # from the error between the setpoint and the gyro angle.
        turningValue = (
            self.kAngleSetpoint - self.imu.getRotation2d().degrees()
        ) * self.kP
        self.myRobot.arcadeDrive(-self.joystick.getY(), -turningValue)
