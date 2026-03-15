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

        self.inst = ntcore.NetworkTableInstance.getDefault()
        self.doubleArrayTopic = self.inst.getDoubleArrayTopic("m_doubleArrayTopic")

        self.controller = wpilib.NiDsXboxController(0)
        self.drive = Drivetrain(self.doubleArrayTopic)

        # Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0 to 1.
        self.velocityLimiter = wpimath.SlewRateLimiter(3)
        self.rotLimiter = wpimath.SlewRateLimiter(3)

    def autonomousPeriodic(self) -> None:
        self.teleopPeriodic()
        self.drive.updateOdometry()

    def simulationPeriodic(self) -> None:
        self.drive.simulationPeriodic()

    def robotPeriodic(self) -> None:
        self.drive.periodic()

    def teleopPeriodic(self) -> None:
        # Get the x velocity. We are inverting this because Xbox controllers return
        # negative values when we push forward.
        xVelocity = -self.velocityLimiter.calculate(self.controller.getLeftY())
        xVelocity *= Drivetrain.kMaxVelocity

        # Get the rate of angular rotation. We are inverting this because we want a
        # positive value when we pull to the left (remember, CCW is positive in
        # mathematics). Xbox controllers return positive values when you pull to
        # the right by default.
        rot = -self.rotLimiter.calculate(self.controller.getRightX())
        rot *= Drivetrain.kMaxAngularVelocity

        self.drive.drive(xVelocity, rot)
