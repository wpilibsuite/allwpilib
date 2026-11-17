from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_requirement_interrupt(scheduler: commands2.CommandScheduler):
    requirement = commands2.Subsystem()
    interrupted = commands2.Command()
    interrupted.add_requirements(requirement)
    interrupter = commands2.Command()
    interrupter.add_requirements(requirement)
    start_spying_on(interrupted)
    start_spying_on(interrupter)

    scheduler.schedule(interrupted)
    scheduler.run()
    scheduler.schedule(interrupter)
    scheduler.run()

    verify(interrupted).initialize()
    verify(interrupted).execute()
    verify(interrupted).end(True)

    verify(interrupter).initialize()
    verify(interrupter).execute()

    assert not interrupted.is_scheduled()
    assert interrupter.is_scheduled()


def test_requirement_uninterruptible(scheduler: commands2.CommandScheduler):
    requirement = commands2.Subsystem()
    not_interrupted = commands2.RunCommand(
        lambda: None, requirement
    ).with_interrupt_behavior(commands2.InterruptionBehavior.CANCEL_INCOMING)
    interrupter = commands2.Command()
    interrupter.add_requirements(requirement)
    start_spying_on(not_interrupted)

    scheduler.schedule(not_interrupted)
    scheduler.schedule(interrupter)

    assert scheduler.is_scheduled(not_interrupted)
    assert not scheduler.is_scheduled(interrupter)


def test_default_command_requirement_error(scheduler: commands2.CommandScheduler):
    system = commands2.Subsystem()
    missing_requirement = commands2.WaitUntilCommand(lambda: False)

    with pytest.raises(commands2.IllegalCommandUse):
        scheduler.set_default_command(system, missing_requirement)
