#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import commands2
import commands2.cmd

import constants


class HatchSubsystem(commands2.Subsystem):
    def __init__(self) -> None:

        self.hatchSolenoid = wpilib.DoubleSolenoid(
            constants.kHatchSolenoidModule,
            constants.kHatchSolenoidModuleType,
            *constants.kHatchSolenoidPorts
        )

    def grabHatch(self) -> commands2.Command:
        """Grabs the hatch"""
        return commands2.cmd.runOnce(
            lambda: self.hatchSolenoid.set(wpilib.DoubleSolenoid.Value.kForward), self
        )

    def releaseHatch(self) -> commands2.Command:
        """Releases the hatch"""
        return commands2.cmd.runOnce(
            lambda: self.hatchSolenoid.set(wpilib.DoubleSolenoid.Value.kReverse), self
        )
