from typing import TYPE_CHECKING

import commands2
from compositiontestbase import MultiCompositionTestBase  # type: ignore
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *
    from .compositiontestbase import MultiCompositionTestBase

import pytest


class TestSelectCommandComposition(MultiCompositionTestBase):
    def compose(self, *members: commands2.Command):
        return commands2.SelectCommand(dict(enumerate(members)), lambda: 0)


def test_selectCommand(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()
    command3 = commands2.Command()

    start_spying_on(command1)
    start_spying_on(command2)
    start_spying_on(command3)

    command1.isFinished = lambda: True

    selectCommand = commands2.SelectCommand(
        {"one": command1, "two": command2, "three": command3}, lambda: "one"
    )

    scheduler.schedule(selectCommand)
    scheduler.run()

    verify(command1).initialize()
    verify(command1).execute()
    verify(command1).end(False)

    verify(command2, never()).initialize()
    verify(command2, never()).execute()
    verify(command2, never()).end(False)

    verify(command3, never()).initialize()
    verify(command3, never()).execute()
    verify(command3, never()).end(False)


def test_selectCommandInvalidKey(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()
    command3 = commands2.Command()

    start_spying_on(command1)
    start_spying_on(command2)
    start_spying_on(command3)

    command1.isFinished = lambda: True

    selectCommand = commands2.SelectCommand(
        {"one": command1, "two": command2, "three": command3}, lambda: "four"
    )

    scheduler.schedule(selectCommand)


def test_selectCommandRequirement(scheduler: commands2.CommandScheduler):
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

    start_spying_on(command1)
    start_spying_on(command2)
    start_spying_on(command3)

    selectCommand = commands2.SelectCommand(
        {"one": command1, "two": command2, "three": command3}, lambda: "one"
    )

    scheduler.schedule(selectCommand)
    scheduler.schedule(commands2.InstantCommand(lambda: None, system3))

    verify(command1).end(interrupted=True)
    verify(command2, never()).end(interrupted=True)
    verify(command3, never()).end(interrupted=True)
