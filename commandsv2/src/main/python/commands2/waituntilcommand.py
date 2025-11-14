# validated: 2024-01-20 DS aaea85ff1656 WaitUntilCommand.java
from __future__ import annotations

from typing import Callable, overload

from wpilib import Timer
from wpimath import units

from .command import Command
from .util import format_args_kwargs


class WaitUntilCommand(Command):
    """
    A command that does nothing but ends after a specified match time or condition. Useful for
    CommandGroups.
    """

    _condition: Callable[[], bool]

    @overload
    def __init__(self, condition: Callable[[], bool]):
        """
        Creates a new WaitUntilCommand that ends after a given condition becomes true.

        :param condition: the condition to determine when to end
        """
        ...

    @overload
    def __init__(self, time: units.seconds):
        """
        Creates a new WaitUntilCommand that ends after a given match time.

        NOTE: The match timer used for this command is UNOFFICIAL. Using this command does NOT
        guarantee that the time at which the action is performed will be judged to be legal by the
        referees. When in doubt, add a safety factor or time the action manually.

        :param time: the match time at which to end, in seconds
        """
        ...

    def __init__(self, *args, **kwargs):
        super().__init__()

        def init_condition(condition: Callable[[], bool]) -> None:
            assert callable(condition)
            self._condition = condition

        def init_time(time: float) -> None:
            self._condition = lambda: Timer.getMatchTime() - time > 0

        num_args = len(args) + len(kwargs)

        if num_args == 1 and len(kwargs) == 1:
            if "condition" in kwargs:
                return init_condition(kwargs["condition"])
            elif "time" in kwargs:
                return init_time(kwargs["time"])
        elif num_args == 1 and len(args) == 1:
            if isinstance(args[0], float):
                return init_time(args[0])
            elif callable(args[0]):
                return init_condition(args[0])

        raise TypeError(
            f"""
TypeError: WaitUntilCommand(): incompatible function arguments. The following argument types are supported:
    1. (self: WaitUntilCommand, condition: () -> bool)
    2. (self: WaitUntilCommand, time: wpimath.units.seconds)

Invoked with: {format_args_kwargs(self, *args, **kwargs)}
"""
        )

    def isFinished(self) -> bool:
        return self._condition()

    def runsWhenDisabled(self) -> bool:
        return True
