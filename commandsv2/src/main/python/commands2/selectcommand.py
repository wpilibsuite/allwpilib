# validated: 2024-01-19 DS a4a8ad9c753e SelectCommand.java
from __future__ import annotations

from typing import Callable, Dict, Hashable

from wpiutil import SendableBuilder

from .command import Command, InterruptionBehavior
from .commandscheduler import CommandScheduler
from .printcommand import PrintCommand


class SelectCommand(Command):
    """
    A command composition that runs one of a selection of commands, either using a selector and a key
    to command mapping, or a supplier that returns the command directly at runtime.

    The rules for command compositions apply: command instances that are passed to it cannot be
    added to any other composition or scheduled individually, and the composition requires all
    subsystems its components require.
    """

    def __init__(
        self,
        commands: Dict[Hashable, Command],
        selector: Callable[[], Hashable],
    ):
        """
        Creates a new SelectCommand.

        :param commands: the map of commands to choose from
        :param selector: the selector to determine which command to run
        """
        super().__init__()

        assert callable(selector)

        self._default_command = PrintCommand(
            "SelectCommand selector value does not correspond to any command!"
        )

        self._commands = commands
        self._selector = selector
        # This is slightly different than Java but avoids UB
        self._selected_command = self._default_command
        self._runs_when_disabled = True
        self._interrupt_behavior = InterruptionBehavior.CANCEL_INCOMING

        scheduler = CommandScheduler.get_instance()
        scheduler.register_composed_commands([self._default_command])
        scheduler.register_composed_commands(commands.values())

        for command in commands.values():
            self.add_requirements(*command.get_requirements())
            self._runs_when_disabled = (
                self._runs_when_disabled and command.runs_when_disabled()
            )
            if command.get_interruption_behavior() == InterruptionBehavior.CANCEL_SELF:
                self._interrupt_behavior = InterruptionBehavior.CANCEL_SELF

    def initialize(self):
        self._selected_command = self._commands.get(
            self._selector(), self._default_command
        )
        self._selected_command.initialize()

    def execute(self):
        self._selected_command.execute()

    def end(self, interrupted: bool):
        self._selected_command.end(interrupted)

    def is_finished(self) -> bool:
        return self._selected_command.is_finished()

    def runs_when_disabled(self) -> bool:
        return self._runs_when_disabled

    def get_interruption_behavior(self) -> InterruptionBehavior:
        return self._interrupt_behavior

    def init_sendable(self, builder: SendableBuilder) -> None:
        super().init_sendable(builder)
        builder.add_string_property(
            "selected", lambda: self._default_command.get_name(), lambda _: None
        )
