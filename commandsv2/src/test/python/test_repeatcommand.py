from typing import TYPE_CHECKING

import commands2
from compositiontestbase import SingleCompositionTestBase  # type: ignore
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *
    from .compositiontestbase import SingleCompositionTestBase

import pytest


class RepeatCommandCompositionTest(SingleCompositionTestBase):
    def composeSingle(self, member):
        return member.repeatedly()


def test_callsMethodsCorrectly(scheduler: commands2.CommandScheduler):
    command = commands2.Command()
    repeated = command.repeatedly()

    start_spying_on(command)

    assert command.initialize.times_called == 0
    assert command.execute.times_called == 0
    assert command.isFinished.times_called == 0
    assert command.end.times_called == 0

    scheduler.schedule(repeated)
    assert command.initialize.times_called == 1
    assert command.execute.times_called == 0
    assert command.isFinished.times_called == 0
    assert command.end.times_called == 0

    command.isFinished = lambda: False

    scheduler.run()
    assert command.initialize.times_called == 1
    assert command.execute.times_called == 1
    assert command.isFinished.times_called == 1
    assert command.end.times_called == 0

    command.isFinished = lambda: True
    scheduler.run()
    assert command.initialize.times_called == 1
    assert command.execute.times_called == 2
    assert command.isFinished.times_called == 2
    assert command.end.times_called == 1

    command.isFinished = lambda: False
    scheduler.run()
    assert command.initialize.times_called == 2
    assert command.execute.times_called == 3
    assert command.isFinished.times_called == 3
    assert command.end.times_called == 1

    command.isFinished = lambda: True
    scheduler.run()
    assert command.initialize.times_called == 2
    assert command.execute.times_called == 4
    assert command.isFinished.times_called == 4
    assert command.end.times_called == 2

    scheduler.cancel(repeated)
    assert command.initialize.times_called == 2
    assert command.execute.times_called == 4
    assert command.isFinished.times_called == 4
    assert command.end.times_called == 2
