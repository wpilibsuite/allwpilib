# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

from typing import TYPE_CHECKING

import wpilib

if TYPE_CHECKING:
    from robot import Robot


@wpilib.autonomous
class DefaultAutoMode(wpilib.PeriodicOpMode):
    def __init__(self, robot: "Robot") -> None:
        super().__init__()
        self.robot = robot
        self.timer = wpilib.Timer()

    def start(self) -> None:
        self.timer.reset()
        self.timer.start()

    def periodic(self) -> None:
        if self.timer.get() < 2.0:
            self.robot.motor0.set_throttle(0.5)
            self.robot.motor1.set_throttle(0.5)
        elif self.timer.get() < 4.0:
            self.robot.motor0.set_throttle(0.9)
            self.robot.motor1.set_throttle(0.9)
        else:
            self.robot.motor0.set_throttle(0.0)
            self.robot.motor1.set_throttle(0.0)
