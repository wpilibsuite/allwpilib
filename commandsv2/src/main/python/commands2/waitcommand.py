# validated: 2024-01-20 DS f29a7d2e501b WaitCommand.java
from __future__ import annotations

from wpilib import Timer
from wpimath import units
from wpiutil import SendableBuilder

from .command import Command


class WaitCommand(Command):
    """
    A command that does nothing but takes a specified amount of time to finish.
    """

    def __init__(self, seconds: units.seconds):
        """
        Creates a new WaitCommand. This command will do nothing, and end after the specified duration.

        :param seconds: the time to wait, in seconds
        """
        super().__init__()
        self._duration = seconds
        self._timer = Timer()
        self.setName(f"{self.getName()}: {seconds}")

    def initialize(self):
        self._timer.restart()

    def end(self, interrupted: bool):
        self._timer.stop()

    def isFinished(self) -> bool:
        return self._timer.hasElapsed(self._duration)

    def runsWhenDisabled(self) -> bool:
        return True

    def initSendable(self, builder: SendableBuilder) -> None:
        super().initSendable(builder)
        builder.addDoubleProperty("duration", lambda: self._duration, lambda _: None)
