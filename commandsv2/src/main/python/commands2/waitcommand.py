# validated: 2024-01-20 DS f29a7d2e501b WaitCommand.java
from __future__ import annotations

from wpilib import TelemetryTable, Timer
from wpimath import units

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
        self.set_name(f"{self.get_name()}: {seconds}")

    def initialize(self):
        self._timer.restart()

    def end(self, interrupted: bool):
        self._timer.stop()

    def is_finished(self) -> bool:
        return self._timer.has_elapsed(self._duration)

    def runs_when_disabled(self) -> bool:
        return True

    def log_to(self, table: TelemetryTable) -> None:
        super().log_to(table)
        table.log("duration", self._duration)
