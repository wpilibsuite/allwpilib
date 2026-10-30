#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math
from commands2 import Subsystem
from wpilib import DifferentialDrive
from wpilib import Encoder
from xrp import XRPMotor

K_GEAR_RATIO = (30.0 / 14.0) * (28.0 / 16.0) * (36.0 / 9.0) * (26.0 / 8.0) # 48.75 : 1
K_COUNTS_PER_MOTOR_SHAFT_REV = 12.0
K_COUNTS_PER_REVOLUTION =  K_COUNTS_PER_MOTOR_SHAFT_REV * K_GEAR_RATIO # 585.0
K_WHEEL_DIAMETER_INCH = 2.3622 # 60 mm

class XRPDrivetrain(Subsystem):
    def __init__(self) -> None:
        super().__init__()
        # This XRP has the left and right motors set to 
        # channels 0 and 1 respectively
        self.left_motor = XRPMotor(0)
        self.right_motor = XRPMotor(1)

        # The XRP has onboard encoders that are hardcoded
        # to use DIO pins 4/5 and 6/7 for the let and right
        self.left_encoder = Encoder(4, 5)
        self.right_encoder = Encoder(6, 7)

        # Use inches as unit for encoder distances
        self.left_encoder.setDistancePerPulse((math.pi * K_WHEEL_DIAMETER_INCH) / K_COUNTS_PER_REVOLUTION)
        self.right_encoder.setDistancePerPulse((math.pi * K_WHEEL_DIAMETER_INCH) / K_COUNTS_PER_REVOLUTION)
        self.left_encoder.reset()
        self.right_encoder.reset()

        # Invert right side since motor is flipped
        self.right_motor.setInverted(True)

        # Set up the differential drive controller
        self.diff_drive = DifferentialDrive(self.left_motor, self.right_motor)

    def arcadeDrive(self, xaxisVelocity: float, zaxisRotate: float) -> None:
        self.diff_drive.arcadeDrive(xaxisVelocity, zaxisRotate)

    def resetEncoders(self) -> None:
        self.left_encoder.reset()
        self.right_encoder.reset()

    def getLeftDistanceInch(self) -> float:
        return self.left_encoder.getDistance()

    def getRightDistanceInch(self) -> float:
        return self.right_encoder.getDistance()

