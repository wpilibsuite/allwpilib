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
        onInit: Callable[[], Any],
        onExecute: Callable[[], Any],
        onEnd: Callable[[bool], Any],
        isFinished: Callable[[], bool],
        *requirements: Subsystem,
    ):
        """
        Creates a new FunctionalCommand.

        :param onInit: the function to run on command initialization
        :param onExecute: the function to run on command execution
        :param onEnd: the function to run on command end
        :param isFinished: the function that determines whether the command has finished
        :param requirements: the subsystems required by this command
        """
        super().__init__()

        assert callable(onInit)
        assert callable(onExecute)
        assert callable(onEnd)
        assert callable(isFinished)

        self._onInit = onInit
        self._onExecute = onExecute
        self._onEnd = onEnd
        self._isFinished = isFinished

        self.addRequirements(*requirements)

    def initialize(self):
        self._onInit()

    def execute(self):
        self._onExecute()

    def end(self, interrupted: bool):
        self._onEnd(interrupted)

    def isFinished(self) -> bool:
        return self._isFinished()
