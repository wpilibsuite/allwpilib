#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import commands2
import wpilib
import wpiutil
import xrp


class Drivetrain(commands2.Subsystem):
    GEAR_RATIO = (30.0 / 14.0) * (28.0 / 16.0) * (36.0 / 9.0) * (26.0 / 8.0)  #  48.75:1
    COUNTS_PER_MOTOR_SHAFT_REV = 12.0
    COUNTS_PER_REVOLUTION = COUNTS_PER_MOTOR_SHAFT_REV * GEAR_RATIO  # 585.0
    WHEEL_DIAMETER_INCH = 2.3622  # 60 mm

    def __init__(self) -> None:

        # The XRP has the left and right motors set to
        # PWM channels 0 and 1 respectively
        self.left_motor = xrp.XRPMotor(0)
        self.right_motor = xrp.XRPMotor(1)

        # The XRP has onboard encoders that are hardcoded
        # to use DIO pins 4/5 and 6/7 for the left and right
        self.left_encoder = wpilib.Encoder(4, 5)
        self.right_encoder = wpilib.Encoder(6, 7)

        # Set up the differential drive controller
        self.drive = wpilib.DifferentialDrive(
            self.left_motor.set_throttle, self.right_motor.set_throttle
        )

        # TODO: these don't work
        # wpiutil.SendableRegistry.addChild(self.drive, self.leftMotor)
        # wpiutil.SendableRegistry.addChild(self.drive, self.rightMotor)

        # Set up the XRPGyro
        self.gyro = xrp.XRPGyro()

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.right_motor.set_inverted(True)

        # Use inches as unit for encoder distances
        self.left_encoder.set_distance_per_pulse(
            (math.pi * self.WHEEL_DIAMETER_INCH) / self.COUNTS_PER_REVOLUTION
        )
        self.right_encoder.set_distance_per_pulse(
            (math.pi * self.WHEEL_DIAMETER_INCH) / self.COUNTS_PER_REVOLUTION
        )
        self.reset_encoders()

    def arcade_drive(self, x_axis_velocity: float, z_axis_rotate: float) -> None:
        """
        Drives the robot using arcade controls.

        :param x_axis_velocity: the commanded forward movement
        :param z_axis_rotate: the commanded rotation
        """
        self.drive.arcade_drive(x_axis_velocity, z_axis_rotate)

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

    def periodic(self) -> None:
        """This method will be called once per scheduler run"""
