from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest


def test_schedulerLambdaTestNoInterrupt(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    scheduler.onCommandInitialize(lambda _: counter.incrementAndGet())
    scheduler.onCommandExecute(lambda _: counter.incrementAndGet())
    scheduler.onCommandFinish(lambda _: counter.incrementAndGet())

    scheduler.schedule(commands2.InstantCommand())
    scheduler.run()

    assert counter == 3


def test_schedulerInterruptLambda(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    scheduler.onCommandInterrupt(lambda _: counter.incrementAndGet())

    command = commands2.WaitCommand(10)

    scheduler.schedule(command)
    scheduler.cancel(command)

    assert counter == 1


def test_scheduler_interrupt_no_cause_lambda(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    def on_interrupt(interrupted, cause):
        assert cause is None
        counter.incrementAndGet()

    scheduler.onCommandInterruptWithCause(on_interrupt)

    command = commands2.cmd.run(lambda: {})

    scheduler.schedule(command)
    scheduler.cancel(command)

    assert counter.get() == 1


def test_scheduler_interrupt_cause_lambda(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    subsystem = commands2.Subsystem()
    command = subsystem.run(lambda: None)
    interruptor = subsystem.runOnce(lambda: None)

    def on_interrupt(interrupted, cause):
        assert cause is interruptor
        counter.incrementAndGet()

    scheduler.onCommandInterruptWithCause(on_interrupt)

    scheduler.schedule(command)
    scheduler.schedule(interruptor)

    assert counter.get() == 1


def test_scheduler_interrupt_cause_lambda_in_run_loop(
    scheduler: commands2.CommandScheduler,
):
    counter = OOInteger()

    subsystem = commands2.Subsystem()
    command = subsystem.run(lambda: None)
    interruptor = subsystem.runOnce(lambda: None)
    # This command will schedule interruptor in execute() inside the run loop
    interruptor_scheduler = commands2.cmd.runOnce(
        lambda: scheduler.schedule(interruptor)
    )

    def on_interrupt(interrupted, cause):
        assert cause is interruptor
        counter.incrementAndGet()

    scheduler.onCommandInterruptWithCause(on_interrupt)

    scheduler.schedule(command)
    scheduler.schedule(interruptor_scheduler)

    scheduler.run()

    assert counter.get() == 1


def test_unregisterSubsystem(scheduler: commands2.CommandScheduler):
    system = commands2.Subsystem()
    scheduler.registerSubsystem(system)
    scheduler.unregisterSubsystem(system)


def test_schedulerCancelAll(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    def on_interrupt(command, interruptor):
        assert interruptor is None

    scheduler.onCommandInterrupt(lambda _: counter.incrementAndGet())
    scheduler.onCommandInterruptWithCause(on_interrupt)

    command = commands2.WaitCommand(10)
    command2 = commands2.WaitCommand(10)

    scheduler.schedule(command)
    scheduler.schedule(command2)
    scheduler.cancelAll()

    assert counter == 2


def test_scheduleScheduledNoOp(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    command = commands2.cmd.startEnd(counter.incrementAndGet, lambda: None)

    scheduler.schedule(command)
    scheduler.schedule(command)

    assert counter == 1
