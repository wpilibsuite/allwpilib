#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import wpimath

from drivetrain import Drivetrain


class MyRobot(wpilib.TimedRobot):
    def __init__(self) -> None:
        super().__init__()

        self.controller = wpilib.NiDsXboxController(0)
        self.mecanum = Drivetrain()

        # Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0 to 1.
        self.x_velocity_limiter = wpimath.SlewRateLimiter(3)
        self.y_velocity_limiter = wpimath.SlewRateLimiter(3)
        self.rot_limiter = wpimath.SlewRateLimiter(3)

    def autonomous_periodic(self) -> None:
        self.drive_with_joystick(False)
        self.mecanum.update_odometry()

    def teleop_periodic(self) -> None:
        self.drive_with_joystick(True)

    def drive_with_joystick(self, field_relative: bool) -> None:
        # Get the x velocity. We are inverting this because Xbox controllers return
        # negative values when we push forward.
        x_velocity = -self.x_velocity_limiter.calculate(self.controller.get_left_y())
        x_velocity *= Drivetrain.MAX_VELOCITY

        # Get the y velocity or sideways/strafe velocity. We are inverting this because
        # we want a positive value when we pull to the left. Xbox controllers
        # return positive values when you pull to the right by default.
        y_velocity = -self.y_velocity_limiter.calculate(self.controller.get_left_x())
        y_velocity *= Drivetrain.MAX_VELOCITY

        # Get the rate of angular rotation. We are inverting this because we want a
        # positive value when we pull to the left (remember, CCW is positive in
        # mathematics). Xbox controllers return positive values when you pull to
        # the right by default.
        rot = -self.rot_limiter.calculate(self.controller.get_right_x())
        rot *= Drivetrain.MAX_ANGULAR_VELOCITY

        self.mecanum.drive(
            x_velocity, y_velocity, rot, field_relative, self.get_period()
        )
