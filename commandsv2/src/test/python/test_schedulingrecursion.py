from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


@pytest.mark.parametrize(
    "interruption_behavior",
    [
        commands2.InterruptionBehavior.CANCEL_INCOMING,
        commands2.InterruptionBehavior.CANCEL_SELF,
    ],
)
def test_cancel_from_initialize(
    interruption_behavior: commands2.InterruptionBehavior,
    scheduler: commands2.CommandScheduler,
):
    has_other_run = OOBoolean()
    requirement = commands2.Subsystem()

    self_cancels = commands2.Command()
    self_cancels.add_requirements(requirement)
    self_cancels.get_interruption_behavior = lambda: interruption_behavior
    self_cancels.initialize = lambda: scheduler.cancel(self_cancels)

    other = commands2.RunCommand(lambda: has_other_run.set(True), requirement)

    scheduler.schedule(self_cancels)
    scheduler.run()
    scheduler.schedule(other)

    assert not scheduler.is_scheduled(self_cancels)
    assert scheduler.is_scheduled(other)
    scheduler.run()
    assert has_other_run == True


@pytest.mark.parametrize(
    "interruption_behavior",
    [
        commands2.InterruptionBehavior.CANCEL_INCOMING,
        commands2.InterruptionBehavior.CANCEL_SELF,
    ],
)
def test_default_command_gets_rescheduled_after_self_canceling(
    interruption_behavior: commands2.InterruptionBehavior,
    scheduler: commands2.CommandScheduler,
):
    has_other_run = OOBoolean()
    requirement = commands2.Subsystem()

    self_cancels = commands2.Command()
    self_cancels.add_requirements(requirement)
    self_cancels.get_interruption_behavior = lambda: interruption_behavior
    self_cancels.initialize = lambda: scheduler.cancel(self_cancels)

    other = commands2.RunCommand(lambda: has_other_run.set(True), requirement)
    scheduler.set_default_command(requirement, other)

    scheduler.schedule(self_cancels)
    scheduler.run()

    scheduler.run()
    assert not scheduler.is_scheduled(self_cancels)
    assert scheduler.is_scheduled(other)
    scheduler.run()
    assert has_other_run == True


def test_cancel_from_end(scheduler: commands2.CommandScheduler):
    counter = OOInteger()
    self_cancels = commands2.Command()

    @patch_via_decorator(self_cancels)
    def end(self, interrupted):
        counter.increment_and_get()
        scheduler.cancel(self)

    scheduler.schedule(self_cancels)

    scheduler.cancel(self_cancels)
    assert counter == 1
    assert not scheduler.is_scheduled(self_cancels)


def test_schedule_from_end(scheduler: commands2.CommandScheduler):
    counter = OOInteger()
    requirement = commands2.Subsystem()
    other = commands2.InstantCommand(lambda: None, requirement)

    self_cancels = commands2.Command()
    self_cancels.add_requirements(requirement)

    @patch_via_decorator(self_cancels)
    def end(self, interrupted):
        counter.increment_and_get()
        scheduler.schedule(other)

    scheduler.schedule(self_cancels)

    scheduler.cancel(self_cancels)
    assert counter == 1
    assert not scheduler.is_scheduled(self_cancels)


def test_schedule_from_end_interrupt(scheduler: commands2.CommandScheduler):
    counter = OOInteger()
    requirement = commands2.Subsystem()
    other = commands2.InstantCommand(lambda: None, requirement)

    self_cancels = commands2.Command()
    self_cancels.add_requirements(requirement)

    @patch_via_decorator(self_cancels)
    def end(self, interrupted):
        counter.increment_and_get()
        scheduler.schedule(other)

    scheduler.schedule(self_cancels)

    scheduler.schedule(other)
    assert counter == 1
    assert not scheduler.is_scheduled(self_cancels)
    assert scheduler.is_scheduled(other)


@pytest.mark.parametrize(
    "interruption_behavior",
    [
        commands2.InterruptionBehavior.CANCEL_INCOMING,
        commands2.InterruptionBehavior.CANCEL_SELF,
    ],
)
def test_schedule_initialize_from_default_command(
    interruption_behavior: commands2.InterruptionBehavior,
    scheduler: commands2.CommandScheduler,
):
    counter = OOInteger()
    requirement = commands2.Subsystem()
    other = commands2.InstantCommand(lambda: None, requirement).with_interrupt_behavior(
        interruption_behavior
    )

    default_command = commands2.Command()
    default_command.add_requirements(requirement)

    @patch_via_decorator(default_command)
    def initialize(self):
        counter.increment_and_get()
        scheduler.schedule(other)

    scheduler.set_default_command(requirement, default_command)

    scheduler.run()
    scheduler.run()
    scheduler.run()

    assert counter == 3
    assert not scheduler.is_scheduled(default_command)
    assert scheduler.is_scheduled(other)
