#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math
import wpilib
import wpimath.units
import wpimath

MOTOR_PORT = 0
ENCODER_A_CHANNEL = 0
ENCODER_B_CHANNEL = 1
JOYSTICK_PORT = 0
RAISED_POSITION = wpimath.units.degrees_to_radians(90.0)
LOWERED_POSITION = wpimath.units.degrees_to_radians(0.0)

# Moment of inertia of the arm, in kg * m^2. Can be estimated with CAD. If finding this constant
# is difficult, LinearSystem.identifyPositionSystem may be better.
ARM_MOI = 1.2

# Reduction between motors and encoder, as output over input. If the arm spins slower than
# the motors, this number should be greater than one.
ARM_GEARING = 10.0


class MyRobot(wpilib.TimedRobot):
    """This is a sample program to demonstrate how to use a state-space controller to control an arm."""

    def __init__(self) -> None:
        super().__init__()

        self.profile = wpimath.TrapezoidProfile(
            wpimath.TrapezoidProfile.Constraints(
                wpimath.units.degrees_to_radians(45),
                wpimath.units.degrees_to_radians(
                    90
                ),  # Max arm velocity and acceleration.
            )
        )

        self.last_profiled_reference = wpimath.TrapezoidProfile.State()

        # The plant holds a state-space model of our arm. This system has the following properties:
        #
        # States: [position, velocity], in radians and radians per second.
        # Inputs (what we can "put in"): [voltage], in volts.
        # Outputs (what we can measure): [position], in radians.
        self.arm_plant = wpimath.Models.single_jointed_arm_from_physical_constants(
            wpimath.DCMotor.neo(2),
            ARM_MOI,
            ARM_GEARING,
        ).slice(0)

        # The observer fuses our encoder data and voltage inputs to reject noise.
        self.observer = wpimath.KalmanFilter_2_1_1(
            self.arm_plant,
            # How accurate we think our model is, in radians and radians/sec.
            (
                0.015,
                0.17,
            ),
            # How accurate we think our encoder position data is. In this case we very highly trust our encoder position reading.
            (0.01,),
            0.020,
        )

        # A LQR uses feedback to create voltage commands.
        self.controller = wpimath.LinearQuadraticRegulator_2_1(
            self.arm_plant,
            # qelms. Velocity error tolerance, in radians and radians per second.
            # Decrease this to more heavily penalize state excursion, or make the
            # controller behave more aggressively.
            (
                wpimath.units.degrees_to_radians(1.0),
                wpimath.units.degrees_to_radians(10.0),
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
            self.arm_plant, self.controller, self.observer, 12.0, 0.020
        )

        # An encoder set up to measure flywheel velocity in radians per second.
        self.encoder = wpilib.Encoder(ENCODER_A_CHANNEL, ENCODER_B_CHANNEL)

        self.motor = wpilib.PWMSparkMax(MOTOR_PORT)

        # A joystick to read the trigger from.
        self.joystick = wpilib.Joystick(JOYSTICK_PORT)

        # We go 2 pi radians in 1 rotation, or 4096 counts.
        self.encoder.set_distance_per_pulse(math.tau / 4096)

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
            goal = wpimath.TrapezoidProfile.State(RAISED_POSITION, 0.0)

        else:
            # Otherwise, we go to the low goal
            goal = wpimath.TrapezoidProfile.State(LOWERED_POSITION, 0.0)

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
