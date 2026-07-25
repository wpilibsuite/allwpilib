# validated: 2024-01-19 DS 6e58db398d63 FunctionalCommand.java
from __future__ import annotations

from typing import Any, Callable

from .command import Command
from .subsystem import Subsystem


class FunctionalCommand(Command):
    """
    A command that allows the user to pass in functions for each of the basic command methods through
    the constructor. Useful for inline definitions of complex commands - note, however, that if a
    command is beyond a certain complexity it is usually better practice to write a proper class for
    it than to inline it.
    """

    def __init__(
        self,
        on_init: Callable[[], Any],
        on_execute: Callable[[], Any],
        on_end: Callable[[bool], Any],
        is_finished: Callable[[], bool],
        *requirements: Subsystem,
    ):
        """
        Creates a new FunctionalCommand.

        :param on_init: the function to run on command initialization
        :param on_execute: the function to run on command execution
        :param on_end: the function to run on command end
        :param is_finished: the function that determines whether the command has finished
        :param requirements: the subsystems required by this command
        """
        super().__init__()

        assert callable(on_init)
        assert callable(on_execute)
        assert callable(on_end)
        assert callable(is_finished)

        self._on_init = on_init
        self._on_execute = on_execute
        self._on_end = on_end
        self._is_finished = is_finished

        self.add_requirements(*requirements)

    def initialize(self):
        self._on_init()

    def execute(self):
        self._on_execute()

    def end(self, interrupted: bool):
        self._on_end(interrupted)

    def is_finished(self) -> bool:
        return self._is_finished()
