from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest
from wpilib import RobotState


def test_robotDisabledCommandCancel(scheduler: commands2.CommandScheduler):
    command = commands2.Command()
    scheduler.schedule(command)
    assert scheduler.isScheduled(command)
    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()
    scheduler.run()
    assert not scheduler.isScheduled(command)
    DriverStationSim.setEnabled(True)
    DriverStationSim.notifyNewData()


def test_runWhenDisabled(scheduler: commands2.CommandScheduler):
    command = commands2.Command()
    command.runsWhenDisabled = lambda: True

    scheduler.schedule(command)

    assert scheduler.isScheduled(command)

    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()

    scheduler.run()

    assert scheduler.isScheduled(command)


def test_sequentialGroupRunWhenDisabled(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command1.runsWhenDisabled = lambda: True
    command2 = commands2.Command()
    command2.runsWhenDisabled = lambda: True
    command3 = commands2.Command()
    command3.runsWhenDisabled = lambda: True
    command4 = commands2.Command()
    command4.runsWhenDisabled = lambda: False

    runWhenDisabled = commands2.SequentialCommandGroup(command1, command2)
    dontRunWhenDisabled = commands2.SequentialCommandGroup(command3, command4)

    scheduler.schedule(runWhenDisabled)
    scheduler.schedule(dontRunWhenDisabled)

    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()

    scheduler.run()

    assert scheduler.isScheduled(runWhenDisabled)
    assert not scheduler.isScheduled(dontRunWhenDisabled)


def test_parallelGroupRunWhenDisabled(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command1.runsWhenDisabled = lambda: True
    command2 = commands2.Command()
    command2.runsWhenDisabled = lambda: True
    command3 = commands2.Command()
    command3.runsWhenDisabled = lambda: True
    command4 = commands2.Command()
    command4.runsWhenDisabled = lambda: False

    runWhenDisabled = commands2.ParallelCommandGroup(command1, command2)
    dontRunWhenDisabled = commands2.ParallelCommandGroup(command3, command4)

    scheduler.schedule(runWhenDisabled)
    scheduler.schedule(dontRunWhenDisabled)

    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()

    scheduler.run()

    assert scheduler.isScheduled(runWhenDisabled)
    assert not scheduler.isScheduled(dontRunWhenDisabled)


def test_conditionalRunWhenDisabled(scheduler: commands2.CommandScheduler):
    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()

    command1 = commands2.Command()
    command1.runsWhenDisabled = lambda: True
    command2 = commands2.Command()
    command2.runsWhenDisabled = lambda: True
    command3 = commands2.Command()
    command3.runsWhenDisabled = lambda: True
    command4 = commands2.Command()
    command4.runsWhenDisabled = lambda: False

    runWhenDisabled = commands2.ConditionalCommand(command1, command2, lambda: True)
    dontRunWhenDisabled = commands2.ConditionalCommand(command3, command4, lambda: True)

    scheduler.schedule(runWhenDisabled, dontRunWhenDisabled)

    assert scheduler.isScheduled(runWhenDisabled)
    assert not scheduler.isScheduled(dontRunWhenDisabled)


def test_selectRunWhenDisabled(scheduler: commands2.CommandScheduler):
    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()

    command1 = commands2.Command()
    command1.runsWhenDisabled = lambda: True
    command2 = commands2.Command()
    command2.runsWhenDisabled = lambda: True
    command3 = commands2.Command()
    command3.runsWhenDisabled = lambda: True
    command4 = commands2.Command()
    command4.runsWhenDisabled = lambda: False

    runWhenDisabled = commands2.SelectCommand({1: command1, 2: command2}, lambda: 1)
    dontRunWhenDisabled = commands2.SelectCommand({1: command3, 2: command4}, lambda: 1)

    scheduler.schedule(runWhenDisabled, dontRunWhenDisabled)
    assert scheduler.isScheduled(runWhenDisabled)
    assert not scheduler.isScheduled(dontRunWhenDisabled)


def test_parallelConditionalRunWhenDisabledTest(scheduler: commands2.CommandScheduler):
    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()

    command1 = commands2.Command()
    command1.runsWhenDisabled = lambda: True
    command2 = commands2.Command()
    command2.runsWhenDisabled = lambda: True
    command3 = commands2.Command()
    command3.runsWhenDisabled = lambda: True
    command4 = commands2.Command()
    command4.runsWhenDisabled = lambda: False

    runWhenDisabled = commands2.ConditionalCommand(command1, command2, lambda: True)
    dontRunWhenDisabled = commands2.ConditionalCommand(command3, command4, lambda: True)

    parallel = commands2.cmd.parallel(runWhenDisabled, dontRunWhenDisabled)

    scheduler.schedule(parallel)

    assert not scheduler.isScheduled(runWhenDisabled)
