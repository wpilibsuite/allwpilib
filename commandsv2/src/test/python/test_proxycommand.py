from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_proxy_command_schedule(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    start_spying_on(command1)

    schedule_command = commands2.ProxyCommand(command1)

    scheduler.schedule(schedule_command)

    verify(command1).schedule()


def test_proxy_command_end(scheduler: commands2.CommandScheduler):
    cond = OOBoolean()

    command = commands2.WaitUntilCommand(cond)

    schedule_command = commands2.ProxyCommand(command)

    scheduler.schedule(schedule_command)
    scheduler.run()

    assert scheduler.is_scheduled(schedule_command)

    cond.set(True)
    scheduler.run()
    scheduler.run()
    assert not scheduler.is_scheduled(schedule_command)
