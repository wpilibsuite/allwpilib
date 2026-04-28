#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import wpimath.units

kMotorPort = 0
kEncoderAChannel = 0
kEncoderBChannel = 1
kJoystickPort = 0

kElevatorKp = 5.0
kElevatorKi = 0.0
kElevatorKd = 0.0

kElevatorkS = 0.0  # volts (V)
kElevatorkG = 0.762  # volts (V)
kElevatorkV = 0.762  # volt per velocity (V/(m/s))
kElevatorkA = 0.0  # volt per acceleration (V/(m/s^2))

kElevatorGearing = 10.0
kElevatorDrumRadius = wpimath.units.inchesToMeters(2.0)
kCarriageMass = 4.0  # kg

kSetpoint = 0.75  # m
# Encoder is reset to measure 0 at the bottom, so minimum height is 0.
kMinElevatorHeight = 0.0  # m
kMaxElevatorHeight = 1.25  # m

# distance per pulse = (distance per revolution) / (pulses per revolution)
#  = (Pi * D) / ppr
kElevatorEncoderDistPerPulse = 2.0 * math.pi * kElevatorDrumRadius / 4096
