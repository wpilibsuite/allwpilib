from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_runCommandSchedule(scheduler: commands2.CommandScheduler):
    counter = OOInteger(0)

    command = commands2.RunCommand(counter.incrementAndGet)

    scheduler.schedule(command)
    scheduler.run()
    scheduler.run()
    scheduler.run()

    assert counter == 3
