from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_scheduleCommandSchedule(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()

    start_spying_on(command1)
    start_spying_on(command2)

    scheduleCommand = commands2.ScheduleCommand(command1, command2)

    scheduler.schedule(scheduleCommand)

    verify(command1).schedule()
    verify(command2).schedule()


def test_scheduleCommandDruingRun(scheduler: commands2.CommandScheduler):
    toSchedule = commands2.InstantCommand()
    scheduleCommand = commands2.ScheduleCommand(toSchedule)
    group = commands2.SequentialCommandGroup(
        commands2.InstantCommand(), scheduleCommand
    )

    scheduler.schedule(group)
    scheduler.schedule(commands2.RunCommand(lambda: None))
    scheduler.run()
