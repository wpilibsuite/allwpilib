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


def test_parallelGroupSchedule(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()

    start_spying_on(command1)
    start_spying_on(command2)

    group = commands2.ParallelCommandGroup(command1, command2)

    scheduler.schedule(group)

    verify(command1).initialize()
    verify(command2).initialize()

    command1.isFinished = lambda: True
    scheduler.run()
    command2.isFinished = lambda: True
    scheduler.run()

    verify(command1).execute()
    verify(command1).end(False)
    verify(command2, times(2)).execute()
    verify(command2).end(False)

    assert not scheduler.isScheduled(group)


def test_parallelGroupInterrupt(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()

    start_spying_on(command1)
    start_spying_on(command2)

    group = commands2.ParallelCommandGroup(command1, command2)

    scheduler.schedule(group)

    command1.isFinished = lambda: True
    scheduler.run()
    scheduler.run()
    scheduler.cancel(group)

    verify(command1).execute()
    verify(command1).end(False)
    verify(command1, never()).end(True)

    verify(command2, times(2)).execute()
    verify(command2, never()).end(False)
    verify(command2).end(True)

    assert not scheduler.isScheduled(group)


def test_notScheduledCancel(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()

    group = commands2.ParallelCommandGroup(command1, command2)

    scheduler.cancel(group)


def test_parallelGroupRequirement(scheduler: commands2.CommandScheduler):
    system1 = commands2.Subsystem()
    system2 = commands2.Subsystem()
    system3 = commands2.Subsystem()
    system4 = commands2.Subsystem()

    command1 = commands2.Command()
    command1.addRequirements(system1, system2)
    command2 = commands2.Command()
    command2.addRequirements(system3)
    command3 = commands2.Command()
    command3.addRequirements(system3, system4)

    group = commands2.ParallelCommandGroup(command1, command2)

    scheduler.schedule(group)
    scheduler.schedule(command3)

    assert not scheduler.isScheduled(group)
    assert scheduler.isScheduled(command3)


def test_parallelGroupRequirementError():
    system1 = commands2.Subsystem()
    system2 = commands2.Subsystem()
    system3 = commands2.Subsystem()

    command1 = commands2.Command()
    command1.addRequirements(system1, system2)
    command2 = commands2.Command()
    command2.addRequirements(system2, system3)

    with pytest.raises(commands2.IllegalCommandUse):
        commands2.ParallelCommandGroup(command1, command2)
