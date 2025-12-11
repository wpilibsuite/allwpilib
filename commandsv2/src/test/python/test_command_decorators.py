from typing import TYPE_CHECKING

from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import commands2
import pytest


def test_timeout(scheduler: commands2.CommandScheduler):
    with ManualSimTime() as sim:
        command1 = commands2.WaitCommand(1)
        timeout = command1.withTimeout(2)
        scheduler.schedule(timeout)
        scheduler.run()
        assert not command1.isScheduled()
        assert timeout.isScheduled()
        sim.step(3)
        scheduler.run()
        assert not timeout.isScheduled()


def test_until(scheduler: commands2.CommandScheduler):
    condition = OOBoolean(False)
    command = commands2.WaitCommand(10).until(condition)
    scheduler.schedule(command)
    scheduler.run()
    assert command.isScheduled()
    condition.set(True)
    scheduler.run()
    assert not command.isScheduled()


def test_only_while(scheduler: commands2.CommandScheduler):
    condition = OOBoolean(True)
    command = commands2.WaitCommand(10).onlyWhile(condition)
    scheduler.schedule(command)
    scheduler.run()
    assert command.isScheduled()
    condition.set(False)
    scheduler.run()
    assert not command.isScheduled()


def test_ignoringDisable(scheduler: commands2.CommandScheduler):
    command = commands2.RunCommand(lambda: None).ignoringDisable(True)
    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()
    scheduler.schedule(command)
    scheduler.run()
    assert command.isScheduled()


def test_beforeStarting(scheduler: commands2.CommandScheduler):
    condition = OOBoolean(False)
    condition.set(False)
    command = commands2.InstantCommand()
    scheduler.schedule(
        command.beforeStarting(commands2.InstantCommand(lambda: condition.set(True)))
    )
    assert condition == True


@pytest.mark.skip
def test_andThenLambda(scheduler: commands2.CommandScheduler): ...


def test_andThen(scheduler: commands2.CommandScheduler):
    condition = OOBoolean(False)
    condition.set(False)
    command1 = commands2.InstantCommand()
    command2 = commands2.InstantCommand(lambda: condition.set(True))
    scheduler.schedule(command1.andThen(command2))
    assert condition == False
    scheduler.run()
    assert condition == True


def test_deadlineWith(scheduler: commands2.CommandScheduler):
    condition = OOBoolean(False)
    condition.set(False)

    dictator = commands2.WaitUntilCommand(condition)
    endsBefore = commands2.InstantCommand()
    endsAfter = commands2.WaitUntilCommand(lambda: False)

    group = dictator.deadlineWith(endsBefore, endsAfter)

    scheduler.schedule(group)
    scheduler.run()
    assert group.isScheduled()
    condition.set(True)
    scheduler.run()
    assert not group.isScheduled()


def test_deadlineFor(scheduler: commands2.CommandScheduler):
    condition = OOBoolean(False)
    condition.set(False)

    dictator = commands2.WaitUntilCommand(condition)
    endsBefore = commands2.InstantCommand()
    endsAfter = commands2.WaitUntilCommand(lambda: False)

    group = dictator.deadlineFor(endsBefore, endsAfter)

    scheduler.schedule(group)
    scheduler.run()
    assert group.isScheduled()
    condition.set(True)
    scheduler.run()
    assert not group.isScheduled()


def test_alongWith(scheduler: commands2.CommandScheduler):
    condition = OOBoolean()
    condition.set(False)

    command1 = commands2.WaitUntilCommand(condition)
    command2 = commands2.InstantCommand()

    group = command1.alongWith(command2)

    scheduler.schedule(group)
    scheduler.run()
    assert group.isScheduled()
    condition.set(True)
    scheduler.run()
    assert not group.isScheduled()


def test_raceWith(scheduler: commands2.CommandScheduler):
    command1 = commands2.WaitUntilCommand(lambda: False)
    command2 = commands2.InstantCommand()

    group = command1.raceWith(command2)

    scheduler.schedule(group)
    scheduler.run()
    assert not group.isScheduled()


def test_unless(scheduler: commands2.CommandScheduler):
    unlessCondition = OOBoolean(True)
    hasRunCondition = OOBoolean(False)

    command = commands2.InstantCommand(lambda: hasRunCondition.set(True)).unless(
        unlessCondition
    )

    scheduler.schedule(command)
    scheduler.run()
    assert hasRunCondition == False
    unlessCondition.set(False)
    scheduler.schedule(command)
    scheduler.run()
    assert hasRunCondition == True


def test_onlyIf(scheduler: commands2.CommandScheduler):
    onlyIfCondition = OOBoolean(False)
    hasRunCondition = OOBoolean(False)

    command = commands2.InstantCommand(lambda: hasRunCondition.set(True)).onlyIf(
        onlyIfCondition
    )

    scheduler.schedule(command)
    scheduler.run()
    assert hasRunCondition == False
    onlyIfCondition.set(True)
    scheduler.schedule(command)
    scheduler.run()
    assert hasRunCondition == True


def test_finallyDo(scheduler: commands2.CommandScheduler):
    first = OOInteger(0)
    second = OOInteger(0)

    command = commands2.FunctionalCommand(
        lambda: None,
        lambda: None,
        lambda interrupted: first.incrementAndGet() if not interrupted else None,
        lambda: True,
    ).finallyDo(lambda interrupted: second.addAndGet(1 + first()))

    scheduler.schedule(command)
    assert first == 0
    assert second == 0
    scheduler.run()
    assert first == 1
    assert second == 2


def test_handleInterrupt(scheduler: commands2.CommandScheduler):
    first = OOInteger(0)
    second = OOInteger(0)

    command = commands2.FunctionalCommand(
        lambda: None,
        lambda: None,
        lambda interrupted: first.incrementAndGet() if interrupted else None,
        lambda: False,
    ).handleInterrupt(lambda: second.addAndGet(1 + first()))

    scheduler.schedule(command)
    scheduler.run()
    assert first == 0
    assert second == 0
    scheduler.cancel(command)
    assert first == 1
    assert second == 2


def test_withName(scheduler: commands2.CommandScheduler):
    command = commands2.InstantCommand()
    name = "Named"
    named = command.withName(name)
    assert named.getName() == name
