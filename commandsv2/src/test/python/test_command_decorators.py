from typing import TYPE_CHECKING

from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import commands2
import pytest


def test_timeout(scheduler: commands2.CommandScheduler):
    with ManualSimTime() as sim:
        command1 = commands2.WaitCommand(1)
        timeout = command1.with_timeout(2)
        scheduler.schedule(timeout)
        scheduler.run()
        assert not command1.is_scheduled()
        assert timeout.is_scheduled()
        sim.step(3)
        scheduler.run()
        assert not timeout.is_scheduled()


def test_until(scheduler: commands2.CommandScheduler):
    condition = OOBoolean(False)
    command = commands2.WaitCommand(10).until(condition)
    scheduler.schedule(command)
    scheduler.run()
    assert command.is_scheduled()
    condition.set(True)
    scheduler.run()
    assert not command.is_scheduled()


def test_only_while(scheduler: commands2.CommandScheduler):
    condition = OOBoolean(True)
    command = commands2.WaitCommand(10).only_while(condition)
    scheduler.schedule(command)
    scheduler.run()
    assert command.is_scheduled()
    condition.set(False)
    scheduler.run()
    assert not command.is_scheduled()


def test_ignoring_disable(scheduler: commands2.CommandScheduler):
    command = commands2.RunCommand(lambda: None).ignoring_disable(True)
    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()
    scheduler.schedule(command)
    scheduler.run()
    assert command.is_scheduled()


def test_before_starting(scheduler: commands2.CommandScheduler):
    condition = OOBoolean(False)
    condition.set(False)
    command = commands2.InstantCommand()
    scheduler.schedule(
        command.before_starting(commands2.InstantCommand(lambda: condition.set(True)))
    )
    assert condition == True


@pytest.mark.skip
def test_and_then_lambda(scheduler: commands2.CommandScheduler): ...


def test_and_then(scheduler: commands2.CommandScheduler):
    condition = OOBoolean(False)
    condition.set(False)
    command1 = commands2.InstantCommand()
    command2 = commands2.InstantCommand(lambda: condition.set(True))
    scheduler.schedule(command1.and_then(command2))
    assert condition == False
    scheduler.run()
    assert condition == True


def test_deadline_for(scheduler: commands2.CommandScheduler):
    condition = OOBoolean(False)
    condition.set(False)

    dictator = commands2.WaitUntilCommand(condition)
    ends_before = commands2.InstantCommand()
    ends_after = commands2.WaitUntilCommand(lambda: False)

    group = dictator.deadline_for(ends_before, ends_after)

    scheduler.schedule(group)
    scheduler.run()
    assert group.is_scheduled()
    condition.set(True)
    scheduler.run()
    assert not group.is_scheduled()


def test_along_with(scheduler: commands2.CommandScheduler):
    condition = OOBoolean()
    condition.set(False)

    command1 = commands2.WaitUntilCommand(condition)
    command2 = commands2.InstantCommand()

    group = command1.along_with(command2)

    scheduler.schedule(group)
    scheduler.run()
    assert group.is_scheduled()
    condition.set(True)
    scheduler.run()
    assert not group.is_scheduled()


def test_race_with(scheduler: commands2.CommandScheduler):
    command1 = commands2.WaitUntilCommand(lambda: False)
    command2 = commands2.InstantCommand()

    group = command1.race_with(command2)

    scheduler.schedule(group)
    scheduler.run()
    assert not group.is_scheduled()


def test_unless(scheduler: commands2.CommandScheduler):
    unless_condition = OOBoolean(True)
    has_run_condition = OOBoolean(False)

    command = commands2.InstantCommand(lambda: has_run_condition.set(True)).unless(
        unless_condition
    )

    scheduler.schedule(command)
    scheduler.run()
    assert has_run_condition == False
    unless_condition.set(False)
    scheduler.schedule(command)
    scheduler.run()
    assert has_run_condition == True


def test_only_if(scheduler: commands2.CommandScheduler):
    only_if_condition = OOBoolean(False)
    has_run_condition = OOBoolean(False)

    command = commands2.InstantCommand(lambda: has_run_condition.set(True)).only_if(
        only_if_condition
    )

    scheduler.schedule(command)
    scheduler.run()
    assert has_run_condition == False
    only_if_condition.set(True)
    scheduler.schedule(command)
    scheduler.run()
    assert has_run_condition == True


def test_finally_do(scheduler: commands2.CommandScheduler):
    first = OOInteger(0)
    second = OOInteger(0)

    command = commands2.FunctionalCommand(
        lambda: None,
        lambda: None,
        lambda interrupted: first.increment_and_get() if not interrupted else None,
        lambda: True,
    ).finally_do(lambda interrupted: second.add_and_get(1 + first()))

    scheduler.schedule(command)
    assert first == 0
    assert second == 0
    scheduler.run()
    assert first == 1
    assert second == 2


def test_handle_interrupt(scheduler: commands2.CommandScheduler):
    first = OOInteger(0)
    second = OOInteger(0)

    command = commands2.FunctionalCommand(
        lambda: None,
        lambda: None,
        lambda interrupted: first.increment_and_get() if interrupted else None,
        lambda: False,
    ).handle_interrupt(lambda: second.add_and_get(1 + first()))

    scheduler.schedule(command)
    scheduler.run()
    assert first == 0
    assert second == 0
    scheduler.cancel(command)
    assert first == 1
    assert second == 2


def test_with_name(scheduler: commands2.CommandScheduler):
    command = commands2.InstantCommand()
    name = "Named"
    named = command.with_name(name)
    assert named.get_name() == name
