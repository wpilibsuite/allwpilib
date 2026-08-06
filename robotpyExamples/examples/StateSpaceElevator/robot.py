#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math
import wpilib
import wpimath
import wpimath.units

MOTOR_PORT = 0
ENCODER_A_CHANNEL = 0
ENCODER_B_CHANNEL = 1
JOYSTICK_PORT = 0
HIGH_GOAL_POSITION = wpimath.units.feet_to_meters(3)
LOW_GOAL_POSITION = wpimath.units.feet_to_meters(0)

CARRIAGE_MASS = 4.5
# kilograms

# A 1.5in diameter drum has a radius of 0.75in, or 0.019in.
DRUM_RADIUS = 1.5 / 2.0 * 25.4 / 1000.0

# Reduction between motors and encoder, as output over input. If the elevator spins slower than
# the motors, this number should be greater than one.
ELEVATOR_GEARING = 6.0


class MyRobot(wpilib.TimedRobot):
    """This is a sample program to demonstrate how to use a state-space controller to control an
    elevator.
    """

    def __init__(self) -> None:
        super().__init__()

        self.profile = wpimath.TrapezoidProfile(
            wpimath.TrapezoidProfile.Constraints(
                wpimath.units.feet_to_meters(3.0),
                wpimath.units.feet_to_meters(
                    6.0
                ),  # Max elevator velocity and acceleration.
            )
        )

        self.last_profiled_reference = wpimath.TrapezoidProfile.State()

        # The plant holds a state-space model of our elevator. This system has the following properties:

        # States: [position, velocity], in meters and meters per second.
        # Inputs (what we can "put in"): [voltage], in volts.
        # Outputs (what we can measure): [position], in meters.

        # This elevator is driven by two NEO motors.
        self.elevator_plant = wpimath.Models.elevator_from_physical_constants(
            wpimath.DCMotor.neo(2),
            CARRIAGE_MASS,
            DRUM_RADIUS,
            ELEVATOR_GEARING,
        ).slice(0)

        # The observer fuses our encoder data and voltage inputs to reject noise.
        self.observer = wpimath.KalmanFilter_2_1_1(
            self.elevator_plant,
            (
                wpimath.units.inches_to_meters(2),
                wpimath.units.inches_to_meters(40),
            ),  # How accurate we think our model is, in meters and meters/second.
            (
                0.001,
            ),  # How accurate we think our encoder position data is. In this case we very highly trust our encoder position reading.
            0.020,
        )

        # A LQR uses feedback to create voltage commands.
        self.controller = wpimath.LinearQuadraticRegulator_2_1(
            self.elevator_plant,
            # qelms. State error tolerance, in meters and meters per second.
            # Decrease this to more heavily penalize state excursion, or make the
            # controller behave more aggressively.
            (
                wpimath.units.inches_to_meters(1.0),
                wpimath.units.inches_to_meters(10.0),
            ),
            # relms. Control effort (voltage) tolerance. Decrease this to more
            # heavily penalize control effort, or make the controller less
            # aggressive. 12 is a good starting point because that is the
            # (approximate) maximum voltage of a battery.
            (12.0,),
            # Nominal time between loops. 20ms for TimedRobot, but can be lower if
            # using notifiers.
            0.020,
        )

        # The state-space loop combines a controller, observer, feedforward and plant for easy control.
        self.loop = wpimath.LinearSystemLoop_2_1_1(
            self.elevator_plant, self.controller, self.observer, 12.0, 0.020
        )

        # An encoder set up to measure elevator height in meters.
        self.encoder = wpilib.Encoder(ENCODER_A_CHANNEL, ENCODER_B_CHANNEL)

        self.motor = wpilib.PWMSparkMax(MOTOR_PORT)

        # A joystick to read the trigger from.
        self.joystick = wpilib.Joystick(JOYSTICK_PORT)

        # Circumference = pi * d, so distance per click = pi * d / counts
        self.encoder.set_distance_per_pulse(math.tau * DRUM_RADIUS / 4096)

    def teleop_enter(self) -> None:
        # Reset our loop to make sure it's in a known state.
        self.loop.reset([self.encoder.get_distance(), self.encoder.get_rate()])

        # Reset our last reference to the current state.
        self.last_profiled_reference = wpimath.TrapezoidProfile.State(
            self.encoder.get_distance(), self.encoder.get_rate()
        )

    def teleop_periodic(self) -> None:
        # Sets the target position of our arm. This is similar to setting the setpoint of a
        # PID controller.

        if self.joystick.get_trigger():
            # the trigger is pressed, so we go to the high goal.
            goal = wpimath.TrapezoidProfile.State(HIGH_GOAL_POSITION, 0.0)

        else:
            # Otherwise, we go to the low goal
            goal = wpimath.TrapezoidProfile.State(LOW_GOAL_POSITION, 0.0)

        # Step our TrapezoidalProfile forward 20ms and set it as our next reference
        self.last_profiled_reference = self.profile.calculate(
            0.020, self.last_profiled_reference, goal
        )
        self.loop.set_next_r(
            [
                self.last_profiled_reference.position,
                self.last_profiled_reference.velocity,
            ]
        )

        # Correct our Kalman filter's state vector estimate with encoder data.
        self.loop.correct([self.encoder.get_distance()])

        # Update our LQR to generate new voltage commands and use the voltages to predict the next
        # state with out Kalman filter.
        self.loop.predict(0.020)

        # Send the new calculated voltage to the motors.
        # voltage = duty cycle * battery voltage, so
        # duty cycle = voltage / battery voltage
        next_voltage = self.loop.u(0)
        self.motor.set_voltage(next_voltage)
