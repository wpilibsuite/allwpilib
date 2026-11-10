# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

from typing import override

import wpilib


class Robot(wpilib.OpModeRobot):
    def __init__(self) -> None:
        # Opmodes marked with @wpilib.autonomous, @wpilib.teleop or
        # @wpilib.utility in the opmodes package are registered and published
        # automatically.
        super().__init__()

    @override
    def driver_station_connected(self) -> None:
        """This function is called exactly once when the DS first connects."""

    @override
    def none_periodic(self) -> None:
        """
        This function is called periodically anytime when no opmode is selected,
        including when the Driver Station is disconnected.
        """
