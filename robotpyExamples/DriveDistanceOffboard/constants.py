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
    kDt = 0.02
    kLeftMotor1Port = 0
    kLeftMotor2Port = 1
    kRightMotor1Port = 2
    kRightMotor2Port = 3

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

    kMaxSpeed = 3.0  # m/s
    kMaxAcceleration = 3.0  # m/s²


class OIConstants:
    kDriverControllerPort = 0
