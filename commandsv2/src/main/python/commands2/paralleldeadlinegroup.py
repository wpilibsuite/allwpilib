# validated: 2024-01-19 DS e07de37e64f2 ParallelDeadlineGroup.java
from __future__ import annotations

from typing import Dict

from wpiutil import SendableBuilder

from .command import Command, InterruptionBehavior
from .commandscheduler import CommandScheduler
from .exceptions import IllegalCommandUse
from .util import flatten_args_commands


class ParallelDeadlineGroup(Command):
    """
    A command composition that runs a set of commands in parallel, ending only when a specific
    command (the "deadline") ends, interrupting all other commands that are still running at that
    point.

    The rules for command compositions apply: command instances that are passed to it cannot be
    added to any other composition or scheduled individually, and the composition requires all
    subsystems its components require.
    """

    def __init__(self, deadline: Command, *commands: Command):
        """
        Creates a new ParallelDeadlineGroup. The given commands (including the
        deadline) will be executed simultaneously. The composition will finish when
        the deadline finishes, interrupting all other still-running commands. If
        the composition is interrupted, only the commands still running will be
        interrupted.

        :param deadline: the command that determines when the composition ends
        :param commands: the commands to be executed

        :raises IllegalCommandUse: if the deadline command is also in the otherCommands argument
        """
        super().__init__()
        self._commands: Dict[Command, bool] = {}
        self._runsWhenDisabled = True
        self._finished = True
        self._interruptBehavior = InterruptionBehavior.kCancelIncoming
        self.addCommands(*commands)
        self.setDeadline(deadline)

    def setDeadline(self, deadline: Command):
        """
        Sets the deadline to the given command. The deadline is added to the group if it is not already
        contained.

        :param deadline: the command that determines when the group ends

        :raises IllegalCommandUse: if the deadline command is already in the composition
        """

        # use getattr here because deadline not set in constructor
        isAlreadyDeadline = deadline == getattr(self, "_deadline", None)
        if isAlreadyDeadline:
            return

        if deadline in self._commands:
            raise IllegalCommandUse(
                f"The deadline command cannot also be in the other commands!",
                deadline=deadline,
            )
        self.addCommands(deadline)
        self._deadline = deadline

    def addCommands(self, *commands: Command):
        """
        Adds the given commands to the group.

        :param commands: Commands to add to the group.

        :raises IllegalCommandUse: if the deadline command is already in the composition
        """
        commands = flatten_args_commands(commands)
        if not self._finished:
            raise IllegalCommandUse(
                "Commands cannot be added to a composition while it is running"
            )

        CommandScheduler.getInstance().registerComposedCommands(commands)

        for command in commands:
            in_common = command.getRequirements().intersection(self.requirements)
            if in_common:
                raise IllegalCommandUse(
                    "Multiple commands in a parallel composition cannot require the same subsystems.",
                    common=in_common,
                )

            self._commands[command] = False
            self.requirements.update(command.getRequirements())
            self._runsWhenDisabled = (
                self._runsWhenDisabled and command.runsWhenDisabled()
            )

            if command.getInterruptionBehavior() == InterruptionBehavior.kCancelSelf:
                self._interruptBehavior = InterruptionBehavior.kCancelSelf

    def initialize(self):
        for command in self._commands:
            command.initialize()
            self._commands[command] = True
        self._finished = False

    def execute(self):
        for command, isRunning in self._commands.items():
            if not isRunning:
                continue
            command.execute()
            if command.isFinished():
                command.end(False)
                self._commands[command] = False
                if command == self._deadline:
                    self._finished = True

    def end(self, interrupted: bool):
        for command, isRunning in self._commands.items():
            if not isRunning:
                continue
            command.end(True)
            self._commands[command] = False

    def isFinished(self) -> bool:
        return self._finished

    def runsWhenDisabled(self) -> bool:
        return self._runsWhenDisabled

    def getInterruptionBehavior(self) -> InterruptionBehavior:
        return self._interruptBehavior

    def initSendable(self, builder: SendableBuilder):
        super().initSendable(builder)

        builder.addStringProperty(
            "deadline", lambda: self._deadline.getName(), lambda _: None
        )
