from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


@pytest.mark.parametrize(
    "interruptionBehavior",
    [
        commands2.InterruptionBehavior.kCancelIncoming,
        commands2.InterruptionBehavior.kCancelSelf,
    ],
)
def test_cancelFromInitialize(
    interruptionBehavior: commands2.InterruptionBehavior,
    scheduler: commands2.CommandScheduler,
):
    hasOtherRun = OOBoolean()
    requirement = commands2.Subsystem()

    selfCancels = commands2.Command()
    selfCancels.addRequirements(requirement)
    selfCancels.getInterruptionBehavior = lambda: interruptionBehavior
    selfCancels.initialize = lambda: scheduler.cancel(selfCancels)

    other = commands2.RunCommand(lambda: hasOtherRun.set(True), requirement)

    scheduler.schedule(selfCancels)
    scheduler.run()
    scheduler.schedule(other)

    assert not scheduler.isScheduled(selfCancels)
    assert scheduler.isScheduled(other)
    scheduler.run()
    assert hasOtherRun == True


@pytest.mark.parametrize(
    "interruptionBehavior",
    [
        commands2.InterruptionBehavior.kCancelIncoming,
        commands2.InterruptionBehavior.kCancelSelf,
    ],
)
def test_defaultCommandGetsRescheduledAfterSelfCanceling(
    interruptionBehavior: commands2.InterruptionBehavior,
    scheduler: commands2.CommandScheduler,
):
    hasOtherRun = OOBoolean()
    requirement = commands2.Subsystem()

    selfCancels = commands2.Command()
    selfCancels.addRequirements(requirement)
    selfCancels.getInterruptionBehavior = lambda: interruptionBehavior
    selfCancels.initialize = lambda: scheduler.cancel(selfCancels)

    other = commands2.RunCommand(lambda: hasOtherRun.set(True), requirement)
    scheduler.setDefaultCommand(requirement, other)

    scheduler.schedule(selfCancels)
    scheduler.run()

    scheduler.run()
    assert not scheduler.isScheduled(selfCancels)
    assert scheduler.isScheduled(other)
    scheduler.run()
    assert hasOtherRun == True


def test_cancelFromEnd(scheduler: commands2.CommandScheduler):
    counter = OOInteger()
    selfCancels = commands2.Command()

    @patch_via_decorator(selfCancels)
    def end(self, interrupted):
        counter.incrementAndGet()
        scheduler.cancel(self)

    scheduler.schedule(selfCancels)

    scheduler.cancel(selfCancels)
    assert counter == 1
    assert not scheduler.isScheduled(selfCancels)


def test_scheduleFromEnd(scheduler: commands2.CommandScheduler):
    counter = OOInteger()
    requirement = commands2.Subsystem()
    other = commands2.InstantCommand(lambda: None, requirement)

    selfCancels = commands2.Command()
    selfCancels.addRequirements(requirement)

    @patch_via_decorator(selfCancels)
    def end(self, interrupted):
        counter.incrementAndGet()
        scheduler.schedule(other)

    scheduler.schedule(selfCancels)

    scheduler.cancel(selfCancels)
    assert counter == 1
    assert not scheduler.isScheduled(selfCancels)


def test_scheduleFromEndInterrupt(scheduler: commands2.CommandScheduler):
    counter = OOInteger()
    requirement = commands2.Subsystem()
    other = commands2.InstantCommand(lambda: None, requirement)

    selfCancels = commands2.Command()
    selfCancels.addRequirements(requirement)

    @patch_via_decorator(selfCancels)
    def end(self, interrupted):
        counter.incrementAndGet()
        scheduler.schedule(other)

    scheduler.schedule(selfCancels)

    scheduler.schedule(other)
    assert counter == 1
    assert not scheduler.isScheduled(selfCancels)
    assert scheduler.isScheduled(other)


@pytest.mark.parametrize(
    "interruptionBehavior",
    [
        commands2.InterruptionBehavior.kCancelIncoming,
        commands2.InterruptionBehavior.kCancelSelf,
    ],
)
def test_scheduleInitializeFromDefaultCommand(
    interruptionBehavior: commands2.InterruptionBehavior,
    scheduler: commands2.CommandScheduler,
):
    counter = OOInteger()
    requirement = commands2.Subsystem()
    other = commands2.InstantCommand(lambda: None, requirement).withInterruptBehavior(
        interruptionBehavior
    )

    defaultCommand = commands2.Command()
    defaultCommand.addRequirements(requirement)

    @patch_via_decorator(defaultCommand)
    def initialize(self):
        counter.incrementAndGet()
        scheduler.schedule(other)

    scheduler.setDefaultCommand(requirement, defaultCommand)

    scheduler.run()
    scheduler.run()
    scheduler.run()

    assert counter == 3
    assert not scheduler.isScheduled(defaultCommand)
    assert scheduler.isScheduled(other)
