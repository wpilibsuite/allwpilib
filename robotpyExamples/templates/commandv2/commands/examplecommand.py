# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import commands2
import wpilib
from subsystems.examplesubsystem import ExampleSubsystem

class ExampleCommand (commands2.Command):
    
    # Creates a new ExampleCommand
    def __init__(self, subsystem) -> None:
        super().__init__()
        self.subsystem = subsystem
        # Use addRequirements() here to declare subsystem dependencies
        self.addRequirements(subsystem)

    # Called when the command is initially scheduled.
    def initialize(self) -> None:

    # Called every time the scheduler runs while the command is scheduled
    def execute(self) -> None:

    # Called once the command ends or in interrupted
    def end(self, interrupted: bool) -> None:

    # Returns true when the command should end
    def is_finished(self) -> bool:
        return False
        