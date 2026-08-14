# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import wpilib
import commands2

class ReplaceMeSequentialCommandGroup(commands2.SequentialCommandGroup):
    # Creates a new ReplaceMeSequentialCommandGroup
    def __init__(self):
        super().__init__()
        # Add your commands in the addCommands() call
        self.addCommands()
