import pytest

from wpilib import Timer
from wpilib.simulation import pauseTiming, restartTiming, resumeTiming, stepTiming


@pytest.fixture(autouse=True)
def timer_setup():
    pauseTiming()
    restartTiming()
    yield
    resumeTiming()


def test_start_stop():
    timer = Timer()

    # Stopped timer doesn't advance
    assert timer.get() == pytest.approx(0.0)
    assert not timer.isRunning()
    stepTiming(0.5)
    assert timer.get() == pytest.approx(0.0)
    assert not timer.isRunning()

    # Running timer advances
    timer.start()
    stepTiming(0.5)
    assert timer.get() == pytest.approx(0.5)
    assert timer.isRunning()

    # Stopped timer freezes
    timer.stop()
    stepTiming(0.5)
    assert timer.get() == pytest.approx(0.5)
    assert not timer.isRunning()


def test_reset():
    timer = Timer()
    timer.start()

    assert timer.get() == pytest.approx(0.0)
    stepTiming(0.5)
    assert timer.get() == pytest.approx(0.5)

    timer.reset()
    assert timer.get() == pytest.approx(0.0)

    stepTiming(0.5)
    assert timer.get() == pytest.approx(0.5)

    # Reset while stopped keeps timer stopped
    timer.stop()
    timer.reset()
    stepTiming(0.5)
    assert timer.get() == pytest.approx(0.0)


def test_has_elapsed():
    timer = Timer()

    # 0 s elapsed since timer hasn't started
    assert timer.hasElapsed(0.0)

    # Stopped timer doesn't accumulate elapsed time
    stepTiming(0.5)
    assert not timer.hasElapsed(0.4)

    timer.start()

    stepTiming(0.5)
    assert timer.hasElapsed(0.4)
    assert timer.hasElapsed(0.4)


def test_advance_if_elapsed():
    timer = Timer()

    # 0 s has elapsed
    assert timer.advanceIfElapsed(0.0)

    # Stopped timer doesn't accumulate elapsed time
    stepTiming(0.5)
    assert not timer.advanceIfElapsed(0.4)

    timer.start()

    # First call returns True, second returns False (advanced past threshold)
    stepTiming(0.5)
    assert timer.advanceIfElapsed(0.4)
    assert not timer.advanceIfElapsed(0.4)

    # After 1 more second, two 400 ms periods have elapsed
    stepTiming(1.0)
    assert timer.advanceIfElapsed(0.4)
    assert timer.advanceIfElapsed(0.4)
    assert not timer.advanceIfElapsed(0.4)


def test_get_monotonic_timestamp():
    start = Timer.getMonotonicTimestamp()
    stepTiming(0.5)
    end = Timer.getMonotonicTimestamp()
    assert (end - start) == pytest.approx(0.5)
