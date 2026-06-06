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

kElevatorKp = 0.75
kElevatorKi = 0.0
kElevatorKd = 0.0

kElevatorMaxV = 10.0  # volts (V)
kElevatorkS = 0.0  # volts (V)
kElevatorkG = 0.62  # volts (V)
kElevatorkV = 3.9  # volts (V)
kElevatorkA = 0.06  # volts (V)

kElevatorGearing = 5.0
kElevatorDrumRadius = wpimath.units.inchesToMeters(1.0)
kCarriageMass = wpimath.units.lbsToKilograms(12)  # kg

kSetpoint = wpimath.units.inchesToMeters(42.875)
kLowerkSetpoint = wpimath.units.inchesToMeters(15)
# Encoder is reset to measure 0 at the bottom, so minimum height is 0.
kMinElevatorHeight = 0.0  # m
kMaxElevatorHeight = wpimath.units.inchesToMeters(50)

# distance per pulse = (distance per revolution) / (pulses per revolution)
#  = (Pi * D) / ppr
kElevatorEncoderDistPerPulse = 2.0 * math.pi * kElevatorDrumRadius / 4096
