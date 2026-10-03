#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import wpimath.units

MOTOR_PORT = 0
ENCODER_A_CHANNEL = 0
ENCODER_B_CHANNEL = 1
JOYSTICK_PORT = 0

ELEVATOR_KP = 0.75
ELEVATOR_KI = 0.0
ELEVATOR_KD = 0.0

ELEVATOR_MAX_V = 10.0  # volts (V)
ELEVATOR_KS = 0.0  # volts (V)
ELEVATOR_KG = 0.62  # volts (V)
ELEVATOR_KV = 3.9  # volts (V)
ELEVATOR_KA = 0.06  # volts (V)

ELEVATOR_GEARING = 5.0
ELEVATOR_DRUM_RADIUS = wpimath.units.inches_to_meters(1.0)
CARRIAGE_MASS = wpimath.units.lbs_to_kilograms(12)  # kg

SETPOINT = wpimath.units.inches_to_meters(42.875)
LOWERK_SETPOINT = wpimath.units.inches_to_meters(15)
# Encoder is reset to measure 0 at the bottom, so minimum height is 0.
MIN_ELEVATOR_HEIGHT = 0.0  # m
MAX_ELEVATOR_HEIGHT = wpimath.units.inches_to_meters(50)

# distance per pulse = (distance per revolution) / (pulses per revolution)
#  = (Pi * D) / ppr
ELEVATOR_ENCODER_DIST_PER_PULSE = 2.0 * math.pi * ELEVATOR_DRUM_RADIUS / 4096
