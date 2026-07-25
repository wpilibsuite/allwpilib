from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_robot_disabled_command_cancel(scheduler: commands2.CommandScheduler):
    command = commands2.Command()
    scheduler.schedule(command)
    assert scheduler.is_scheduled(command)
    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()
    scheduler.run()
    assert not scheduler.is_scheduled(command)
    DriverStationSim.set_enabled(True)
    DriverStationSim.notify_new_data()


def test_run_when_disabled(scheduler: commands2.CommandScheduler):
    command = commands2.Command()
    command.runs_when_disabled = lambda: True

    scheduler.schedule(command)

    assert scheduler.is_scheduled(command)

    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()

    scheduler.run()

    assert scheduler.is_scheduled(command)


def test_sequential_group_run_when_disabled(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command1.runs_when_disabled = lambda: True
    command2 = commands2.Command()
    command2.runs_when_disabled = lambda: True
    command3 = commands2.Command()
    command3.runs_when_disabled = lambda: True
    command4 = commands2.Command()
    command4.runs_when_disabled = lambda: False

    run_when_disabled = commands2.SequentialCommandGroup(command1, command2)
    dont_run_when_disabled = commands2.SequentialCommandGroup(command3, command4)

    scheduler.schedule(run_when_disabled)
    scheduler.schedule(dont_run_when_disabled)

    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()

    scheduler.run()

    assert scheduler.is_scheduled(run_when_disabled)
    assert not scheduler.is_scheduled(dont_run_when_disabled)


def test_parallel_group_run_when_disabled(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command1.runs_when_disabled = lambda: True
    command2 = commands2.Command()
    command2.runs_when_disabled = lambda: True
    command3 = commands2.Command()
    command3.runs_when_disabled = lambda: True
    command4 = commands2.Command()
    command4.runs_when_disabled = lambda: False

    run_when_disabled = commands2.ParallelCommandGroup(command1, command2)
    dont_run_when_disabled = commands2.ParallelCommandGroup(command3, command4)

    scheduler.schedule(run_when_disabled)
    scheduler.schedule(dont_run_when_disabled)

    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()

    scheduler.run()

    assert scheduler.is_scheduled(run_when_disabled)
    assert not scheduler.is_scheduled(dont_run_when_disabled)


def test_conditional_run_when_disabled(scheduler: commands2.CommandScheduler):
    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()

    command1 = commands2.Command()
    command1.runs_when_disabled = lambda: True
    command2 = commands2.Command()
    command2.runs_when_disabled = lambda: True
    command3 = commands2.Command()
    command3.runs_when_disabled = lambda: True
    command4 = commands2.Command()
    command4.runs_when_disabled = lambda: False

    run_when_disabled = commands2.ConditionalCommand(command1, command2, lambda: True)
    dont_run_when_disabled = commands2.ConditionalCommand(
        command3, command4, lambda: True
    )

    scheduler.schedule(run_when_disabled, dont_run_when_disabled)

    assert scheduler.is_scheduled(run_when_disabled)
    assert not scheduler.is_scheduled(dont_run_when_disabled)


def test_select_run_when_disabled(scheduler: commands2.CommandScheduler):
    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()

    command1 = commands2.Command()
    command1.runs_when_disabled = lambda: True
    command2 = commands2.Command()
    command2.runs_when_disabled = lambda: True
    command3 = commands2.Command()
    command3.runs_when_disabled = lambda: True
    command4 = commands2.Command()
    command4.runs_when_disabled = lambda: False

    run_when_disabled = commands2.SelectCommand({1: command1, 2: command2}, lambda: 1)
    dont_run_when_disabled = commands2.SelectCommand(
        {1: command3, 2: command4}, lambda: 1
    )

    scheduler.schedule(run_when_disabled, dont_run_when_disabled)
    assert scheduler.is_scheduled(run_when_disabled)
    assert not scheduler.is_scheduled(dont_run_when_disabled)


def test_parallel_conditional_run_when_disabled_test(
    scheduler: commands2.CommandScheduler,
):
    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()

    command1 = commands2.Command()
    command1.runs_when_disabled = lambda: True
    command2 = commands2.Command()
    command2.runs_when_disabled = lambda: True
    command3 = commands2.Command()
    command3.runs_when_disabled = lambda: True
    command4 = commands2.Command()
    command4.runs_when_disabled = lambda: False

    run_when_disabled = commands2.ConditionalCommand(command1, command2, lambda: True)
    dont_run_when_disabled = commands2.ConditionalCommand(
        command3, command4, lambda: True
    )

    parallel = commands2.cmd.parallel(run_when_disabled, dont_run_when_disabled)

    scheduler.schedule(parallel)

    assert not scheduler.is_scheduled(run_when_disabled)
