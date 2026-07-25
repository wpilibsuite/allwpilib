from typing import TYPE_CHECKING

import commands2
from compositiontestbase import MultiCompositionTestBase  # type: ignore
from util import *  # type: ignore

# from tests.compositiontestbase import T

if TYPE_CHECKING:
    from .util import *
    from .compositiontestbase import MultiCompositionTestBase

import pytest


class TestParallelDeadlineGroupComposition(MultiCompositionTestBase):
    def compose(self, *members: commands2.Command):
        return commands2.ParallelDeadlineGroup(members[0], *members[1:])


def test_parallel_deadline_schedule(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()
    command2.is_finished = lambda: True
    command3 = commands2.Command()

    start_spying_on(command1)
    start_spying_on(command2)
    start_spying_on(command3)

    group = commands2.ParallelDeadlineGroup(command1, command2, command3)

    scheduler.schedule(group)
    scheduler.run()

    assert scheduler.is_scheduled(group)

    command1.is_finished = lambda: True
    scheduler.run()

    assert not scheduler.is_scheduled(group)

    verify(command2).initialize()
    verify(command2).execute()
    verify(command2).end(False)
    verify(command2, never()).end(True)

    verify(command1).initialize()
    verify(command1, times(2)).execute()
    verify(command1).end(False)
    verify(command1, never()).end(True)

    verify(command3).initialize()
    verify(command3, times(2)).execute()
    verify(command3, never()).end(False)
    verify(command3).end(True)


def test_parallel_deadline_interrupt(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()
    command2.is_finished = lambda: True

    start_spying_on(command1)
    start_spying_on(command2)

    group = commands2.ParallelDeadlineGroup(command1, command2)

    scheduler.schedule(group)

    scheduler.run()
    scheduler.run()
    scheduler.cancel(group)

    verify(command1, times(2)).execute()
    verify(command1, never()).end(False)
    verify(command1).end(True)

    verify(command2).execute()
    verify(command2).end(False)
    verify(command2, never()).end(True)

    assert not scheduler.is_scheduled(group)


def test_parallel_deadline_requirement(scheduler: commands2.CommandScheduler):
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

    group = commands2.ParallelDeadlineGroup(command1, command2)

    scheduler.schedule(group)
    scheduler.schedule(command3)

    assert not scheduler.is_scheduled(group)
    assert scheduler.is_scheduled(command3)


def test_parallel_deadline_requirement_error(scheduler: commands2.CommandScheduler):
    system1 = commands2.Subsystem()
    system2 = commands2.Subsystem()
    system3 = commands2.Subsystem()

    command1 = commands2.Command()
    command1.add_requirements(system1, system2)
    command2 = commands2.Command()
    command2.add_requirements(system2, system3)

    with pytest.raises(commands2.IllegalCommandUse):
        commands2.ParallelDeadlineGroup(command1, command2)
