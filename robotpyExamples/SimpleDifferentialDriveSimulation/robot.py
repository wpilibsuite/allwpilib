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
        self.velocityLimiter = wpimath.SlewRateLimiter(3)
        self.rotLimiter = wpimath.SlewRateLimiter(3)

        self.drive = Drivetrain()
        self.feedback = wpimath.LTVUnicycleController(0.020)
        self.timer = wpilib.Timer()

        # Called once at the beginning of the robot program.
        self.trajectory = wpimath.TrajectoryGenerator.generateTrajectory(
            wpimath.Pose2d(2, 2, wpimath.Rotation2d()),
            [],
            wpimath.Pose2d(6, 4, wpimath.Rotation2d()),
            wpimath.TrajectoryConfig(2, 2),
        )

    def robotPeriodic(self) -> None:
        self.drive.periodic()

    def autonomousInit(self) -> None:
        self.timer.restart()
        self.drive.resetOdometry(self.trajectory.initialPose())

    def autonomousPeriodic(self) -> None:
        elapsed = self.timer.get()
        reference = self.trajectory.sample(elapsed)
        velocities = self.feedback.calculate(self.drive.getPose(), reference)
        self.drive.drive(velocities.vx, velocities.omega)

    def teleopPeriodic(self) -> None:
        # Get the x velocity. We are inverting this because Xbox controllers return
        # negative values when we push forward.
        xVelocity = (
            -self.velocityLimiter.calculate(self.controller.getLeftY())
            * Drivetrain.kMaxVelocity
        )

        # Get the rate of angular rotation. We are inverting this because we want a
        # positive value when we pull to the left (remember, CCW is positive in
        # mathematics). Xbox controllers return positive values when you pull to
        # the right by default.
        rot = (
            -self.rotLimiter.calculate(self.controller.getRightX())
            * Drivetrain.kMaxAngularVelocity
        )
        self.drive.drive(xVelocity, rot)

    def simulationPeriodic(self) -> None:
        self.drive.simulationPeriodic()
