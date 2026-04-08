from typing import TYPE_CHECKING

import commands2
from compositiontestbase import MultiCompositionTestBase  # type: ignore
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *
    from .compositiontestbase import MultiCompositionTestBase

import pytest


class TestSequentialCommandGroupComposition(MultiCompositionTestBase):
    def compose(self, *members: commands2.Command):
        return commands2.SequentialCommandGroup(*members)


def test_sequntialGroupSchedule(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()

    start_spying_on(command1)
    start_spying_on(command2)

    group = commands2.SequentialCommandGroup(command1, command2)

    scheduler.schedule(group)

    verify(command1).initialize()
    verify(command2, never()).initialize()

    command1.isFinished = lambda: True
    scheduler.run()

    verify(command1).execute()
    verify(command1).end(False)
    verify(command2).initialize()
    verify(command2, never()).execute()
    verify(command2, never()).end(False)

    command2.isFinished = lambda: True
    scheduler.run()

    verify(command1).execute()
    verify(command1).end(False)
    verify(command2).execute()
    verify(command2).end(False)

    assert not scheduler.isScheduled(group)


def test_sequentialGroupInterrupt(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()
    command3 = commands2.Command()

    start_spying_on(command1)
    start_spying_on(command2)
    start_spying_on(command3)

    group = commands2.SequentialCommandGroup(command1, command2, command3)

    scheduler.schedule(group)

    command1.isFinished = lambda: True
    scheduler.run()
    scheduler.cancel(group)
    scheduler.run()

    verify(command1).execute()
    verify(command1, never()).end(True)
    verify(command1).end(False)
    verify(command2, never()).execute()
    verify(command2).end(True)
    verify(command3, never()).initialize()
    verify(command3, never()).execute()

    # assert command3.end.times_called == 0
    verify(command3, never()).end(True)
    verify(command3, never()).end(False)

    assert not scheduler.isScheduled(group)


def test_notScheduledCancel(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()

    group = commands2.SequentialCommandGroup(command1, command2)

    scheduler.cancel(group)


def test_sequentialGroupRequirement(scheduler: commands2.CommandScheduler):
    system1 = commands2.Subsystem()
    system2 = commands2.Subsystem()
    system3 = commands2.Subsystem()
    system4 = commands2.Subsystem()

    command1 = commands2.InstantCommand()
    command1.addRequirements(system1, system2)
    command2 = commands2.InstantCommand()
    command2.addRequirements(system3)
    command3 = commands2.InstantCommand()
    command3.addRequirements(system3, system4)

    group = commands2.SequentialCommandGroup(command1, command2)

    scheduler.schedule(group)
    scheduler.schedule(command3)

    assert not scheduler.isScheduled(group)
    assert scheduler.isScheduled(command3)
