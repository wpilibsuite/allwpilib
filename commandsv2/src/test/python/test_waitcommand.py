from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_waitCommand(scheduler: commands2.CommandScheduler):
    with ManualSimTime() as sim:
        waitCommand = commands2.WaitCommand(2)

        scheduler.schedule(waitCommand)
        scheduler.run()
        sim.step(1)
        scheduler.run()

        assert scheduler.isScheduled(waitCommand)

        sim.step(2)

        scheduler.run()

        assert not scheduler.isScheduled(waitCommand)


def test_withTimeout(scheduler: commands2.CommandScheduler):
    with ManualSimTime() as sim:
        command1 = commands2.Command()
        start_spying_on(command1)

        timeout = command1.withTimeout(2)

        scheduler.schedule(timeout)
        scheduler.run()

        verify(command1).initialize()
        verify(command1).execute()
        assert not scheduler.isScheduled(command1)
        assert scheduler.isScheduled(timeout)

        sim.step(3)
        scheduler.run()

        verify(command1).end(True)
        verify(command1, never()).end(False)
        assert not scheduler.isScheduled(timeout)
