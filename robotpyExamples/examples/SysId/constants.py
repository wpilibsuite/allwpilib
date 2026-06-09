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
    # Assumes the encoders are directly mounted on the wheel shafts
    kEncoderDistancePerPulse = (kWheelDiameter * math.pi) / kEncoderCPR


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
    # Should have value 12V at free velocity
    kV = 12.0 / kShooterFreeRPS  # V/(rot/s)
    kA = 0.0  # V/(rot/s^2)

    kFeederVelocity = 0.5


class IntakeConstants:
    kMotorPort = 6
    kSolenoidPorts = (2, 3)


class StorageConstants:
    kMotorPort = 7
    kBallSensorPort = 6


class AutoConstants:
    kTimeout = 3
    kDriveDistance = 2.0  # m
    kDriveVelocity = 0.5


class OIConstants:
    kDriverControllerPort = 0
