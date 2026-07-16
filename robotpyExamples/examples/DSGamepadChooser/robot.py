#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from hal import RobotMode
import wpilib
from wpilib.opmoderobot import OpModeRobot


class DoNothingTeleop(wpilib.PeriodicOpMode):
    """A teleop opmode that intentionally does nothing."""


class ScoreAuto(wpilib.PeriodicOpMode):
    """An autonomous opmode with selectable scoring parameters."""

    def __init__(self) -> None:
        super().__init__()
        wpilib.DriverStationDisplay.set_mode(wpilib.DriverStationDisplay.Mode.LINE)
        self.chooser = wpilib.DSGamepadChooser(0)
        self.chooser.add_options("Target", ["High", "Mid", "Low"])
        self.chooser.add_integer_options("Delay", 0, 5, 1)
        self.chooser.add_double_options("Speed", 0.25, 1.0, 0.25)
        self.target = "High"
        self.delay = 0
        self.speed = 0.25

    def disabledPeriodic(self) -> None:
        self.chooser.update()
        wpilib.DriverStationDisplay.update_lines()

    def start(self) -> None:
        self.target = self.chooser.get_selected("Target")
        self.delay = self.chooser.get_selected_integer("Delay")
        self.speed = self.chooser.get_selected_double("Speed")

    def periodic(self) -> None:
        wpilib.DriverStationDisplay.add_data("Selected Auto", "Score")
        wpilib.DriverStationDisplay.add_data("Target", self.target)
        wpilib.DriverStationDisplay.add_data("Delay", str(self.delay))
        wpilib.DriverStationDisplay.add_data("Speed", str(self.speed))
        wpilib.DriverStationDisplay.update_lines()


class BalanceAuto(wpilib.PeriodicOpMode):
    """An autonomous opmode with selectable balance parameters."""

    def __init__(self) -> None:
        super().__init__()
        wpilib.DriverStationDisplay.set_mode(wpilib.DriverStationDisplay.Mode.LINE)
        self.chooser = wpilib.DSGamepadChooser(0)
        self.chooser.add_options("Approach", ["Left", "Center", "Right"])
        self.chooser.add_integer_options("Attempts", 1, 3, 1)
        self.chooser.add_double_options("Turn Scale", 0.2, 1.0, 0.2)
        self.approach = "Center"
        self.attempts = 1
        self.turnScale = 0.5

    def disabledPeriodic(self) -> None:
        self.chooser.update()
        wpilib.DriverStationDisplay.update_lines()

    def start(self) -> None:
        self.approach = self.chooser.get_selected("Approach")
        self.attempts = self.chooser.get_selected_integer("Attempts")
        self.turnScale = self.chooser.get_selected_double("Turn Scale")

    def periodic(self) -> None:
        wpilib.DriverStationDisplay.add_data("Selected Auto", "Balance")
        wpilib.DriverStationDisplay.add_data("Approach", self.approach)
        wpilib.DriverStationDisplay.add_data("Attempts", str(self.attempts))
        wpilib.DriverStationDisplay.add_data("Turn Scale", str(self.turnScale))
        wpilib.DriverStationDisplay.update_lines()


class MyRobot(OpModeRobot):
    def __init__(self) -> None:
        super().__init__()
        self.add_opmode(DoNothingTeleop, RobotMode.TELEOPERATED, "Do Nothing Teleop")
        self.add_opmode(ScoreAuto, RobotMode.AUTONOMOUS, "Score Auto")
        self.add_opmode(BalanceAuto, RobotMode.AUTONOMOUS, "Balance Auto")
        self.publish_opmodes()
