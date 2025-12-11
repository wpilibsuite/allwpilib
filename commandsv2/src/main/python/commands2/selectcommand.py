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

        self._defaultCommand = PrintCommand(
            "SelectCommand selector value does not correspond to any command!"
        )

        self._commands = commands
        self._selector = selector
        # This is slightly different than Java but avoids UB
        self._selectedCommand = self._defaultCommand
        self._runsWhenDisabled = True
        self._interruptBehavior = InterruptionBehavior.kCancelIncoming

        scheduler = CommandScheduler.getInstance()
        scheduler.registerComposedCommands([self._defaultCommand])
        scheduler.registerComposedCommands(commands.values())

        for command in commands.values():
            self.addRequirements(*command.getRequirements())
            self._runsWhenDisabled = (
                self._runsWhenDisabled and command.runsWhenDisabled()
            )
            if command.getInterruptionBehavior() == InterruptionBehavior.kCancelSelf:
                self._interruptBehavior = InterruptionBehavior.kCancelSelf

    def initialize(self):
        self._selectedCommand = self._commands.get(
            self._selector(), self._defaultCommand
        )
        self._selectedCommand.initialize()

    def execute(self):
        self._selectedCommand.execute()

    def end(self, interrupted: bool):
        self._selectedCommand.end(interrupted)

    def isFinished(self) -> bool:
        return self._selectedCommand.isFinished()

    def runsWhenDisabled(self) -> bool:
        return self._runsWhenDisabled

    def getInterruptionBehavior(self) -> InterruptionBehavior:
        return self._interruptBehavior

    def initSendable(self, builder: SendableBuilder) -> None:
        super().initSendable(builder)
        builder.addStringProperty(
            "selected", lambda: self._defaultCommand.getName(), lambda _: None
        )
