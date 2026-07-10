from typing import TYPE_CHECKING

import commands2
from compositiontestbase import MultiCompositionTestBase  # type: ignore
from util import *  # type: ignore

# from tests.compositiontestbase import T

if TYPE_CHECKING:
    from .util import *
    from .compositiontestbase import MultiCompositionTestBase

import pytest


class TestParallelCommandGroupComposition(MultiCompositionTestBase):
    def compose(self, *members: commands2.Command):
        return commands2.ParallelCommandGroup(*members)


def test_parallel_group_schedule(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()

    start_spying_on(command1)
    start_spying_on(command2)

    group = commands2.ParallelCommandGroup(command1, command2)

    scheduler.schedule(group)

    verify(command1).initialize()
    verify(command2).initialize()

    command1.is_finished = lambda: True
    scheduler.run()
    command2.is_finished = lambda: True
    scheduler.run()

    verify(command1).execute()
    verify(command1).end(False)
    verify(command2, times(2)).execute()
    verify(command2).end(False)

    assert not scheduler.is_scheduled(group)


def test_parallel_group_interrupt(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()

    start_spying_on(command1)
    start_spying_on(command2)

    group = commands2.ParallelCommandGroup(command1, command2)

    scheduler.schedule(group)

    command1.is_finished = lambda: True
    scheduler.run()
    scheduler.run()
    scheduler.cancel(group)

    verify(command1).execute()
    verify(command1).end(False)
    verify(command1, never()).end(True)

    verify(command2, times(2)).execute()
    verify(command2, never()).end(False)
    verify(command2).end(True)

    assert not scheduler.is_scheduled(group)


def test_not_scheduled_cancel(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()

    group = commands2.ParallelCommandGroup(command1, command2)

    scheduler.cancel(group)


def test_parallel_group_requirement(scheduler: commands2.CommandScheduler):
    system1 = commands2.Subsystem()
    system2 = commands2.Subsystem()
    system3 = commands2.Subsystem()
    system4 = commands2.Subsystem()

    command1 = commands2.Command()
    command1.add_requirements(system1, system2)
    command2 = commands2.Command()
    command2.add_requirements(system3)
    command3 = commands2.Command()
    command3.add_requirements(system3, system4)

    group = commands2.ParallelCommandGroup(command1, command2)

    scheduler.schedule(group)
    scheduler.schedule(command3)

    assert not scheduler.is_scheduled(group)
    assert scheduler.is_scheduled(command3)


def test_parallel_group_requirement_error():
    system1 = commands2.Subsystem()
    system2 = commands2.Subsystem()
    system3 = commands2.Subsystem()

    command1 = commands2.Command()
    command1.add_requirements(system1, system2)
    command2 = commands2.Command()
    command2.add_requirements(system2, system3)

    with pytest.raises(commands2.IllegalCommandUse):
        commands2.ParallelCommandGroup(command1, command2)
