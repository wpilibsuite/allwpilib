#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib

import constants
from subsystems.intake import Intake


class MyRobot(wpilib.TimedRobot):
    """The methods in this class are called automatically corresponding to each mode, as
    described in the TimedRobot documentation. If you change the name of this class or the
    package after creating this project, you must also update the Main.java file in the
    project.
    """

    def __init__(self) -> None:
        super().__init__()
        self.intake = Intake()
        self.joystick = wpilib.Joystick(constants.kJoystickIndex)

    def teleopPeriodic(self) -> None:
        """This function is called periodically during operator control."""
        # Activate the intake while the trigger is held
        if self.joystick.getTrigger():
            self.intake.activate(constants.IntakeConstants.kIntakeSpeed)
        else:
            self.intake.activate(0)

        # Toggle deploying the intake when the top button is pressed
        if self.joystick.getTop():
            if self.intake.isDeployed():
                self.intake.retract()
            else:
                self.intake.deploy()
