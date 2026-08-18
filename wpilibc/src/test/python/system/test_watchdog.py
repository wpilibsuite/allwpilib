import pytest

from wpilib import Watchdog
from wpilib.simulation import pause_timing, resume_timing, step_timing


@pytest.fixture(autouse=True)
def watchdog_setup():
    pause_timing()
    yield
    resume_timing()


def test_enable_disable():
    counter = 0

    def on_expire() -> None:
        nonlocal counter
        counter += 1

    watchdog = Watchdog(0.4, on_expire)

    # Run 1: disable before timeout
    watchdog.enable()
    step_timing(0.2)
    watchdog.disable()
    assert counter == 0

    # Run 2: step past timeout
    counter = 0
    watchdog.enable()
    step_timing(0.4)
    watchdog.disable()
    assert counter == 1

    # Run 3: step well past timeout, only triggers once
    counter = 0
    watchdog.enable()
    step_timing(1.0)
    watchdog.disable()
    assert counter == 1


def test_reset():
    counter = 0

    def on_expire() -> None:
        nonlocal counter
        counter += 1

    watchdog = Watchdog(0.4, on_expire)

    watchdog.enable()
    step_timing(0.2)
    watchdog.reset()
    step_timing(0.2)
    watchdog.disable()

    assert counter == 0


def test_set_timeout():
    counter = 0

    def on_expire() -> None:
        nonlocal counter
        counter += 1

    watchdog = Watchdog(1.0, on_expire)

    watchdog.enable()
    step_timing(0.2)
    watchdog.set_timeout(0.2)

    assert watchdog.get_timeout() == pytest.approx(0.2)
    assert counter == 0

    step_timing(0.3)
    watchdog.disable()

    assert counter == 1


def test_is_expired():
    watchdog = Watchdog(0.2, lambda: None)
    assert not watchdog.is_expired()
    watchdog.enable()

    assert not watchdog.is_expired()
    step_timing(0.3)
    assert watchdog.is_expired()

    watchdog.disable()
    assert watchdog.is_expired()

    watchdog.reset()
    assert not watchdog.is_expired()


def test_epochs():
    counter = 0

    def on_expire() -> None:
        nonlocal counter
        counter += 1

    watchdog = Watchdog(0.4, on_expire)

    # Run 1: under timeout with epochs
    watchdog.enable()
    watchdog.add_epoch("Epoch 1")
    step_timing(0.1)
    watchdog.add_epoch("Epoch 2")
    step_timing(0.1)
    watchdog.add_epoch("Epoch 3")
    watchdog.disable()
    assert counter == 0

    # Run 2: reset mid-run keeps under timeout
    watchdog.enable()
    watchdog.add_epoch("Epoch 1")
    step_timing(0.2)
    watchdog.reset()
    step_timing(0.2)
    watchdog.add_epoch("Epoch 2")
    watchdog.disable()
    assert counter == 0


def test_multi_watchdog():
    counter1 = 0
    counter2 = 0

    def on_expire1() -> None:
        nonlocal counter1
        counter1 += 1

    def on_expire2() -> None:
        nonlocal counter2
        counter2 += 1

    watchdog1 = Watchdog(0.2, on_expire1)
    watchdog2 = Watchdog(0.6, on_expire2)

    watchdog2.enable()
    step_timing(0.25)
    assert counter1 == 0
    assert counter2 == 0

    # watchdog1 enabled later but has shorter timeout — expires first
    watchdog1.enable()
    step_timing(0.25)
    watchdog1.disable()
    watchdog2.disable()

    assert counter1 == 1
    assert counter2 == 0
