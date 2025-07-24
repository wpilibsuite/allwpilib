# validated: 2024-01-19 DS b390cad09505 StartEndCommand.java
from __future__ import annotations

from typing import Any, Callable

from .functionalcommand import FunctionalCommand
from .subsystem import Subsystem


class StartEndCommand(FunctionalCommand):
    """
    A command that runs a given Callable when it is initialized, and another Callable when it ends.
    Useful for running and then stopping a motor, or extending and then retracting a solenoid. Has no
    end condition as-is; either subclass it or use :func:`commands2.Command.withTimeout`
    or :func:`commands2.Command.until` to give it one.
    """

    def __init__(
        self,
        onInit: Callable[[], Any],
        onEnd: Callable[[], Any],
        *requirements: Subsystem,
    ):
        """
        Creates a new StartEndCommand. Will run the given Callables when the command starts and when it
        ends.

        :param onInit: the Callable to run on command init
        :param onEnd: the Callable to run on command end
        :param requirements: the subsystems required by this command
        """
        assert callable(onInit)
        assert callable(onEnd)
        super().__init__(
            onInit, lambda: None, lambda _: onEnd(), lambda: False, *requirements
        )
