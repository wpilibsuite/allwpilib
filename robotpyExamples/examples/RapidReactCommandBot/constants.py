#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import wpimath.units


class DriveConstants:
    LEFT_MOTOR1_PORT = 0
    LEFT_MOTOR2_PORT = 1
    RIGHT_MOTOR1_PORT = 2
    RIGHT_MOTOR2_PORT = 3

    LEFT_ENCODER_PORTS = (0, 1)
    RIGHT_ENCODER_PORTS = (2, 3)
    LEFT_ENCODER_REVERSED = False
    RIGHT_ENCODER_REVERSED = True

    ENCODER_CPR = 1024
    WHEEL_DIAMETER = wpimath.units.inches_to_meters(6)
    ENCODER_DISTANCE_PER_PULSE = (WHEEL_DIAMETER * math.pi) / ENCODER_CPR

    # These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
    # These values MUST be determined either experimentally or theoretically for *your* robot's
    # drive. The SysId tool provides a convenient method for obtaining feedback and feedforward
    # values for your robot.
    TURN_P = 1.0
    TURN_I = 0.0
    TURN_D = 0.0

    TURN_TOLERANCE_DEG = 5.0
    TURN_RATE_TOLERANCE_DEG_PER_S = 10.0  # degrees per second

    MAX_TURN_RATE_DEG_PER_S = 100
    MAX_TURN_ACCELERATION_DEG_PER_S_SQUARED = 300

    S = 1.0  # V
    V = 0.8  # V/(deg/s)
    A = 0.15  # V/(deg/s^2)


class ShooterConstants:
    ENCODER_PORTS = (4, 5)
    ENCODER_REVERSED = False
    ENCODER_CPR = 1024
    # Distance units will be rotations
    ENCODER_DISTANCE_PER_PULSE = 1.0 / ENCODER_CPR

    SHOOTER_MOTOR_PORT = 4
    FEEDER_MOTOR_PORT = 5

    SHOOTER_FREE_RPS = 5300.0
    SHOOTER_TARGET_RPS = 4000.0
    SHOOTER_TOLERANCE_RPS = 50.0

    # These are not real PID gains, and will have to be tuned for your specific robot.
    P = 1.0

    # On a real robot the feedforward constants should be empirically determined; these are
    # reasonable guesses.
    S = 0.05  # V
    # Should have value 12V at free velocity
    V = 12.0 / SHOOTER_FREE_RPS  # V/(rot/s)

    FEEDER_VELOCITY = 0.5


class IntakeConstants:
    MOTOR_PORT = 6
    SOLENOID_PORTS = (2, 3)


class StorageConstants:
    MOTOR_PORT = 7
    BALL_SENSOR_PORT = 6


class AutoConstants:
    TIMEOUT = 3
    DRIVE_DISTANCE = 2.0  # m
    DRIVE_VELOCITY = 0.5


class OIConstants:
    DRIVER_CONTROLLER_PORT = 0
