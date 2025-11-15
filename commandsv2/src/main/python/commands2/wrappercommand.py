# validated: 2024-01-20 DS ad0859a8c9ec WrapperCommand.java
from __future__ import annotations

from typing import Set

from .command import Command, InterruptionBehavior
from .commandscheduler import CommandScheduler


class WrapperCommand(Command):
    """
    A class used internally to wrap commands while overriding a specific method; all other methods
    will call through to the wrapped command.

    The rules for command compositions apply: command instances that are passed to it cannot be
    added to any other composition or scheduled individually, and the composition requires all
    subsystems its components require.
    """

    def __init__(self, command: Command):
        """
        Wrap a command.

        :param command: the command being wrapped. Trying to directly schedule this
                        command or add it to a composition will throw an exception.
        """
        super().__init__()

        CommandScheduler.getInstance().registerComposedCommands([command])
        self._command = command
        self.setName(self._command.getName())

    def initialize(self):
        """
        The initial subroutine of a command. Called once when the command is initially scheduled.
        """
        self._command.initialize()

    def execute(self):
        """
        The main body of a command. Called repeatedly while the command is scheduled.
        """
        self._command.execute()

    def end(self, interrupted: bool):
        """
        The action to take when the command ends. Called when either the command finishes normally, or
        when it interrupted/canceled.

        Do not schedule commands here that share requirements with this command. Use
        :func:`commands2.Command.andThen` instead.

        :param interrupted: whether the command was interrupted/canceled
        """
        self._command.end(interrupted)

    def isFinished(self) -> bool:
        """
        Whether the command has finished. Once a command finishes, the scheduler will call its end()
        method and un-schedule it.

        :returns: whether the command has finished.
        """
        return self._command.isFinished()

    def getRequirements(self) -> Set:
        """
        Specifies the set of subsystems used by this command. Two commands cannot use the same
        subsystem at the same time. If the command is scheduled as interruptible and another command is
        scheduled that shares a requirement, the command will be interrupted. Else, the command will
        not be scheduled. If no subsystems are required, return an empty set.

        Note: it is recommended that user implementations contain the requirements as a field, and
        return that field here, rather than allocating a new set every time this is called.

        :returns: the set of subsystems that are required
        """
        return self._command.getRequirements()

    def runsWhenDisabled(self) -> bool:
        """
        Whether the given command should run when the robot is disabled. Override to return true if the
        command should run when disabled.

        :returns: whether the command should run when the robot is disabled
        """
        return self._command.runsWhenDisabled()

    def getInterruptionBehavior(self) -> InterruptionBehavior:
        return self._command.getInterruptionBehavior()
