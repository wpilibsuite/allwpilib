#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import wpimath.units


class DriveConstants:
    kLeftMotor1Port = 0
    kLeftMotor2Port = 1
    kRightMotor1Port = 2
    kRightMotor2Port = 3

    kLeftEncoderPorts = (0, 1)
    kRightEncoderPorts = (2, 3)
    kLeftEncoderReversed = False
    kRightEncoderReversed = True

    kEncoderCPR = 1024
    kWheelDiameter = wpimath.units.inchesToMeters(6)
    kEncoderDistancePerPulse = (kWheelDiameter * math.pi) / kEncoderCPR

    # These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
    # These values MUST be determined either experimentally or theoretically for *your* robot's
    # drive. The SysId tool provides a convenient method for obtaining feedback and feedforward
    # values for your robot.
    kTurnP = 1.0
    kTurnI = 0.0
    kTurnD = 0.0

    kTurnToleranceDeg = 5.0
    kTurnRateToleranceDegPerS = 10.0  # degrees per second

    kMaxTurnRateDegPerS = 100
    kMaxTurnAccelerationDegPerSSquared = 300

    kS = 1.0  # V
    kV = 0.8  # V/(deg/s)
    kA = 0.15  # V/(deg/s^2)


class ShooterConstants:
    kEncoderPorts = (4, 5)
    kEncoderReversed = False
    kEncoderCPR = 1024
    # Distance units will be rotations
    kEncoderDistancePerPulse = 1.0 / kEncoderCPR

    kShooterMotorPort = 4
    kFeederMotorPort = 5

    kShooterFreeRPS = 5300.0
    kShooterTargetRPS = 4000.0
    kShooterToleranceRPS = 50.0

    # These are not real PID gains, and will have to be tuned for your specific robot.
    kP = 1.0

    # On a real robot the feedforward constants should be empirically determined; these are
    # reasonable guesses.
    kS = 0.05  # V
    # Should have value 12V at free speed
    kV = 12.0 / kShooterFreeRPS  # V/(rot/s)

    kFeederSpeed = 0.5


class IntakeConstants:
    kMotorPort = 6
    kSolenoidPorts = (2, 3)


class StorageConstants:
    kMotorPort = 7
    kBallSensorPort = 6


class AutoConstants:
    kTimeout = 3
    kDriveDistance = 2.0  # m
    kDriveSpeed = 0.5


class OIConstants:
    kDriverControllerPort = 0
