import pytest

from wpilib import Watchdog
from wpilib.simulation import pauseTiming, resumeTiming, stepTiming


@pytest.fixture(autouse=True)
def watchdog_setup():
    pauseTiming()
    yield
    resumeTiming()


def test_enable_disable():
    counter = [0]
    watchdog = Watchdog(0.4, lambda: counter.__setitem__(0, counter[0] + 1))

    # Run 1: disable before timeout
    watchdog.enable()
    stepTiming(0.2)
    watchdog.disable()
    assert counter[0] == 0

    # Run 2: step past timeout
    counter[0] = 0
    watchdog.enable()
    stepTiming(0.4)
    watchdog.disable()
    assert counter[0] == 1

    # Run 3: step well past timeout, only triggers once
    counter[0] = 0
    watchdog.enable()
    stepTiming(1.0)
    watchdog.disable()
    assert counter[0] == 1


def test_reset():
    counter = [0]
    watchdog = Watchdog(0.4, lambda: counter.__setitem__(0, counter[0] + 1))

    watchdog.enable()
    stepTiming(0.2)
    watchdog.reset()
    stepTiming(0.2)
    watchdog.disable()

    assert counter[0] == 0


def test_set_timeout():
    counter = [0]
    watchdog = Watchdog(1.0, lambda: counter.__setitem__(0, counter[0] + 1))

    watchdog.enable()
    stepTiming(0.2)
    watchdog.setTimeout(0.2)

    assert watchdog.getTimeout() == pytest.approx(0.2)
    assert counter[0] == 0

    stepTiming(0.3)
    watchdog.disable()

    assert counter[0] == 1


def test_is_expired():
    watchdog = Watchdog(0.2, lambda: None)
    assert not watchdog.isExpired()
    watchdog.enable()

    assert not watchdog.isExpired()
    stepTiming(0.3)
    assert watchdog.isExpired()

    watchdog.disable()
    assert watchdog.isExpired()

    watchdog.reset()
    assert not watchdog.isExpired()


def test_epochs():
    counter = [0]
    watchdog = Watchdog(0.4, lambda: counter.__setitem__(0, counter[0] + 1))

    # Run 1: under timeout with epochs
    watchdog.enable()
    watchdog.addEpoch("Epoch 1")
    stepTiming(0.1)
    watchdog.addEpoch("Epoch 2")
    stepTiming(0.1)
    watchdog.addEpoch("Epoch 3")
    watchdog.disable()
    assert counter[0] == 0

    # Run 2: reset mid-run keeps under timeout
    watchdog.enable()
    watchdog.addEpoch("Epoch 1")
    stepTiming(0.2)
    watchdog.reset()
    stepTiming(0.2)
    watchdog.addEpoch("Epoch 2")
    watchdog.disable()
    assert counter[0] == 0


def test_multi_watchdog():
    counter1 = [0]
    counter2 = [0]
    watchdog1 = Watchdog(0.2, lambda: counter1.__setitem__(0, counter1[0] + 1))
    watchdog2 = Watchdog(0.6, lambda: counter2.__setitem__(0, counter2[0] + 1))

    watchdog2.enable()
    stepTiming(0.25)
    assert counter1[0] == 0
    assert counter2[0] == 0

    # watchdog1 enabled later but has shorter timeout — expires first
    watchdog1.enable()
    stepTiming(0.25)
    watchdog1.disable()
    watchdog2.disable()

    assert counter1[0] == 1
    assert counter2[0] == 0
