#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import wpilib
import wpimath.units

MOTOR_PORT = 0
ENCODER_A_CHANNEL = 0
ENCODER_B_CHANNEL = 1
JOYSTICK_PORT = 0

SPIN_UP_RAD_PER_SEC = 500.0
FLYWHEEL_MOMENT_OF_INERTIA = 0.00032  # kg/m^2

# Reduction between motors and encoder, as output over input. If the flywheel spins slower than
# the motors, this number should be greater than one.
FLYWHEEL_GEARING = 1.0


class MyRobot(wpilib.TimedRobot):
    """
    This is a sample program to demonstrate how to use a state-space controller to control a
    flywheel.
    """

    def __init__(self) -> None:
        super().__init__()

        self.SPIN_UP_RAD_PER_SEC = (
            wpimath.units.rotations_per_minute_to_radians_per_second(500)
        )

        # The plant holds a state-space model of our flywheel. This system has the following properties:
        #
        # States: [velocity], in radians per second.
        # Inputs (what we can "put in"): [voltage], in volts.
        # Outputs (what we can measure): [velocity], in radians per second.
        self.flywheel_plant = wpimath.Models.flywheel_from_physical_constants(
            wpimath.DCMotor.neo(2),
            FLYWHEEL_MOMENT_OF_INERTIA,
            FLYWHEEL_GEARING,
        )

        # The observer fuses our encoder data and voltage inputs to reject noise.
        self.observer = wpimath.KalmanFilter_1_1_1(
            self.flywheel_plant,
            (3,),  # How accurate we think our model is
            (0.01,),  # How accurate we think our encoder data is
            0.020,
        )

        # A LQR uses feedback to create voltage commands.
        self.controller = wpimath.LinearQuadraticRegulator_1_1(
            self.flywheel_plant,
            # qelms. Velocity error tolerance, in radians per second. Decrease
            # this to more heavily penalize state excursion, or make the controller behave more
            # aggressively.
            (8,),
            # relms. Control effort (voltage) tolerance. Decrease this to more
            # heavily penalize control effort, or make the controller less aggressive. 12 is a good
            # starting point because that is the (approximate) maximum voltage of a battery.
            (12,),
            # Nominal time between loops. 0.020 for TimedRobot, but can be lower if using notifiers.
            0.020,
        )

        # The state-space loop combines a controller, observer, feedforward and plant for easy control.
        self.loop = wpimath.LinearSystemLoop_1_1_1(
            self.flywheel_plant, self.controller, self.observer, 12.0, 0.020
        )

        # An encoder set up to measure flywheel velocity in radians per second.
        self.encoder = wpilib.Encoder(ENCODER_A_CHANNEL, ENCODER_B_CHANNEL)

        self.motor = wpilib.PWMSparkMax(MOTOR_PORT)

        # A joystick to read the trigger from.
        self.joystick = wpilib.Joystick(JOYSTICK_PORT)

        # We go 2 pi radians per 4096 clicks.
        self.encoder.set_distance_per_pulse(math.tau / 4096)

    def teleop_init(self) -> None:
        self.loop.reset([self.encoder.get_rate()])

    def teleop_periodic(self) -> None:

        # Sets the target velocity of our flywheel. This is similar to setting the setpoint of a
        # PID controller.
        if self.joystick.get_trigger_pressed():
            # We just pressed the trigger, so let's set our next reference
            self.loop.set_next_r([SPIN_UP_RAD_PER_SEC])

        elif self.joystick.get_trigger_released():
            # We just released the trigger, so let's spin down
            self.loop.set_next_r([0.0])

        # Correct our Kalman filter's state vector estimate with encoder data.
        self.loop.correct([self.encoder.get_rate()])

        # Update our LQR to generate new voltage commands and use the voltages to predict the next
        # state with out Kalman filter.
        self.loop.predict(0.020)

        # Send the new calculated voltage to the motors.
        # voltage = duty cycle * battery voltage, so
        # duty cycle = voltage / battery voltage
        next_voltage = self.loop.u(0)
        self.motor.set_voltage(next_voltage)
