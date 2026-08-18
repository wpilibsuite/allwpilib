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
        self._runs_when_disabled = True
        self._finished = True
        self._interrupt_behavior = InterruptionBehavior.CANCEL_INCOMING
        self.add_commands(*commands)

    def add_commands(self, *commands: Command):
        """
        Adds the given commands to the group.

        :param commands: Commands to add to the group.
        """
        commands = flatten_args_commands(commands)
        if not self._finished:
            raise IllegalCommandUse(
                "Commands cannot be added to a composition while it is running"
            )

        CommandScheduler.get_instance().register_composed_commands(commands)

        for command in commands:
            in_common = command.get_requirements().intersection(self.requirements)
            if in_common:
                raise IllegalCommandUse(
                    "Multiple commands in a parallel composition cannot require the same subsystems.",
                    common=in_common,
                )

            self._commands.add(command)
            self.requirements.update(command.get_requirements())
            self._runs_when_disabled = (
                self._runs_when_disabled and command.runs_when_disabled()
            )

            if command.get_interruption_behavior() == InterruptionBehavior.CANCEL_SELF:
                self._interrupt_behavior = InterruptionBehavior.CANCEL_SELF

    def initialize(self):
        self._finished = False
        for command in self._commands:
            command.initialize()

    def execute(self):
        for command in self._commands:
            command.execute()
            if command.is_finished():
                self._finished = True

    def end(self, interrupted: bool):
        for command in self._commands:
            command.end(not command.is_finished())

    def is_finished(self) -> bool:
        return self._finished

    def runs_when_disabled(self) -> bool:
        return self._runs_when_disabled

    def get_interruption_behavior(self) -> InterruptionBehavior:
        return self._interrupt_behavior
