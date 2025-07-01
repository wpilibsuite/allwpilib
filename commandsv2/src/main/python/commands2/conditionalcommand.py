# validated: 2024-01-19 DS aaea85ff1656 ConditionalCommand.java
from __future__ import annotations

from typing import Callable, Optional

from wpilib import TelemetryTable

from .command import Command, InterruptionBehavior
from .commandscheduler import CommandScheduler


class ConditionalCommand(Command):
    """
    A command composition that runs one of two commands, depending on the value of the given
    condition when this command is initialized.

    The rules for command compositions apply: command instances that are passed to it cannot be
    added to any other composition or scheduled individually, and the composition requires all
    subsystems its components require.
    """

    selected_command: Optional[Command]

    def __init__(
        self, on_true: Command, on_false: Command, condition: Callable[[], bool]
    ):
        """
        Creates a new ConditionalCommand.

        :param on_true: the command to run if the condition is true
        :param on_false: the command to run if the condition is false
        :param condition: the condition to determine which command to run
        """
        super().__init__()

        CommandScheduler.get_instance().register_composed_commands([on_true, on_false])

        self.on_true = on_true
        self.on_false = on_false
        self.condition = condition

        self.add_requirements(*on_true.get_requirements())
        self.add_requirements(*on_false.get_requirements())

    def initialize(self):
        if self.condition():
            self.selected_command = self.on_true
        else:
            self.selected_command = self.on_false

        self.selected_command.initialize()

    def execute(self):
        assert self.selected_command is not None
        self.selected_command.execute()

    def end(self, interrupted: bool):
        assert self.selected_command is not None
        self.selected_command.end(interrupted)

    def is_finished(self) -> bool:
        assert self.selected_command is not None
        return self.selected_command.is_finished()

    def runs_when_disabled(self) -> bool:
        return self.on_true.runs_when_disabled() and self.on_false.runs_when_disabled()

    def get_interruption_behavior(self) -> InterruptionBehavior:
        if (
            self.on_true.get_interruption_behavior() == InterruptionBehavior.CANCEL_SELF
            or self.on_false.get_interruption_behavior()
            == InterruptionBehavior.CANCEL_SELF
        ):
            return InterruptionBehavior.CANCEL_SELF
        else:
            return InterruptionBehavior.CANCEL_INCOMING

    def log_to(self, table: TelemetryTable) -> None:
        super().log_to(table)
        table.log("onTrue", self.on_true.get_name())
        table.log("onFalse", self.on_false.get_name())

        def _selected():
            if self.selected_command is None:
                return "null"
            else:
                return self.selected_command.get_name()

        table.log("selected", _selected())
