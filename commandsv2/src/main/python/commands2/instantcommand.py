# validated: 2024-01-19 DS 5cf961edb973 InstantCommand.java
from __future__ import annotations

from typing import Callable, Optional

from .functionalcommand import FunctionalCommand
from .subsystem import Subsystem


class InstantCommand(FunctionalCommand):
    """
    A Command that runs instantly; it will initialize, execute once, and end on the same iteration of
    the scheduler. Users can either pass in a Callable and a set of requirements, or else subclass
    this command if desired.
    """

    def __init__(
        self, toRun: Optional[Callable[[], None]] = None, *requirements: Subsystem
    ):
        """
        Creates a new InstantCommand that runs the given Callable with the given requirements.

        :param toRun: the Runnable to run
        :param requirements: the subsystems required by this command
        """
        assert toRun is None or callable(toRun)
        super().__init__(
            toRun or (lambda: None),
            lambda: None,
            lambda _: None,
            lambda: True,
            *requirements,
        )
