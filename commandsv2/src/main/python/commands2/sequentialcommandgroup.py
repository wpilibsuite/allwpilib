# validated: 2024-01-19 DS aaea85ff1656 SequentialCommandGroup.java
from __future__ import annotations

from typing import List

from wpiutil import SendableBuilder

from .command import Command, InterruptionBehavior
from .commandscheduler import CommandScheduler
from .exceptions import IllegalCommandUse
from .util import flatten_args_commands


class SequentialCommandGroup(Command):
    """
    A command composition that runs a list of commands in sequence.

    The rules for command compositions apply: command instances that are passed to it cannot be
    added to any other composition or scheduled individually, and the composition requires all
    subsystems its components require.
    """

    def __init__(self, *commands: Command):
        """
        Creates a new SequentialCommandGroup. The given commands will be run sequentially, with the
        composition finishing when the last command finishes.

        :param commands: the commands to include in this composition.
        """
        super().__init__()
        self._commands: List[Command] = []
        self._current_command_index = -1
        self._runs_when_disabled = True
        self._interrupt_behavior = InterruptionBehavior.CANCEL_INCOMING
        self.add_commands(*commands)

    def add_commands(self, *commands: Command):
        """
        Adds the given commands to the group.

        :param commands: Commands to add to the group.
        """
        commands = flatten_args_commands(commands)
        if self._current_command_index != -1:
            raise IllegalCommandUse(
                "Commands cannot be added to a composition while it is running"
            )

        CommandScheduler.get_instance().register_composed_commands(commands)

        for command in commands:
            self._commands.append(command)
            self.requirements.update(command.get_requirements())
            self._runs_when_disabled = (
                self._runs_when_disabled and command.runs_when_disabled()
            )
            if command.get_interruption_behavior() == InterruptionBehavior.CANCEL_SELF:
                self._interrupt_behavior = InterruptionBehavior.CANCEL_SELF

    def initialize(self):
        self._current_command_index = 0
        if self._commands:
            self._commands[0].initialize()

    def execute(self):
        if not self._commands:
            return

        current_command = self._commands[self._current_command_index]

        current_command.execute()
        if current_command.is_finished():
            current_command.end(False)
            self._current_command_index += 1
            if self._current_command_index < len(self._commands):
                self._commands[self._current_command_index].initialize()

    def end(self, interrupted: bool):
        if (
            interrupted
            and self._commands
            and -1 < self._current_command_index < len(self._commands)
        ):
            self._commands[self._current_command_index].end(True)

        self._current_command_index = -1

    def is_finished(self) -> bool:
        return self._current_command_index == len(self._commands)

    def runs_when_disabled(self) -> bool:
        return self._runs_when_disabled

    def get_interruption_behavior(self) -> InterruptionBehavior:
        return self._interrupt_behavior

    def init_sendable(self, builder: SendableBuilder) -> None:
        super().init_sendable(builder)
        builder.add_integer_property(
            "index", lambda: self._current_command_index, lambda _: None
        )
