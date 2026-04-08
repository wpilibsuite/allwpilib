#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

from wpimath import units


class Constants:
    kMotorPort = 0
    kEncoderAChannel = 0
    kEncoderBChannel = 1
    kJoystickPort = 0

    kArmPositionKey = "ArmPosition"
    kArmPKey = "ArmP"

    # The P gain for the PID controller that drives this arm.
    kDefaultArmKp = 50.0
    kDefaultArmSetpointDegrees = 75.0

    # distance per pulse = (angle per revolution) / (pulses per revolution)
    #  = (2 * PI rads) / (4096 pulses)
    kArmEncoderDistPerPulse = 2.0 * math.pi / 4096

    kArmReduction = 200
    kArmMass = 8.0  # Kilograms
    kArmLength = units.inchesToMeters(30)
    kMinAngleRads = units.degreesToRadians(-75)
    kMaxAngleRads = units.degreesToRadians(255)
