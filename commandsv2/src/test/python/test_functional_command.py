from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_functionalCommandSchedule(scheduler: commands2.CommandScheduler):
    cond1 = OOBoolean()
    cond2 = OOBoolean()
    cond3 = OOBoolean()
    cond4 = OOBoolean()

    command = commands2.FunctionalCommand(
        lambda: cond1.set(True),
        lambda: cond2.set(True),
        lambda _: cond3.set(True),
        lambda: cond4.get(),
    )

    scheduler.schedule(command)
    scheduler.run()

    assert scheduler.isScheduled(command)

    cond4.set(True)
    scheduler.run()

    assert not scheduler.isScheduled(command)
    assert cond1
    assert cond2
    assert cond3
