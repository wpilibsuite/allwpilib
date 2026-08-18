from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_wait_command(scheduler: commands2.CommandScheduler):
    with ManualSimTime() as sim:
        wait_command = commands2.WaitCommand(2)

        scheduler.schedule(wait_command)
        scheduler.run()
        sim.step(1)
        scheduler.run()

        assert scheduler.is_scheduled(wait_command)

        sim.step(2)

        scheduler.run()

        assert not scheduler.is_scheduled(wait_command)


def test_with_timeout(scheduler: commands2.CommandScheduler):
    with ManualSimTime() as sim:
        command1 = commands2.Command()
        start_spying_on(command1)

        timeout = command1.with_timeout(2)

        scheduler.schedule(timeout)
        scheduler.run()

        verify(command1).initialize()
        verify(command1).execute()
        assert not scheduler.is_scheduled(command1)
        assert scheduler.is_scheduled(timeout)

        sim.step(3)
        scheduler.run()

        verify(command1).end(True)
        verify(command1, never()).end(False)
        assert not scheduler.is_scheduled(timeout)
