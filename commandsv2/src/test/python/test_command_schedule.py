from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_instant_schedule(scheduler: commands2.CommandScheduler):
    command = commands2.Command()
    command.is_finished = lambda: True
    start_spying_on(command)

    scheduler.schedule(command)
    assert scheduler.is_scheduled(command)
    verify(command).initialize()

    scheduler.run()

    verify(command).execute()
    verify(command).end(False)
    assert not scheduler.is_scheduled(command)


def test_single_iteration_schedule(scheduler: commands2.CommandScheduler):
    command = commands2.Command()
    start_spying_on(command)

    scheduler.schedule(command)
    assert scheduler.is_scheduled(command)

    scheduler.run()
    command.is_finished = lambda: True
    scheduler.run()

    verify(command).initialize()
    verify(command, times(2)).execute()
    verify(command).end(False)
    assert not scheduler.is_scheduled(command)


def test_multi_schedule(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()
    command3 = commands2.Command()

    scheduler.schedule(command1, command2, command3)
    assert scheduler.is_scheduled(command1, command2, command3)
    scheduler.run()
    assert scheduler.is_scheduled(command1, command2, command3)

    command1.is_finished = lambda: True
    scheduler.run()
    assert scheduler.is_scheduled(command2, command3)
    assert not scheduler.is_scheduled(command1)

    command2.is_finished = lambda: True
    scheduler.run()
    assert scheduler.is_scheduled(command3)
    assert not scheduler.is_scheduled(command1, command2)

    command3.is_finished = lambda: True
    scheduler.run()
    assert not scheduler.is_scheduled(command1, command2, command3)


def test_scheduler_cancel(scheduler: commands2.CommandScheduler):
    command = commands2.Command()
    start_spying_on(command)

    scheduler.schedule(command)

    scheduler.run()
    scheduler.cancel(command)
    scheduler.run()

    verify(command).execute()
    verify(command).end(True)
    verify(command, never()).end(False)

    assert not scheduler.is_scheduled(command)


def test_not_scheduled_cancel(scheduler: commands2.CommandScheduler):
    command = commands2.Command()

    scheduler.cancel(command)
