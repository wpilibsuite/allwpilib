# validated: 2024-01-19 DS aaea85ff1656 ScheduleCommand.java
from __future__ import annotations

from .command import Command


class ScheduleCommand(Command):
    """
    Schedules the given commands when this command is initialized. Useful for forking off from
    CommandGroups. Note that if run from a composition, the composition will not know about the
    status of the scheduled commands, and will treat this command as finishing instantly.
    """

    def __init__(self, *commands: Command):
        """
        Creates a new ScheduleCommand that schedules the given commands when initialized.

        :param toSchedule: the commands to schedule
        """
        super().__init__()
        self._toSchedule = set(commands)

    def initialize(self):
        for command in self._toSchedule:
            command.schedule()

    def isFinished(self) -> bool:
        return True

    def runsWhenDisabled(self) -> bool:
        return True
