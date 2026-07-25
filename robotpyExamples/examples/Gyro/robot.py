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

    ANGLE_SETPOINT = 0.0
    P = 0.005  # proportional turning constant

    LEFT_MOTOR_PORT = 0
    RIGHT_MOTOR_PORT = 1
    IMU_MOUNT_ORIENTATION = wpilib.OnboardIMU.MountOrientation.FLAT
    JOYSTICK_PORT = 0

    def __init__(self) -> None:
        """Robot initialization function"""
        super().__init__()

        self.left_drive = wpilib.PWMSparkMax(self.LEFT_MOTOR_PORT)
        self.right_drive = wpilib.PWMSparkMax(self.RIGHT_MOTOR_PORT)
        self.my_robot = wpilib.DifferentialDrive(self.left_drive, self.right_drive)
        self.imu = wpilib.OnboardIMU(self.IMU_MOUNT_ORIENTATION)
        self.joystick = wpilib.Joystick(self.JOYSTICK_PORT)

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.right_drive.set_inverted(True)

    def teleop_periodic(self) -> None:
        # The motor velocity is set from the joystick while the DifferentialDrive turning value is assigned
        # from the error between the setpoint and the gyro angle.
        turning_value = (
            self.ANGLE_SETPOINT - self.imu.get_rotation2d().degrees()
        ) * self.P
        self.my_robot.arcade_drive(-self.joystick.get_y(), -turning_value)
