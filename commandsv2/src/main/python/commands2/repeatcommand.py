# validated: 2024-01-19 DS 6e58db398d63 RepeatCommand.java
from __future__ import annotations

from wpiutil import SendableBuilder

from .command import Command, InterruptionBehavior
from .commandscheduler import CommandScheduler


class RepeatCommand(Command):
    """
    A command that runs another command repeatedly, restarting it when it ends, until this command is
    interrupted. Command instances that are passed to it cannot be added to any other groups, or
    scheduled individually.

    The rules for command compositions apply: command instances that are passed to it cannot be
    added to any other composition or scheduled individually,and the composition requires all
    subsystems its components require.
    """

    def __init__(self, command: Command):
        """
        Creates a new RepeatCommand. Will run another command repeatedly, restarting it whenever it
        ends, until this command is interrupted.

        :param command: the command to run repeatedly
        """
        super().__init__()
        self._command = command
        CommandScheduler.getInstance().registerComposedCommands([command])
        self.requirements.update(command.getRequirements())
        self.setName(f"Repeat({command.getName()})")

    def initialize(self):
        self._ended = False
        self._command.initialize()

    def execute(self):
        if self._ended:
            self._ended = False
            self._command.initialize()

        self._command.execute()

        if self._command.isFinished():
            # restart command
            self._command.end(False)
            self._ended = True

    def isFinished(self) -> bool:
        return False

    def end(self, interrupted: bool):
        # Make sure we didn't already call end() (which would happen if the command finished in the
        # last call to our execute())
        if not self._ended:
            self._command.end(interrupted)
            self._ended = True

    def runsWhenDisabled(self) -> bool:
        return self._command.runsWhenDisabled()

    def getInterruptionBehavior(self) -> InterruptionBehavior:
        return self._command.getInterruptionBehavior()

    def initSendable(self, builder: SendableBuilder) -> None:
        super().initSendable(builder)
        builder.addStringProperty("command", self._command.getName, lambda _: None)
