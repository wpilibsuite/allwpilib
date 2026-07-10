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
        self.shooter_encoder = wpilib.Encoder(0, 1)
        self.controller = wpimath.PIDController(0.3, 0, 0)
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(0.1, 0.065)

        self.kicker = wpilib.PWMSparkMax(1)

        self.intake = wpilib.PWMSparkMax(2)

        self.loop = wpilib.EventLoop()
        self.joystick = wpilib.Joystick(0)

        # Called once at the beginning of the robot program.
        self.controller.set_tolerance(TOLERANCE)

        is_ball_at_kicker_event = wpilib.BooleanEvent(
            self.loop, lambda: False
        )  # self.kicker_sensor.get_range() < KICKER_THRESHOLD
        intake_button = wpilib.BooleanEvent(
            self.loop, lambda: self.joystick.get_raw_button(2)
        )

        # if the thumb button is held
        intake_button.and_(is_ball_at_kicker_event.negate()).if_high(
            # and there is not a ball at the kicker
            # activate the intake
            lambda: self.intake.set(0.5)
        )

        # if the thumb button is not held
        intake_button.negate().or_(is_ball_at_kicker_event).if_high(
            # or there is a ball in the kicker
            # stop the intake
            self.intake.stop_motor
        )

        shoot_trigger = wpilib.BooleanEvent(self.loop, self.joystick.get_trigger)

        # if the trigger is held
        shoot_trigger.if_high(
            # accelerate the shooter wheel
            lambda: self.shooter.set_voltage(
                self.controller.calculate(
                    self.shooter_encoder.get_rate(), SHOT_VELOCITY
                )
                + self.feedforward.calculate(SHOT_VELOCITY)
            )
        )

        # if not, stop
        shoot_trigger.negate().if_high(self.shooter.stop_motor)

        at_target_velocity = wpilib.BooleanEvent(
            self.loop, self.controller.at_setpoint
        ).debounce(
            # debounce for more stability
            0.2
        )

        # if we're at the target velocity, kick the ball into the shooter wheel
        at_target_velocity.if_high(lambda: self.kicker.set(0.7))

        # when we stop being at the target velocity, it means the ball was shot
        at_target_velocity.falling().if_high(
            # so stop the kicker
            self.kicker.stop_motor
        )

    def robot_periodic(self) -> None:
        # poll all the bindings
        self.loop.poll()
