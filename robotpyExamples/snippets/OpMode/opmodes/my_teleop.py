# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

from typing import TYPE_CHECKING, override

import wpilib

if TYPE_CHECKING:
    from robot import Robot


@wpilib.teleop(name="My Teleop", description="An example teleop opmode")
class MyTeleop(wpilib.PeriodicOpMode):
    def __init__(self, robot: "Robot") -> None:
        """The Robot instance is passed into the opmode via the constructor."""
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
