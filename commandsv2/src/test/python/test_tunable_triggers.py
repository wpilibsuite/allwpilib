import commands2
import pytest
import tunables
import wpilib
from util import OOBoolean, OOInteger


@pytest.fixture
def backend():
    tunables.TunableRegistry.reset()
    backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("", backend)
    try:
        yield backend
    finally:
        tunables.TunableRegistry.reset()


def tune(backend: tunables.MockTunableBackend, path: str, value: float) -> None:
    backend.set_double(path, value)
    tunables.TunableRegistry.update()


def test_initial_false_and_first_poll_detection(backend):
    loop = wpilib.EventLoop()
    gain = tunables.add_double("gain", 1.0)

    tune(backend, "/gain", 2.0)
    trigger = commands2.button.TunableTriggers.tuned(loop, gain)

    assert not trigger.get_as_boolean()

    tune(backend, "/gain", 3.0)
    assert not trigger.get_as_boolean()

    loop.poll()
    assert trigger.get_as_boolean()

    loop.poll()
    assert not trigger.get_as_boolean()


def test_consecutive_and_coalesced_tunes(backend):
    loop = wpilib.EventLoop()
    gain = tunables.add_double("gain", 1.0)
    trigger = commands2.button.TunableTriggers.tuned(loop, gain)

    loop.poll()
    assert not trigger.get_as_boolean()

    backend.set_double("/gain", 2.0)
    backend.set_double("/gain", 3.0)
    tunables.TunableRegistry.update()
    loop.poll()
    assert trigger.get_as_boolean()

    tune(backend, "/gain", 4.0)
    loop.poll()
    assert trigger.get_as_boolean()

    loop.poll()
    assert not trigger.get_as_boolean()


def test_local_writes_do_not_activate_trigger(backend):
    loop = wpilib.EventLoop()
    gain = tunables.add_double("gain", 1.0)
    trigger = commands2.button.TunableTriggers.tuned(loop, gain)

    gain.set(2.0)
    loop.poll()

    assert not trigger.get_as_boolean()


def test_multiple_bindings_repeated_reads_and_independent_observers(backend):
    loop = wpilib.EventLoop()
    gain = tunables.add_double("gain", 1.0)
    first = commands2.button.TunableTriggers.tuned(loop, gain)
    second = commands2.button.TunableTriggers.tuned(loop, gain)
    first_count = OOInteger(0)
    second_count = OOInteger(0)

    first.if_true(commands2.cmd.start_end(first_count.increment_and_get, lambda: None))
    first.if_true(commands2.cmd.start_end(second_count.increment_and_get, lambda: None))

    tune(backend, "/gain", 2.0)
    loop.poll()

    assert first.get_as_boolean()
    assert first.get_as_boolean()
    assert second.get_as_boolean()
    assert first_count == 1
    assert second_count == 1

    loop.poll()

    assert not first.get_as_boolean()
    assert not second.get_as_boolean()


def test_different_loop_rates_and_false_gate(backend):
    fast_loop = wpilib.EventLoop()
    slow_loop = wpilib.EventLoop()
    gain = tunables.add_double("gain", 1.0)
    fast = commands2.button.TunableTriggers.tuned(fast_loop, gain)
    slow = commands2.button.TunableTriggers.tuned(slow_loop, gain)
    gate = OOBoolean(False)
    gated_fast = fast.and_(gate)

    tune(backend, "/gain", 2.0)

    fast_loop.poll()
    assert fast.get_as_boolean()
    assert not gated_fast.get_as_boolean()
    assert not slow.get_as_boolean()

    gate.set(True)
    fast_loop.poll()
    assert not fast.get_as_boolean()
    assert not gated_fast.get_as_boolean()

    slow_loop.poll()
    assert slow.get_as_boolean()

    slow_loop.poll()
    assert not slow.get_as_boolean()


def test_or_composition(backend):
    loop = wpilib.EventLoop()
    kP = tunables.add_double("kP", 1.0)
    kI = tunables.add_double("kI", 0.0)
    kD = tunables.add_double("kD", 0.0)
    trigger = (
        commands2.button.TunableTriggers.tuned(loop, kP)
        .or_(commands2.button.TunableTriggers.tuned(loop, kI))
        .or_(commands2.button.TunableTriggers.tuned(loop, kD))
    )

    loop.poll()
    assert not trigger.get_as_boolean()

    tune(backend, "/kI", 1.0)
    loop.poll()
    assert trigger.get_as_boolean()

    loop.poll()
    assert not trigger.get_as_boolean()

    backend.set_double("/kP", 2.0)
    backend.set_double("/kD", 3.0)
    tunables.TunableRegistry.update()
    loop.poll()
    assert trigger.get_as_boolean()


def test_default_loop_and_explicit_loop_isolation(
    scheduler: commands2.CommandScheduler, backend
):
    explicit_loop = wpilib.EventLoop()
    gain = tunables.add_double("gain", 1.0)
    default_trigger = commands2.button.TunableTriggers.tuned(gain)
    explicit_trigger = commands2.button.TunableTriggers.tuned(explicit_loop, gain)

    tune(backend, "/gain", 2.0)

    explicit_loop.poll()
    assert explicit_trigger.get_as_boolean()
    assert not default_trigger.get_as_boolean()

    scheduler.get_default_button_loop().poll()
    assert default_trigger.get_as_boolean()


def test_if_true_runs_on_consecutive_tune_polls(
    scheduler: commands2.CommandScheduler, backend
):
    gain = tunables.add_double("gain", 1.0)
    runs = OOInteger(0)

    commands2.button.TunableTriggers.tuned(gain).if_true(
        commands2.cmd.run_once(runs.increment_and_get)
    )

    scheduler.run()
    assert runs == 0

    tune(backend, "/gain", 2.0)
    scheduler.run()
    assert runs == 1

    tune(backend, "/gain", 3.0)
    scheduler.run()
    assert runs == 2

    scheduler.run()
    assert runs == 2
