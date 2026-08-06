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

        # Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0
        # to 1.
        self.velocity_limiter = wpimath.SlewRateLimiter(3)
        self.rot_limiter = wpimath.SlewRateLimiter(3)

        self.drive = Drivetrain()
        self.feedback = wpimath.LTVUnicycleController(0.020)
        self.timer = wpilib.Timer()

        # Called once at the beginning of the robot program.
        self.trajectory = wpimath.DrivetrainSplineTrajectoryGenerator.generate(
            wpimath.Pose2d(2, 2, wpimath.Rotation2d()),
            [],
            wpimath.Pose2d(6, 4, wpimath.Rotation2d()),
            wpimath.TrajectoryConfig(2, 2),
        )

    def robot_periodic(self) -> None:
        self.drive.periodic()

    def autonomous_enter(self) -> None:
        self.timer.restart()
        self.drive.reset_odometry(self.trajectory.initial_pose())

    def autonomous_periodic(self) -> None:
        elapsed = self.timer.get()
        reference = self.trajectory.sample_at(elapsed)
        velocities = self.feedback.calculate(self.drive.get_pose(), reference)
        self.drive.drive(velocities.vx, velocities.omega)

    def teleop_periodic(self) -> None:
        # Get the x velocity. We are inverting this because Xbox controllers return
        # negative values when we push forward.
        x_velocity = (
            -self.velocity_limiter.calculate(self.controller.get_left_y())
            * Drivetrain.MAX_VELOCITY
        )

        # Get the rate of angular rotation. We are inverting this because we want a
        # positive value when we pull to the left (remember, CCW is positive in
        # mathematics). Xbox controllers return positive values when you pull to
        # the right by default.
        rot = (
            -self.rot_limiter.calculate(self.controller.get_right_x())
            * Drivetrain.MAX_ANGULAR_VELOCITY
        )
        self.drive.drive(x_velocity, rot)

    def simulation_periodic(self) -> None:
        self.drive.simulation_periodic()
