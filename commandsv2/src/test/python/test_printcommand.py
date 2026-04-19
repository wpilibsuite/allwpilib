from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_printCommandSchedule(capsys, scheduler: commands2.CommandScheduler):
    command = commands2.PrintCommand("Test!")
    scheduler.schedule(command)
    scheduler.run()
    assert not scheduler.isScheduled(command)
    assert capsys.readouterr().out == "Test!\n"
