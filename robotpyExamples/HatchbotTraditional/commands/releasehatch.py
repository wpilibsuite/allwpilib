#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2

from subsystems.hatchsubsystem import HatchSubsystem


class ReleaseHatch(commands2.InstantCommand):
    def __init__(self, hatch: HatchSubsystem) -> None:
        super().__init__(hatch.releaseHatch, hatch)
