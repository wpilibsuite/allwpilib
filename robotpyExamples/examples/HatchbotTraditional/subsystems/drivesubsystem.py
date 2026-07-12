#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2
import wpilib
import constants


class DriveSubsystem(commands2.Subsystem):
    def __init__(self) -> None:

        self.left1 = wpilib.PWMVictorSPX(constants.LEFT_MOTOR1_PORT)
        self.left2 = wpilib.PWMVictorSPX(constants.LEFT_MOTOR2_PORT)
        self.right1 = wpilib.PWMVictorSPX(constants.RIGHT_MOTOR1_PORT)
        self.right2 = wpilib.PWMVictorSPX(constants.RIGHT_MOTOR2_PORT)

        # We need to invert one side of the drivetrain so that positive velocities
        # result in both sides moving forward. Depending on how your robot's
        # drivetrain is constructed, you might have to invert the left side instead.
        self.right1.set_inverted(True)

        # The robot's drive
        self.drive = wpilib.DifferentialDrive(self.left1, self.right1)

        # The left-side drive encoder
        self.left_encoder = wpilib.Encoder(
            *constants.LEFT_ENCODER_PORTS,
            reverse_direction=constants.LEFT_ENCODER_REVERSED
        )

        # The right-side drive encoder
        self.right_encoder = wpilib.Encoder(
            *constants.RIGHT_ENCODER_PORTS,
            reverse_direction=constants.RIGHT_ENCODER_REVERSED
        )

        # Sets the distance per pulse for the encoders
        self.left_encoder.set_distance_per_pulse(constants.ENCODER_DISTANCE_PER_PULSE)
        self.right_encoder.set_distance_per_pulse(constants.ENCODER_DISTANCE_PER_PULSE)

    def arcade_drive(self, fwd: float, rot: float) -> None:
        """
        Drives the robot using arcade controls.

        :param fwd: the commanded forward movement
        :param rot: the commanded rotation
        """
        self.drive.arcade_drive(fwd, rot)

    def reset_encoders(self) -> None:
        """Resets the drive encoders to currently read a position of 0."""

    def get_average_encoder_distance(self) -> float:
        """Gets the average distance of the TWO encoders."""
        return (
            self.left_encoder.get_distance() + self.right_encoder.get_distance()
        ) / 2.0

    def set_max_output(self, max_output: float):
        """
        Sets the max output of the drive. Useful for scaling the
        drive to drive more slowly.
        """
        self.drive.set_max_output(max_output)
