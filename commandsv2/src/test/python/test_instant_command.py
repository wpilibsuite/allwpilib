from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_instant_command_schedule(scheduler: commands2.CommandScheduler):
    cond = OOBoolean()

    command = commands2.InstantCommand(lambda: cond.set(True))

    scheduler.schedule(command)
    scheduler.run()

    assert cond
    assert not scheduler.is_scheduled(command)
