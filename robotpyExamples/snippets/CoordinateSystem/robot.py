#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import wpimath


class MyRobot(wpilib.TimedRobot):
    """Coordinate system changing using Alliance Color snippets for wpilib-docs. https://docs.wpilib.org/en/stable/docs/software/basic-programming/coordinate-system.html"""

    # Applies driver input to mecanum drive using a field coordinate system that changes based on alliance color.
    @staticmethod
    def drive_alliance_relative_mecanum(
        robot_drive, x_speed, y_speed, z_rotation, gyro_angle
    ):
        # The origin is always blue. When our alliance is red, X and Y need to be inverted
        invert = 1
        if wpilib.MatchState.get_alliance() == wpilib.Alliance.RED:
            invert = -1

        robot_drive.drive_cartesian(
            x_speed * invert, y_speed * invert, z_rotation, gyro_angle
        )

    # Applies driver input to swerve using a field coordinate system that changes based on alliance color.
    @staticmethod
    def drive_alliance_relative_swerve(x_speed, y_speed, z_rotation, gyro_angle):
        # The origin is always blue. When our alliance is red, X and Y need to be inverted
        invert = 1
        if wpilib.MatchState.get_alliance() == wpilib.Alliance.RED:
            invert = -1

        # Create field-relative ChassisVelocities for controlling Swerve
        chassis_velocities = wpimath.ChassisVelocities(
            x_speed * invert, y_speed * invert, z_rotation
        ).to_robot_relative(gyro_angle)

        return chassis_velocities
