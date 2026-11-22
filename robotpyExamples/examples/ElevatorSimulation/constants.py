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

ELEVATOR_KP = 5.0
ELEVATOR_KI = 0.0
ELEVATOR_KD = 0.0

ELEVATOR_KS = 0.0  # volts (V)
ELEVATOR_KG = 0.762  # volts (V)
ELEVATOR_KV = 0.762  # volt per velocity (V/(m/s))
ELEVATOR_KA = 0.0  # volt per acceleration (V/(m/s^2))

ELEVATOR_GEARING = 10.0
ELEVATOR_DRUM_RADIUS = wpimath.units.inches_to_meters(2.0)
CARRIAGE_MASS = 4.0  # kg

SETPOINT = 0.75  # m
# Encoder is reset to measure 0 at the bottom, so minimum height is 0.
MIN_ELEVATOR_HEIGHT = 0.0  # m
MAX_ELEVATOR_HEIGHT = 1.25  # m

# distance per pulse = (distance per revolution) / (pulses per revolution)
#  = (Pi * D) / ppr
ELEVATOR_ENCODER_DIST_PER_PULSE = 2.0 * math.pi * ELEVATOR_DRUM_RADIUS / 4096
