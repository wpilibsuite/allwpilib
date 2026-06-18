from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_defaultCommandSchedule(scheduler: commands2.CommandScheduler):
    hasDefaultCommand = commands2.Subsystem()

    defaultCommand = commands2.Command()
    defaultCommand.addRequirements(hasDefaultCommand)

    scheduler.setDefaultCommand(hasDefaultCommand, defaultCommand)
    scheduler.run()

    assert scheduler.isScheduled(defaultCommand)


def test_defaultCommandInterruptResume(scheduler: commands2.CommandScheduler):
    hasDefaultCommand = commands2.Subsystem()

    defaultCommand = commands2.Command()
    defaultCommand.addRequirements(hasDefaultCommand)

    interrupter = commands2.Command()
    interrupter.addRequirements(hasDefaultCommand)

    scheduler.setDefaultCommand(hasDefaultCommand, defaultCommand)
    scheduler.run()
    scheduler.schedule(interrupter)

    assert not scheduler.isScheduled(defaultCommand)
    assert scheduler.isScheduled(interrupter)

    scheduler.cancel(interrupter)
    scheduler.run()

    assert scheduler.isScheduled(defaultCommand)
    assert not scheduler.isScheduled(interrupter)


def test_defaultCommandDisableResume(scheduler: commands2.CommandScheduler):
    hasDefaultCommand = commands2.Subsystem()

    defaultCommand = commands2.Command()
    defaultCommand.addRequirements(hasDefaultCommand)
    defaultCommand.runsWhenDisabled = lambda: False

    start_spying_on(defaultCommand)

    scheduler.setDefaultCommand(hasDefaultCommand, defaultCommand)
    scheduler.run()

    assert scheduler.isScheduled(defaultCommand)

    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()
    scheduler.run()

    assert not scheduler.isScheduled(defaultCommand)

    DriverStationSim.setEnabled(True)
    DriverStationSim.notifyNewData()
    scheduler.run()

    assert scheduler.isScheduled(defaultCommand)

    assert defaultCommand.end.called_with(True)
