from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_notifierCommandScheduler(scheduler: commands2.CommandScheduler):
    with ManualSimTime() as sim:
        counter = OOInteger(0)
        command = commands2.NotifierCommand(counter.incrementAndGet, 0.01)

        scheduler.schedule(command)
        for i in range(5):
            sim.step(0.005)
        scheduler.cancel(command)

        assert counter == 2
