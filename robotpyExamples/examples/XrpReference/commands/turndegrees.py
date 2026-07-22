#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math
import commands2

from subsystems.drivetrain import Drivetrain


class TurnDegrees(commands2.Command):
    def __init__(self, velocity: float, degrees: float, drive: Drivetrain) -> None:
        """Creates a new TurnDegrees. This command will turn your robot for a desired rotation (in
        degrees) and rotational velocity.

        :param velocity:   The velocity which the robot will drive. Negative is in reverse.
        :param degrees: Degrees to turn. Leverages encoders to compare distance.
        :param drive:   The drive subsystem on which this command will run
        """

        self.degrees = degrees
        self.velocity = velocity
        self.drive = drive
        self.add_requirements(drive)

    def initialize(self) -> None:
        """Called when the command is initially scheduled."""
        # Set motors to stop, read encoder values for starting point
        self.drive.arcade_drive(0, 0)
        self.drive.reset_encoders()

    def execute(self) -> None:
        """Called every time the scheduler runs while the command is scheduled."""
        self.drive.arcade_drive(0, self.velocity)

    def end(self, interrupted: bool) -> None:
        """Called once the command ends or is interrupted."""
        self.drive.arcade_drive(0, 0)

    def is_finished(self) -> bool:
        """Returns true when the command should end."""

        # Need to convert distance travelled to degrees. The Standard
        # XRP Chassis found here, https://www.sparkfun.com/products/22230,
        # has a wheel placement diameter (163 mm) - width of the wheel (8 mm) = 155 mm
        # or 6.102 inches. We then take into consideration the width of the tires.
        inch_per_degree = math.pi * 6.102 / 360

        # Compare distance travelled from start to distance based on degree turn
        return self._get_average_turning_distance() >= inch_per_degree * self.degrees

    def _get_average_turning_distance(self) -> float:
        left_distance = abs(self.drive.get_left_distance_inch())
        right_distance = abs(self.drive.get_right_distance_inch())
        return (left_distance + right_distance) / 2.0
