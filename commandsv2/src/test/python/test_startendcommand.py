from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_start_end_command_schedule(scheduler: commands2.CommandScheduler):
    cond1 = OOBoolean(False)
    cond2 = OOBoolean(False)

    command = commands2.StartEndCommand(
        lambda: cond1.set(True),
        lambda: cond2.set(True),
    )

    scheduler.schedule(command)
    scheduler.run()

    assert scheduler.is_scheduled(command)

    scheduler.cancel(command)

    assert not scheduler.is_scheduled(command)
    assert cond1 == True
    assert cond2 == True
