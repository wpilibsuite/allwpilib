#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import commands2
import wpilib
import romi


class Drivetrain(commands2.Subsystem):
    COUNTS_PER_REVOLUTION = 1440.0
    WHEEL_DIAMETER_INCH = 2.75591

    def __init__(self) -> None:

        # The Romi has the left and right motors set to
        # PWM channels 0 and 1 respectively
        self.left_motor = wpilib.Spark(0)
        self.right_motor = wpilib.Spark(1)

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.right_motor.set_inverted(True)

        # The Romi has onboard encoders that are hardcoded
        # to use DIO pins 4/5 and 6/7 for the left and right
        self.left_encoder = wpilib.Encoder(4, 5)
        self.right_encoder = wpilib.Encoder(6, 7)

        # Set up the differential drive controller
        self.drive = wpilib.DifferentialDrive(self.left_motor, self.right_motor)

        # Set up the RomiGyro
        self.gyro = romi.RomiGyro()

        # Use inches as unit for encoder distances
        self.left_encoder.set_distance_per_pulse(
            (math.pi * self.WHEEL_DIAMETER_INCH) / self.COUNTS_PER_REVOLUTION
        )
        self.right_encoder.set_distance_per_pulse(
            (math.pi * self.WHEEL_DIAMETER_INCH) / self.COUNTS_PER_REVOLUTION
        )
        self.reset_encoders()

    def arcade_drive(self, fwd: float, rot: float) -> None:
        """
        Drives the robot using arcade controls.

        :param fwd: the commanded forward movement
        :param rot: the commanded rotation
        """
        self.drive.arcade_drive(fwd, rot)

    def reset_encoders(self) -> None:
        """Resets the drive encoders to currently read a position of 0."""
        self.left_encoder.reset()
        self.right_encoder.reset()

    def get_left_encoder_count(self) -> int:
        return self.left_encoder.get()

    def get_right_encoder_count(self) -> int:
        return self.right_encoder.get()

    def get_left_distance_inch(self) -> float:
        return self.left_encoder.get_distance()

    def get_right_distance_inch(self) -> float:
        return self.right_encoder.get_distance()

    def get_average_distance_inch(self) -> float:
        """Gets the average distance of the TWO encoders."""
        return (self.get_left_distance_inch() + self.get_right_distance_inch()) / 2.0

    def get_gyro_angle_x(self) -> float:
        """Current angle of the Romi around the X-axis.

        :returns: The current angle of the Romi in degrees
        """
        return self.gyro.get_angle_x()

    def get_gyro_angle_y(self) -> float:
        """Current angle of the Romi around the Y-axis.

        :returns: The current angle of the Romi in degrees
        """
        return self.gyro.get_angle_y()

    def get_gyro_angle_z(self) -> float:
        """Current angle of the Romi around the Z-axis.

        :returns: The current angle of the Romi in degrees
        """
        return self.gyro.get_angle_z()

    def reset_gyro(self) -> None:
        """Reset the gyro"""
        self.gyro.reset()
