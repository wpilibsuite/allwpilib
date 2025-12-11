from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_commandInMultipleGroups():
    command1 = commands2.Command()
    command2 = commands2.Command()

    commands2.ParallelCommandGroup(command1, command2)
    with pytest.raises(commands2.IllegalCommandUse):
        commands2.ParallelCommandGroup(command1, command2)


def test_commandInGroupExternallyScheduled(scheduler: commands2.CommandScheduler):
    command1 = commands2.Command()
    command2 = commands2.Command()

    commands2.ParallelCommandGroup(command1, command2)

    with pytest.raises(commands2.IllegalCommandUse):
        scheduler.schedule(command1)


def test_redecoratedCommandError(scheduler: commands2.CommandScheduler):
    command = commands2.InstantCommand()
    command.withTimeout(10).until(lambda: False)
    with pytest.raises(commands2.IllegalCommandUse):
        command.withTimeout(10)
    scheduler.removeComposedCommand(command)
    command.withTimeout(10)
