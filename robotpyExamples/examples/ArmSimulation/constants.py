#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

from wpimath import units


class Constants:
    MOTOR_PORT = 0
    ENCODER_A_CHANNEL = 0
    ENCODER_B_CHANNEL = 1
    JOYSTICK_PORT = 0

    ARM_POSITION_KEY = "ArmPosition"
    ARM_P_KEY = "ArmP"

    # The P gain for the PID controller that drives this arm.
    DEFAULT_ARM_KP = 50.0
    DEFAULT_ARM_SETPOINT_DEGREES = 75.0

    # distance per pulse = (angle per revolution) / (pulses per revolution)
    #  = (2 * PI rads) / (4096 pulses)
    ARM_ENCODER_DIST_PER_PULSE = 2.0 * math.pi / 4096

    ARM_REDUCTION = 200
    ARM_MASS = 8.0  # Kilograms
    ARM_LENGTH = units.inches_to_meters(30)
    MIN_ANGLE_RADS = units.degrees_to_radians(-75)
    MAX_ANGLE_RADS = units.degrees_to_radians(255)
