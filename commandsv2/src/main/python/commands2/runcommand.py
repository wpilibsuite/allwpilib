# validated: 2024-01-19 DS 5cf961edb973 RunCommand.java
from __future__ import annotations

from typing import Any, Callable

from .functionalcommand import FunctionalCommand
from .subsystem import Subsystem


class RunCommand(FunctionalCommand):
    """
    A command that runs a Callable continuously. Has no end condition as-is; either subclass it or
    use :func:`commands2.Command.withTimeout` or :func:`commands2.Command.until` to give it one.
    If you only wish to execute a Callable once, use :class:`commands2.InstantCommand`.
    """

    def __init__(self, toRun: Callable[[], Any], *requirements: Subsystem):
        """
        Creates a new RunCommand. The Callable will be run continuously until the command ends. Does
        not run when disabled.

        :param toRun: the Callable to run
        :param requirements: the subsystems to require
        """
        assert callable(toRun)
        super().__init__(
            lambda: None, toRun, lambda interrupted: None, lambda: False, *requirements
        )
