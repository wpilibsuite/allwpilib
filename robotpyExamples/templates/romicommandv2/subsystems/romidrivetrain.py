#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2.subsystem import Subsystem
from wpilib import DifferentialDrive
from wpilib import Spark
from wpilib import Encoder
import math

K_COUNTS_PER_REVOLUTION = 1440.0
K_WHEEL_DIAMETER_INCH = 2.75591 # 70 mm

class RomiDrivetrain(Subsystem):
    # Creates a new RomiDrivetrain
    def __init__(self) -> None:
        self.left_motor = Spark(0)
        self.right_motor = Spark(1)

        # Use inches as distance for encoder distances
        self.left_encoder = Encoder(4, 5)
        self.left_encoder.setDistancePerPulse((math.pi * K_WHEEL_DIAMETER_INCH) / K_COUNTS_PER_REVOLUTION)
        self.left_encoder.reset()
        
        self.right_encoder = Encoder(6, 7)
        self.right_encoder.setDistancePerPulse((math.pi * K_WHEEL_DIAMETER_INCH) / K_COUNTS_PER_REVOLUTION)
        self.right_encoder.reset()

        # Invert right side since motor is flipped
        self.right_motor.setInverted(True)

        self.diff_drive = DifferentialDrive(leftMotor=self.left_motor, rightMotor=self.right_motor)

    def arcade_drive(self, xaxisVelocity: float, zaxisRotate: float):
        self.diff_drive.arcadeDrive(xaxisVelocity, zaxisRotate)

    def reset_encoders(self):
        self.left_encoder.reset()
        self.right_encoder.reset()

    def get_left_distance_inch(self):
        return self.left_encoder.getDistance()

    def get_right_distance_inch(self):
        return self.right_encoder.getDistance()
    
    def periodic(self):
        # This method will be called once per scheduler run

    def simulation_periodic(self):
        # This method will be called once per scheduler run during simulation

        