# validated: 2024-01-19 DS aaea85ff1656 ConditionalCommand.java
from __future__ import annotations

from typing import Callable, Optional

from wpiutil import SendableBuilder

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

    selectedCommand: Optional[Command]

    def __init__(
        self, onTrue: Command, onFalse: Command, condition: Callable[[], bool]
    ):
        """
        Creates a new ConditionalCommand.

        :param onTrue: the command to run if the condition is true
        :param onFalse: the command to run if the condition is false
        :param condition: the condition to determine which command to run
        """
        super().__init__()

        CommandScheduler.getInstance().registerComposedCommands([onTrue, onFalse])

        self.onTrue = onTrue
        self.onFalse = onFalse
        self.condition = condition

        self.addRequirements(*onTrue.getRequirements())
        self.addRequirements(*onFalse.getRequirements())

    def initialize(self):
        if self.condition():
            self.selectedCommand = self.onTrue
        else:
            self.selectedCommand = self.onFalse

        self.selectedCommand.initialize()

    def execute(self):
        assert self.selectedCommand is not None
        self.selectedCommand.execute()

    def end(self, interrupted: bool):
        assert self.selectedCommand is not None
        self.selectedCommand.end(interrupted)

    def isFinished(self) -> bool:
        assert self.selectedCommand is not None
        return self.selectedCommand.isFinished()

    def runsWhenDisabled(self) -> bool:
        return self.onTrue.runsWhenDisabled() and self.onFalse.runsWhenDisabled()

    def getInterruptionBehavior(self) -> InterruptionBehavior:
        if (
            self.onTrue.getInterruptionBehavior() == InterruptionBehavior.kCancelSelf
            or self.onFalse.getInterruptionBehavior()
            == InterruptionBehavior.kCancelSelf
        ):
            return InterruptionBehavior.kCancelSelf
        else:
            return InterruptionBehavior.kCancelIncoming

    def initSendable(self, builder: SendableBuilder):
        super().initSendable(builder)
        builder.addStringProperty("onTrue", self.onTrue.getName, lambda _: None)
        builder.addStringProperty("onFalse", self.onFalse.getName, lambda _: None)

        def _selected():
            if self.selectedCommand is None:
                return "null"
            else:
                return self.selectedCommand.getName()

        builder.addStringProperty(
            "selected",
            _selected,
            lambda _: None,
        )
