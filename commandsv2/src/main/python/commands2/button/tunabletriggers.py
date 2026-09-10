from types import SimpleNamespace
from typing import Protocol, overload

from wpilib import EventLoop

from ..commandscheduler import CommandScheduler
from ..util import format_args_kwargs
from .trigger import Trigger


class _TuneRevisionSource(Protocol):
    def get_tune_revision(self) -> int: ...


class TunableTriggers:
    """
    Static :class:`Trigger` factories for reacting to tunable changes.

    Each call to :meth:`tuned` creates an independent observer of the supplied tunable.
    The returned trigger reports whether that observer detected a tuning revision change
    during the current event-loop poll. It initially reports ``False``, including when
    the tunable was tuned before observation began. Multiple backend-applied tunes between
    polls coalesce into one ``True`` poll, and local writes do not activate the trigger.

    Use :meth:`Trigger.if_true` to react to every poll containing a tune.
    :meth:`Trigger.on_true` keeps ordinary rising-edge semantics and merges consecutive
    ``True`` polls.

    .. code-block:: python

        TunableTriggers.tuned(kP).if_true(apply_gains_command)

        (
            TunableTriggers.tuned(kP)
            .or_(TunableTriggers.tuned(kI))
            .or_(TunableTriggers.tuned(kD))
            .if_true(apply_gains_command)
        )

    Triggers created with an explicit :class:`wpilib.EventLoop` are only updated when
    that loop is polled by its owner. Command scheduling still follows
    :class:`commands2.CommandScheduler` rules.
    """

    @overload
    @staticmethod
    def tuned(tunable: _TuneRevisionSource) -> Trigger:
        """
        Creates a trigger for detecting backend-applied tunes to a tunable.

        The trigger is polled by :meth:`commands2.CommandScheduler.get_default_button_loop`.

        :param tunable: the tunable to observe; the returned trigger does not copy it
        :returns: a trigger that is true for polls where this observer detected a new tuning revision
        """
        ...

    @overload
    @staticmethod
    def tuned(loop: EventLoop, tunable: _TuneRevisionSource) -> Trigger:
        """
        Creates a trigger for detecting backend-applied tunes to a tunable.

        The supplied event loop must be polled by its owner. The returned trigger observes
        the supplied object directly, so the tunable must outlive the returned trigger and
        any bindings made from it.

        :param loop: the event loop that polls this trigger
        :param tunable: the tunable to observe; the returned trigger does not copy it
        :returns: a trigger that is true for polls where this observer detected a new tuning revision
        """
        ...

    @staticmethod
    def tuned(*args, **kwargs) -> Trigger:
        """
        Creates a trigger for detecting backend-applied tunes to a tunable.

        With one argument, the trigger is polled by
        :meth:`commands2.CommandScheduler.get_default_button_loop`. With an explicit
        :class:`wpilib.EventLoop`, that loop must be polled by its owner. The returned
        trigger observes the supplied object directly, so the tunable must outlive the
        returned trigger and any bindings made from it.

        :param args: either ``(tunable)`` or ``(loop, tunable)``
        :returns: a trigger that is true for polls where this observer detected a new tuning revision
        """

        def init_loop_tunable(loop: EventLoop, tunable: _TuneRevisionSource) -> Trigger:
            if loop is None:
                raise TypeError("loop cannot be None")
            if not hasattr(tunable, "get_tune_revision"):
                raise TypeError("tunable must provide get_tune_revision()")

            state = SimpleNamespace(last_seen=tunable.get_tune_revision(), tuned=False)

            @loop.bind
            def _():
                current = tunable.get_tune_revision()
                state.tuned = current != state.last_seen
                state.last_seen = current

            return Trigger(loop, lambda: state.tuned)

        def init_tunable(tunable: _TuneRevisionSource) -> Trigger:
            return init_loop_tunable(
                CommandScheduler.get_instance().get_default_button_loop(), tunable
            )

        num_args = len(args) + len(kwargs)

        if num_args == 1 and len(args) == 1:
            return init_tunable(args[0])
        elif num_args == 1 and "tunable" in kwargs:
            return init_tunable(kwargs["tunable"])
        elif num_args == 2:
            loop, tunable, *_ = args + (None, None)
            if "loop" in kwargs:
                loop = kwargs["loop"]
            if "tunable" in kwargs:
                tunable = kwargs["tunable"]
            if loop is not None and tunable is not None:
                return init_loop_tunable(loop, tunable)

        raise TypeError(f"""
TypeError: TunableTriggers.tuned(): incompatible function arguments. The following argument types are supported:
    1. (tunable: tunable with get_tune_revision() -> int)
    2. (loop: EventLoop, tunable: tunable with get_tune_revision() -> int)

Invoked with: {format_args_kwargs(*args, **kwargs)}
""")
