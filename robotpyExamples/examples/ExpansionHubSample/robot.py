# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import wpilib


class Robot(wpilib.OpModeRobot):
    """Demo robot for Expansion Hub motors and servos.

    The motors and servos are driven using the controllers in the teleop OpMode
    and timed in the autonomous OpMode.
    """

    def __init__(self) -> None:
        """Called once at the beginning of the robot program."""
        super().__init__()
        self.motor0 = wpilib.ExpansionHubMotor(0, 0)
        self.motor1 = wpilib.ExpansionHubMotor(0, 1)
        self.motor2 = wpilib.ExpansionHubMotor(0, 2)
        self.motor3 = wpilib.ExpansionHubMotor(0, 3)
        self.servo0 = wpilib.ExpansionHubServo(0, 0)
        self.servo1 = wpilib.ExpansionHubServo(0, 1)
