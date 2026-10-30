# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import wpilib
import commands2

class ReplaceMeParallelDeadlineGroup(commands2.ParallelDeadlineGroup):
    # Creates a new ReplaceMeParallelDeadlineGroup
    def __init__(self):
        # Add the deadline command in the super() call. Add other commands using
        # addCommands()
        super().__init__(commands2.InstantCommand())
        # self.addCommands()
