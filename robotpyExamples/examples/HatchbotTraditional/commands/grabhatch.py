#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2
from subsystems.hatchsubsystem import HatchSubsystem


class GrabHatch(commands2.Command):
    def __init__(self, hatch: HatchSubsystem) -> None:
        self.hatch = hatch
        self.addRequirements(hatch)

    def initialize(self) -> None:
        self.hatch.grabHatch()

    def isFinished(self) -> bool:
        return True
