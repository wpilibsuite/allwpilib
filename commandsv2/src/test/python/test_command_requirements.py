from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_requirementInterrupt(scheduler: commands2.CommandScheduler):
    requirement = commands2.Subsystem()
    interrupted = commands2.Command()
    interrupted.addRequirements(requirement)
    interrupter = commands2.Command()
    interrupter.addRequirements(requirement)
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

    assert not interrupted.isScheduled()
    assert interrupter.isScheduled()


def test_requirementUninterruptible(scheduler: commands2.CommandScheduler):
    requirement = commands2.Subsystem()
    notInterrupted = commands2.RunCommand(
        lambda: None, requirement
    ).withInterruptBehavior(commands2.InterruptionBehavior.kCancelIncoming)
    interrupter = commands2.Command()
    interrupter.addRequirements(requirement)
    start_spying_on(notInterrupted)

    scheduler.schedule(notInterrupted)
    scheduler.schedule(interrupter)

    assert scheduler.isScheduled(notInterrupted)
    assert not scheduler.isScheduled(interrupter)


def test_defaultCommandRequirementError(scheduler: commands2.CommandScheduler):
    system = commands2.Subsystem()
    missingRequirement = commands2.WaitUntilCommand(lambda: False)

    with pytest.raises(commands2.IllegalCommandUse):
        scheduler.setDefaultCommand(system, missingRequirement)
