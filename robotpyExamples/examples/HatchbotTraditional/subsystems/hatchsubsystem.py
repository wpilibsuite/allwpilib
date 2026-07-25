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

        self.hatch_solenoid = wpilib.DoubleSolenoid(
            constants.HATCH_SOLENOID_MODULE,
            constants.HATCH_SOLENOID_MODULE_TYPE,
            *constants.HATCH_SOLENOID_PORTS
        )

    def grab_hatch(self) -> None:
        """Grabs the hatch"""
        self.hatch_solenoid.set(wpilib.DoubleSolenoid.Value.FORWARD)

    def release_hatch(self) -> None:
        """Releases the hatch"""
        self.hatch_solenoid.set(wpilib.DoubleSolenoid.Value.REVERSE)
