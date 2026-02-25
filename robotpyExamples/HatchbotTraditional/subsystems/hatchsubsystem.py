#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import commands2

import constants


class HatchSubsystem(commands2.Subsystem):
    def __init__(self) -> None:

        self.hatchSolenoid = wpilib.DoubleSolenoid(
            constants.kHatchSolenoidModule,
            constants.kHatchSolenoidModuleType,
            *constants.kHatchSolenoidPorts
        )

    def grabHatch(self) -> None:
        """Grabs the hatch"""
        self.hatchSolenoid.set(wpilib.DoubleSolenoid.Value.kForward)

    def releaseHatch(self) -> None:
        """Releases the hatch"""
        self.hatchSolenoid.set(wpilib.DoubleSolenoid.Value.kReverse)
