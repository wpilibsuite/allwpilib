#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import Command
from subsystems.xrpdrivetrain import XRPDrivetrain

class ExampleCommand(Command):
    def __init__(self, subsystem) -> None:
        super().__init__()
        self.subsystem = subsystem
        # Use addRequirements() here to declare subsystem dependencies
        self.addRequirements(subsystem)

    # Called when the command is initially scheduled
    def initialize(self) -> None:

    # Called every time the scheduler runs while the command is scheduled
    def execute(self) -> None:

    # Called once the command ends or is interrupted
    def end(self, interrupted: bool) -> None:

    # Returns true when the command should end
    def is_finished(self) -> bool:
        return False