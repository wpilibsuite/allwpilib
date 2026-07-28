#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import ntcore
import wpilib
import wpimath

from drivetrain import Drivetrain


class MyRobot(wpilib.TimedRobot):
    def __init__(self) -> None:
        super().__init__()

        self.inst = ntcore.NetworkTableInstance.get_default()
        self.double_array_topic = self.inst.get_double_array_topic("doubleArrayTopic")

        self.controller = wpilib.NiDsXboxController(0)
        self.drive = Drivetrain(self.double_array_topic)

        # Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0 to 1.
        self.velocity_limiter = wpimath.SlewRateLimiter(3)
        self.rot_limiter = wpimath.SlewRateLimiter(3)

    def autonomous_periodic(self) -> None:
        self.teleop_periodic()
        self.drive.update_odometry()

    def simulation_periodic(self) -> None:
        self.drive.simulation_periodic()

    def robot_periodic(self) -> None:
        self.drive.periodic()

    def teleop_periodic(self) -> None:
        # Get the x velocity. We are inverting this because Xbox controllers return
        # negative values when we push forward.
        x_velocity = -self.velocity_limiter.calculate(self.controller.get_left_y())
        x_velocity *= Drivetrain.MAX_VELOCITY

        # Get the rate of angular rotation. We are inverting this because we want a
        # positive value when we pull to the left (remember, CCW is positive in
        # mathematics). Xbox controllers return positive values when you pull to
        # the right by default.
        rot = -self.rot_limiter.calculate(self.controller.get_right_x())
        rot *= Drivetrain.MAX_ANGULAR_VELOCITY

        self.drive.drive(x_velocity, rot)
