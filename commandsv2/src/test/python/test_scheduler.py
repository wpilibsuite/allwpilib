from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest
import telemetry
import telemetry.mock_backend as mock_backend
import tunables
import wpilib


def test_scheduler_lambda_test_no_interrupt(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    scheduler.on_command_initialize(lambda _: counter.increment_and_get())
    scheduler.on_command_execute(lambda _: counter.increment_and_get())
    scheduler.on_command_finish(lambda _: counter.increment_and_get())

    scheduler.schedule(commands2.InstantCommand())
    scheduler.run()

    assert counter == 3


def test_scheduler_interrupt_lambda(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    scheduler.on_command_interrupt(lambda _: counter.increment_and_get())

    command = commands2.WaitCommand(10)

    scheduler.schedule(command)
    scheduler.cancel(command)

    assert counter == 1


def test_scheduler_interrupt_no_cause_lambda(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    def on_interrupt(interrupted, cause):
        assert cause is None
        counter.increment_and_get()

    scheduler.on_command_interrupt_with_cause(on_interrupt)

    command = commands2.cmd.run(lambda: {})

    scheduler.schedule(command)
    scheduler.cancel(command)

    assert counter.get() == 1


def test_scheduler_interrupt_cause_lambda(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    subsystem = commands2.Subsystem()
    command = subsystem.run(lambda: None)
    interruptor = subsystem.run_once(lambda: None)

    def on_interrupt(interrupted, cause):
        assert cause is interruptor
        counter.increment_and_get()

    scheduler.on_command_interrupt_with_cause(on_interrupt)

    scheduler.schedule(command)
    scheduler.schedule(interruptor)

    assert counter.get() == 1


def test_scheduler_interrupt_cause_lambda_in_run_loop(
    scheduler: commands2.CommandScheduler,
):
    counter = OOInteger()

    subsystem = commands2.Subsystem()
    command = subsystem.run(lambda: None)
    interruptor = subsystem.run_once(lambda: None)
    # This command will schedule interruptor in execute() inside the run loop
    interruptor_scheduler = commands2.cmd.run_once(
        lambda: scheduler.schedule(interruptor)
    )

    def on_interrupt(interrupted, cause):
        assert cause is interruptor
        counter.increment_and_get()

    scheduler.on_command_interrupt_with_cause(on_interrupt)

    scheduler.schedule(command)
    scheduler.schedule(interruptor_scheduler)

    scheduler.run()

    assert counter.get() == 1


def test_unregister_subsystem(scheduler: commands2.CommandScheduler):
    system = commands2.Subsystem()
    scheduler.register_subsystem(system)
    scheduler.unregister_subsystem(system)


def test_scheduler_cancel_all(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    def on_interrupt(command, interruptor):
        assert interruptor is None

    scheduler.on_command_interrupt(lambda _: counter.increment_and_get())
    scheduler.on_command_interrupt_with_cause(on_interrupt)

    command = commands2.WaitCommand(10)
    command2 = commands2.WaitCommand(10)

    scheduler.schedule(command)
    scheduler.schedule(command2)
    scheduler.cancel_all()

    assert counter == 2


def test_schedule_scheduled_no_op(scheduler: commands2.CommandScheduler):
    counter = OOInteger()

    command = commands2.cmd.start_end(counter.increment_and_get, lambda: None)

    scheduler.schedule(command)
    scheduler.schedule(command)

    assert counter == 1


def test_scheduler_logs_names_and_ids(scheduler: commands2.CommandScheduler):
    backend = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.reset()
    telemetry.TelemetryRegistry.register_backend("", backend)
    command = commands2.WaitCommand(10).with_name("WaitForIt")

    try:
        scheduler.schedule(command)
        scheduler.log_to(telemetry.get_table())

        assert backend.get_last_action("/Names") == mock_backend.Action(
            "/Names", ["WaitForIt"]
        )
        assert backend.get_last_value("/Names") == ["WaitForIt"]
        assert backend.get_last_action("/Ids") == mock_backend.Action(
            "/Ids", [id(command)]
        )
        assert backend.get_last_value("/Ids") == [id(command)]
    finally:
        telemetry.TelemetryRegistry.reset()


def test_scheduler_tunable_publishes_names_ids_and_cancel(
    scheduler: commands2.CommandScheduler,
):
    backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.reset()
    tunables.TunableRegistry.register_backend("", backend)
    command = commands2.WaitCommand(10).with_name("WaitForIt")

    try:
        scheduler.schedule(command)
        tunables.publish("scheduler", scheduler)

        assert backend.get_value("/scheduler/Names") == ["WaitForIt"]
        assert backend.get_value("/scheduler/Ids") == [id(command)]
        assert backend.get_value("/scheduler/Cancel") == []

        scheduler.cancel(command)
        tunables.TunableRegistry.update()

        assert backend.get_value("/scheduler/Names") == []
        assert backend.get_value("/scheduler/Ids") == []

        scheduler.schedule(command)
        tunables.TunableRegistry.update()

        assert backend.get_value("/scheduler/Names") == ["WaitForIt"]
        assert backend.get_value("/scheduler/Ids") == [id(command)]

        backend.set_int64_vector("/scheduler/Cancel", [id(command)])
        tunables.TunableRegistry.update()

        assert not scheduler.is_scheduled(command)
    finally:
        tunables.TunableRegistry.reset()


def test_reset_instance_removes_scheduler_tunable_publications(
    scheduler: commands2.CommandScheduler,
):
    backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.reset()
    tunables.TunableRegistry.register_backend("", backend)

    try:
        command = commands2.WaitCommand(10).with_name("OldCommand")
        scheduler.schedule(command)
        tunables.publish("scheduler", scheduler)

        commands2.CommandScheduler.reset_instance()
        replacement = commands2.CommandScheduler.get_instance()
        tunables.publish("scheduler", replacement)
        tunables.TunableRegistry.update()

        assert backend.get_value("/scheduler/Names") == []
        assert backend.get_value("/scheduler/Ids") == []
        assert backend.get_value("/scheduler/Cancel") == []
    finally:
        tunables.TunableRegistry.reset()
        commands2.CommandScheduler.reset_instance()
