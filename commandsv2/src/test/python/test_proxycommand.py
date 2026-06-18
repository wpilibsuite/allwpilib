from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_proxyCommandSchedule(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    start_spying_on(command1)

    scheduleCommand = commands2.ProxyCommand(command1)

    scheduler.schedule(scheduleCommand)

    verify(command1).schedule()


def test_proxyCommandEnd(scheduler: commands2.CommandScheduler):
    cond = OOBoolean()

    command = commands2.WaitUntilCommand(cond)

    scheduleCommand = commands2.ProxyCommand(command)

    scheduler.schedule(scheduleCommand)
    scheduler.run()

    assert scheduler.isScheduled(scheduleCommand)

    cond.set(True)
    scheduler.run()
    scheduler.run()
    assert not scheduler.isScheduled(scheduleCommand)
