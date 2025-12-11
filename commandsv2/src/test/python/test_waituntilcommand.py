from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_waitUntil(scheduler: commands2.CommandScheduler):
    condition = OOBoolean()

    command = commands2.WaitUntilCommand(condition)

    scheduler.schedule(command)
    scheduler.run()
    assert scheduler.isScheduled(command)
    condition.set(True)
    scheduler.run()
    assert not scheduler.isScheduled(command)
