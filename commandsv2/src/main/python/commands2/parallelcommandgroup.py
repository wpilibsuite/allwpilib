# validated: 2024-01-19 DS aaea85ff1656 ParallelCommandGroup.java
from __future__ import annotations

from typing import Dict

from .command import Command, InterruptionBehavior
from .commandscheduler import CommandScheduler
from .exceptions import IllegalCommandUse
from .util import flatten_args_commands


class ParallelCommandGroup(Command):
    """
    A command composition that runs a set of commands in parallel, ending when the last command ends.

    The rules for command compositions apply: command instances that are passed to it cannot be
    added to any other composition or scheduled individually, and the composition requires all
    subsystems its components require.
    """

    def __init__(self, *commands: Command):
        """
        Creates a new ParallelCommandGroup. The given commands will be executed simultaneously. The
        command composition will finish when the last command finishes. If the composition is
        interrupted, only the commands that are still running will be interrupted.

        :param commands: the commands to include in this composition.
        """
        super().__init__()
        self._commands: Dict[Command, bool] = {}
        self._runs_when_disabled = True
        self._interrupt_behavior = InterruptionBehavior.CANCEL_INCOMING
        self.add_commands(*commands)

    def add_commands(self, *commands: Command):
        """
        Adds the given commands to the group.

        :param commands: Commands to add to the group
        """
        commands = flatten_args_commands(commands)
        if True in self._commands.values():
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

            self._commands[command] = False
            self.requirements.update(command.get_requirements())
            self._runs_when_disabled = (
                self._runs_when_disabled and command.runs_when_disabled()
            )

            if command.get_interruption_behavior() == InterruptionBehavior.CANCEL_SELF:
                self._interrupt_behavior = InterruptionBehavior.CANCEL_SELF

    def initialize(self):
        for command in self._commands:
            command.initialize()
            self._commands[command] = True

    def execute(self):
        for command, is_running in self._commands.items():
            if not is_running:
                continue
            command.execute()
            if command.is_finished():
                command.end(False)
                self._commands[command] = False

    def end(self, interrupted: bool):
        if interrupted:
            for command, is_running in self._commands.items():
                if not is_running:
                    continue
                command.end(True)
                self._commands[command] = False

    def is_finished(self) -> bool:
        return True not in self._commands.values()

    def runs_when_disabled(self) -> bool:
        return self._runs_when_disabled

    def get_interruption_behavior(self) -> InterruptionBehavior:
        return self._interrupt_behavior
