#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

"""
A place for the constant values in the code that may be used in more than one place.
This offers a convenient resources to teams who need to make both quick and universal
changes.
"""

import math


class DriveConstants:
    DT = 0.02
    LEFT_MOTOR1_PORT = 0
    LEFT_MOTOR2_PORT = 1
    RIGHT_MOTOR1_PORT = 2
    RIGHT_MOTOR2_PORT = 3

    """
    These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
    These characterization values MUST be determined either experimentally or theoretically
    for *your* robot's drive.
    The SysId tool provides a convenient method for obtaining these values for your robot.
    """
    ks = 1.0  # V
    kv = 0.8  # V/(m/s)
    ka = 0.15  # V/(m/s²)

    kp = 1.0

    MAX_VELOCITY = 3.0  # m/s
    MAX_ACCELERATION = 3.0  # m/s²


class OIConstants:
    DRIVER_CONTROLLER_PORT = 0
