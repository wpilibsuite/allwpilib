#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import Command
from subsystems.romidrivetrain import RomiDrivetrain

# An example command tht uses an example subsystem
class ExampleCommand(Command):

    '''
    Creates a new Example Command
    @params subsystem The subsystem used by this command
    '''
    def __init__(self, subsystem: RomiDrivetrain) -> None:
        self.subsystem = subsystem
        # Use addRequirements() here to declare subsystem dependencies.
        self.addRequirements(subsystem)

    # Called when the command is initially scheduled
    def initialize(self) -> None:

    # Called every time the scheduler runs while the command is scheduled
    def execute(self) -> None:

    # Called once the command ends or is interrupted
    def end(self, interrupted: bool) -> None:

    # Returns true when the comand should end
    def is_finished(self) -> bool:
        return False;