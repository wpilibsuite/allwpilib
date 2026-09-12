# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

from typing import TYPE_CHECKING

import wpilib

if TYPE_CHECKING:
    from robot import Robot


@wpilib.teleop
class DefaultTeleMode(wpilib.PeriodicOpMode):
    def __init__(self, robot: "Robot") -> None:
        super().__init__()
        self.robot = robot
        self.gamepad = wpilib.DriverStation.get_gamepad(0)

    def periodic(self) -> None:
        self.robot.motor0.set_throttle(-self.gamepad.get_left_y())
        self.robot.motor1.set_throttle(-self.gamepad.get_right_y())
        self.robot.motor2.set_throttle(-self.gamepad.get_left_x())
        self.robot.motor3.set_throttle(-self.gamepad.get_right_x())
        self.robot.servo0.set_position(self.gamepad.get_left_trigger())
        self.robot.servo1.set_position(self.gamepad.get_right_trigger())
