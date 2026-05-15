#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import wpimath

SHOT_VELOCITY = 200.0  # rpm
TOLERANCE = 8.0  # rpm


class MyRobot(wpilib.TimedRobot):
    def __init__(self) -> None:
        super().__init__()

        self.shooter = wpilib.PWMSparkMax(0)
        self.shooterEncoder = wpilib.Encoder(0, 1)
        self.controller = wpimath.PIDController(0.3, 0, 0)
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(0.1, 0.065)

        self.kicker = wpilib.PWMSparkMax(1)

        self.intake = wpilib.PWMSparkMax(2)

        self.loop = wpilib.EventLoop()
        self.joystick = wpilib.Joystick(0)

        # Called once at the beginning of the robot program.
        self.controller.setTolerance(TOLERANCE)

        isBallAtKickerEvent = wpilib.BooleanEvent(
            self.loop, lambda: False
        )  # self.kickerSensor.getRange() < KICKER_THRESHOLD
        intakeButton = wpilib.BooleanEvent(
            self.loop, lambda: self.joystick.getRawButton(2)
        )

        # if the thumb button is held
        intakeButton.and_(isBallAtKickerEvent.negate()).ifHigh(
            # and there is not a ball at the kicker
            # activate the intake
            lambda: self.intake.set(0.5)
        )

        # if the thumb button is not held
        intakeButton.negate().or_(isBallAtKickerEvent).ifHigh(
            # or there is a ball in the kicker
            # stop the intake
            self.intake.stopMotor
        )

        shootTrigger = wpilib.BooleanEvent(self.loop, self.joystick.getTrigger)

        # if the trigger is held
        shootTrigger.ifHigh(
            # accelerate the shooter wheel
            lambda: self.shooter.setVoltage(
                self.controller.calculate(self.shooterEncoder.getRate(), SHOT_VELOCITY)
                + self.feedforward.calculate(SHOT_VELOCITY)
            )
        )

        # if not, stop
        shootTrigger.negate().ifHigh(self.shooter.stopMotor)

        atTargetVelocity = wpilib.BooleanEvent(
            self.loop, self.controller.atSetpoint
        ).debounce(
            # debounce for more stability
            0.2
        )

        # if we're at the target velocity, kick the ball into the shooter wheel
        atTargetVelocity.ifHigh(lambda: self.kicker.set(0.7))

        # when we stop being at the target velocity, it means the ball was shot
        atTargetVelocity.falling().ifHigh(
            # so stop the kicker
            self.kicker.stopMotor
        )

    def robotPeriodic(self) -> None:
        # poll all the bindings
        self.loop.poll()
