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
        wpilib.DriverStationDisplay.setMode(wpilib.DriverStationDisplay.Mode.Line)
        self.selectable = wpilib.GamepadSelectable(0)
        self.selectable.addOptions("Target", ["High", "Mid", "Low"])
        self.selectable.addIntegerOptions("Delay", 0, 5, 1)
        self.selectable.addDoubleOptions("Speed", 0.25, 1.0, 0.25)
        self.target = "High"
        self.delay = 0
        self.speed = 0.25

    def disabledPeriodic(self) -> None:
        self.selectable.update()
        wpilib.DriverStationDisplay.updateLines()

    def start(self) -> None:
        self.target = self.selectable.getSelected("Target")
        self.delay = self.selectable.getSelectedInteger("Delay")
        self.speed = self.selectable.getSelectedDouble("Speed")

    def periodic(self) -> None:
        wpilib.DriverStationDisplay.addData("Selected Auto", "Score")
        wpilib.DriverStationDisplay.addData("Target", self.target)
        wpilib.DriverStationDisplay.addData("Delay", self.delay)
        wpilib.DriverStationDisplay.addData("Speed", self.speed)
        wpilib.DriverStationDisplay.updateLines()


class BalanceAuto(wpilib.PeriodicOpMode):
    """An autonomous opmode with selectable balance parameters."""

    def __init__(self) -> None:
        super().__init__()
        wpilib.DriverStationDisplay.setMode(wpilib.DriverStationDisplay.Mode.Line)
        self.selectable = wpilib.GamepadSelectable(0)
        self.selectable.addOptions("Approach", ["Left", "Center", "Right"])
        self.selectable.addIntegerOptions("Attempts", 1, 3, 1)
        self.selectable.addDoubleOptions("Turn Scale", 0.2, 1.0, 0.2)
        self.approach = "Center"
        self.attempts = 1
        self.turnScale = 0.5

    def disabledPeriodic(self) -> None:
        self.selectable.update()
        wpilib.DriverStationDisplay.updateLines()

    def start(self) -> None:
        self.approach = self.selectable.getSelected("Approach")
        self.attempts = self.selectable.getSelectedInteger("Attempts")
        self.turnScale = self.selectable.getSelectedDouble("Turn Scale")

    def periodic(self) -> None:
        wpilib.DriverStationDisplay.addData("Selected Auto", "Balance")
        wpilib.DriverStationDisplay.addData("Approach", self.approach)
        wpilib.DriverStationDisplay.addData("Attempts", self.attempts)
        wpilib.DriverStationDisplay.addData("Turn Scale", self.turnScale)
        wpilib.DriverStationDisplay.updateLines()


class MyRobot(OpModeRobot):
    def __init__(self) -> None:
        super().__init__()
        self.addOpMode(DoNothingTeleop, RobotMode.TELEOPERATED, "Do Nothing Teleop")
        self.addOpMode(ScoreAuto, RobotMode.AUTONOMOUS, "Score Auto")
        self.addOpMode(BalanceAuto, RobotMode.AUTONOMOUS, "Balance Auto")
        self.publishOpModes()
