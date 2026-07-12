from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_default_command_schedule(scheduler: commands2.CommandScheduler):
    has_default_command = commands2.Subsystem()

    default_command = commands2.Command()
    default_command.add_requirements(has_default_command)

    scheduler.set_default_command(has_default_command, default_command)
    scheduler.run()

    assert scheduler.is_scheduled(default_command)


def test_default_command_interrupt_resume(scheduler: commands2.CommandScheduler):
    has_default_command = commands2.Subsystem()

    default_command = commands2.Command()
    default_command.add_requirements(has_default_command)

    interrupter = commands2.Command()
    interrupter.add_requirements(has_default_command)

    scheduler.set_default_command(has_default_command, default_command)
    scheduler.run()
    scheduler.schedule(interrupter)

    assert not scheduler.is_scheduled(default_command)
    assert scheduler.is_scheduled(interrupter)

    scheduler.cancel(interrupter)
    scheduler.run()

    assert scheduler.is_scheduled(default_command)
    assert not scheduler.is_scheduled(interrupter)


def test_default_command_disable_resume(scheduler: commands2.CommandScheduler):
    has_default_command = commands2.Subsystem()

    default_command = commands2.Command()
    default_command.add_requirements(has_default_command)
    default_command.runs_when_disabled = lambda: False

    start_spying_on(default_command)

    scheduler.set_default_command(has_default_command, default_command)
    scheduler.run()

    assert scheduler.is_scheduled(default_command)

    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()
    scheduler.run()

    assert not scheduler.is_scheduled(default_command)

    DriverStationSim.set_enabled(True)
    DriverStationSim.notify_new_data()
    scheduler.run()

    assert scheduler.is_scheduled(default_command)

    assert default_command.end.called_with(True)
