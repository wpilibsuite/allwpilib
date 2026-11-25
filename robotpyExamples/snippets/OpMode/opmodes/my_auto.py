# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

from typing import TYPE_CHECKING, override

import wpilib

if TYPE_CHECKING:
    from robot import Robot


@wpilib.autonomous(name="My Auto")
class MyAuto(wpilib.PeriodicOpMode):
    def __init__(self, robot: "Robot") -> None:
        """The Robot instance is passed into the opmode via the constructor."""
        # Can pass a period to the base class constructor to set a different
        # periodic time interval.
        #
        # Additional periodic methods may be configured with add_periodic().
        super().__init__()
        self.robot = robot

    @override
    def start(self) -> None:
        """Called once when the robot is first enabled."""

    @override
    def periodic(self) -> None:
        """Called periodically (set time interval) while the robot is enabled."""

    @override
    def end(self) -> None:
        """Called when the robot is disabled (after previously being enabled)."""
