# validated: 2024-01-19 DS 6e58db398d63 NotifierCommand.java
from __future__ import annotations

from typing import Any, Callable

from wpilib import Notifier
from wpimath import units

from .command import Command
from .subsystem import Subsystem


class NotifierCommand(Command):
    """
    A command that starts a notifier to run the given Callable periodically in a separate thread. Has
    no end condition as-is; either subclass it or use :func:`commands2.Command.withTimeout` or :func:`commands2.Command.until` to give it one.

    .. warning:: Do not use this class unless you are confident in your ability
                 to make the executed code thread-safe. If you do not know what
                 "thread-safe" means, that is a good sign that you should not use
                 this class.
    """

    def __init__(
        self, toRun: Callable[[], Any], period: units.seconds, *requirements: Subsystem
    ):
        """
        Creates a new NotifierCommand.

        :param toRun: the Callable for the notifier to run
        :param period: the period at which the notifier should run, in seconds
        :param requirements: the subsystems required by this command
        """
        super().__init__()

        assert callable(toRun)

        self._notifier = Notifier(toRun)
        self._period = period
        self.addRequirements(*requirements)

    def initialize(self):
        self._notifier.startPeriodic(self._period)

    def end(self, interrupted: bool):
        self._notifier.stop()
