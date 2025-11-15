# validated: 2024-04-02 DS 0b1345946950 button/Trigger.java
from types import SimpleNamespace
from typing import Callable, overload

from typing_extensions import Self
from wpilib.event import EventLoop
from wpimath.filter import Debouncer

from ..command import Command
from ..commandscheduler import CommandScheduler
from ..util import format_args_kwargs


class Trigger:
    """
    This class provides an easy way to link commands to conditions.

    It is very easy to link a button to a command. For instance, you could link the trigger button
    of a joystick to a "score" command.
    """

    _loop: EventLoop
    _condition: Callable[[], bool]

    @overload
    def __init__(self, condition: Callable[[], bool] = lambda: False):
        """
        Creates a new trigger based on the given condition.

        Polled by the default scheduler button loop.

        :param condition: the condition represented by this trigger
        """
        ...

    @overload
    def __init__(self, loop: EventLoop, condition: Callable[[], bool]):
        """
        Creates a new trigger based on the given condition.

        :param loop: The loop instance that polls this trigger.
        :param condition: the condition represented by this trigger
        """
        ...

    def __init__(self, *args, **kwargs):
        def init_loop_condition(loop: EventLoop, condition: Callable[[], bool]):
            assert callable(condition)
            self._loop = loop
            self._condition = condition

        def init_condition(condition: Callable[[], bool]):
            init_loop_condition(
                CommandScheduler.getInstance().getDefaultButtonLoop(), condition
            )

        num_args = len(args) + len(kwargs)

        if num_args == 0:
            return init_condition(lambda: False)
        elif num_args == 1 and len(kwargs) == 1:
            if "condition" in kwargs:
                return init_condition(kwargs["condition"])
        elif num_args == 1 and len(args) == 1:
            if callable(args[0]):
                return init_condition(args[0])
        elif num_args == 2:
            loop, condition, *_ = args + (None, None)
            if "loop" in kwargs:
                loop = kwargs["loop"]
            if "condition" in kwargs:
                condition = kwargs["condition"]
            if loop is not None and condition is not None:
                return init_loop_condition(loop, condition)

        raise TypeError(
            f"""
TypeError: Trigger(): incompatible function arguments. The following argument types are supported:
    1. (self: Trigger)
    2. (self: Trigger, condition: () -> bool)
    3. (self: Trigger, loop: EventLoop, condition: () -> bool)

Invoked with: {format_args_kwargs(self, *args, **kwargs)}
"""
        )

    def onTrue(self, command: Command) -> Self:
        """
        Starts the given command whenever the condition changes from `False` to `True`.

        :param command: the command to start
        :returns: this trigger, so calls can be chained
        """

        state = SimpleNamespace(pressed_last=self._condition())

        @self._loop.bind
        def _():
            pressed = self._condition()
            if not state.pressed_last and pressed:
                command.schedule()
            state.pressed_last = pressed

        return self

    def onFalse(self, command: Command) -> Self:
        """
        Starts the given command whenever the condition changes from `True` to `False`.

        :param command: the command to start
        :returns: this trigger, so calls can be chained
        """

        state = SimpleNamespace(pressed_last=self._condition())

        @self._loop.bind
        def _():
            pressed = self._condition()
            if state.pressed_last and not pressed:
                command.schedule()
            state.pressed_last = pressed

        return self

    def onChange(self, command: Command) -> Self:
        """
        Starts the command when the condition changes.

        :param command: the command t start
        :returns: this trigger, so calls can be chained
        """

        state = SimpleNamespace(pressed_last=self._condition())

        @self._loop.bind
        def _():
            pressed = self._condition()

            if state.pressed_last != pressed:
                command.schedule()

            state.pressed_last = pressed

        return self

    def whileTrue(self, command: Command) -> Self:
        """
        Starts the given command when the condition changes to `True` and cancels it when the condition
        changes to `False`.

        Doesn't re-start the command if it ends while the condition is still `True`. If the command
        should restart, see :class:`commands2.RepeatCommand`.

        :param command: the command to start
        :returns: this trigger, so calls can be chained
        """

        state = SimpleNamespace(pressed_last=self._condition())

        @self._loop.bind
        def _():
            pressed = self._condition()
            if not state.pressed_last and pressed:
                command.schedule()
            elif state.pressed_last and not pressed:
                command.cancel()
            state.pressed_last = pressed

        return self

    def whileFalse(self, command: Command) -> Self:
        """
        Starts the given command when the condition changes to `False` and cancels it when the
        condition changes to `True`.

        Doesn't re-start the command if it ends while the condition is still `False`. If the command
        should restart, see :class:`commands2.RepeatCommand`.

        :param command: the command to start
        :returns: this trigger, so calls can be chained
        """

        state = SimpleNamespace(pressed_last=self._condition())

        @self._loop.bind
        def _():
            pressed = self._condition()
            if state.pressed_last and not pressed:
                command.schedule()
            elif not state.pressed_last and pressed:
                command.cancel()
            state.pressed_last = pressed

        return self

    def toggleOnTrue(self, command: Command) -> Self:
        """
        Toggles a command when the condition changes from `False` to `True`.

        :param command: the command to toggle
        :returns: this trigger, so calls can be chained
        """

        state = SimpleNamespace(pressed_last=self._condition())

        @self._loop.bind
        def _():
            pressed = self._condition()
            if not state.pressed_last and pressed:
                if command.isScheduled():
                    command.cancel()
                else:
                    command.schedule()
            state.pressed_last = pressed

        return self

    def toggleOnFalse(self, command: Command) -> Self:
        """
        Toggles a command when the condition changes from `True` to `False`.

        :param command: the command to toggle
        :returns: this trigger, so calls can be chained
        """

        state = SimpleNamespace(pressed_last=self._condition())

        @self._loop.bind
        def _():
            pressed = self._condition()
            if state.pressed_last and not pressed:
                if command.isScheduled():
                    command.cancel()
                else:
                    command.schedule()
            state.pressed_last = pressed

        return self

    def __call__(self) -> bool:
        return self._condition()

    def getAsBoolean(self) -> bool:
        return self._condition()

    def __bool__(self) -> bool:
        return self._condition()

    def __and__(self, other: Callable[[], bool]) -> "Trigger":
        assert callable(other)
        return Trigger(self._loop, lambda: self() and other())

    def and_(self, other: Callable[[], bool]) -> "Trigger":
        """
        Composes two triggers with logical AND.

        :param trigger: the condition to compose with
        :returns: A trigger which is active when both component triggers are active.
        """
        return self & other

    def __or__(self, other: Callable[[], bool]) -> "Trigger":
        assert callable(other)
        return Trigger(self._loop, lambda: self() or other())

    def or_(self, other: Callable[[], bool]) -> "Trigger":
        """
        Composes two triggers with logical OR.

        :param trigger: the condition to compose with
        :returns: A trigger which is active when either component trigger is active.
        """
        return self | other

    def __invert__(self) -> "Trigger":
        return Trigger(self._loop, lambda: not self())

    def negate(self) -> "Trigger":
        """
        Creates a new trigger that is active when this trigger is inactive, i.e. that acts as the
        negation of this trigger.

        :returns: the negated trigger
        """
        return ~self

    def not_(self) -> "Trigger":
        return ~self

    def debounce(
        self,
        seconds: float,
        debounce_type: Debouncer.DebounceType = Debouncer.DebounceType.kRising,
    ) -> "Trigger":
        """
        Creates a new debounced trigger from this trigger - it will become active when this trigger has
        been active for longer than the specified period.

        :param seconds: The debounce period.
        :param type: The debounce type.
        :returns: The debounced trigger.
        """
        debouncer = Debouncer(seconds, debounce_type)
        return Trigger(self._loop, lambda: debouncer.calculate(self()))
