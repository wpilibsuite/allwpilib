# validated: 2024-01-19 DS aaea85ff1656 ParallelRaceGroup.java
from __future__ import annotations

from typing import Set

from .command import Command, InterruptionBehavior
from .commandscheduler import CommandScheduler
from .exceptions import IllegalCommandUse
from .util import flatten_args_commands


class ParallelRaceGroup(Command):
    """
    A composition that runs a set of commands in parallel, ending when any one of the commands ends
    and interrupting all the others.

    The rules for command compositions apply: command instances that are passed to it cannot be
    added to any other composition or scheduled individually, and the composition requires all
    subsystems its components require.
    """

    def __init__(self, *commands: Command):
        """
        Creates a new ParallelCommandRace. The given commands will be executed simultaneously, and will
        "race to the finish" - the first command to finish ends the entire command, with all other
        commands being interrupted.

        :param commands: the commands to include in this composition.
        """
        super().__init__()
        self._commands: Set[Command] = set()
        self._runsWhenDisabled = True
        self._finished = True
        self._interruptBehavior = InterruptionBehavior.kCancelIncoming
        self.addCommands(*commands)

    def addCommands(self, *commands: Command):
        """
        Adds the given commands to the group.

        :param commands: Commands to add to the group.
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

            self._commands.add(command)
            self.requirements.update(command.getRequirements())
            self._runsWhenDisabled = (
                self._runsWhenDisabled and command.runsWhenDisabled()
            )

            if command.getInterruptionBehavior() == InterruptionBehavior.kCancelSelf:
                self._interruptBehavior = InterruptionBehavior.kCancelSelf

    def initialize(self):
        self._finished = False
        for command in self._commands:
            command.initialize()

    def execute(self):
        for command in self._commands:
            command.execute()
            if command.isFinished():
                self._finished = True

    def end(self, interrupted: bool):
        for command in self._commands:
            command.end(not command.isFinished())

    def isFinished(self) -> bool:
        return self._finished

    def runsWhenDisabled(self) -> bool:
        return self._runsWhenDisabled

    def getInterruptionBehavior(self) -> InterruptionBehavior:
        return self._interruptBehavior
