# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import pytest

from wpilib import RobotController, Timer
from wpilib.simulation import (
    is_timing_paused,
    pause_timing,
    restart_timing,
    resume_timing,
    step_timing,
)


@pytest.fixture(autouse=True)
def timing_setup():
    RobotController.set_time_source(RobotController.get_monotonic_time)
    pause_timing()
    restart_timing()
    yield
    RobotController.set_time_source(RobotController.get_monotonic_time)
    resume_timing()


def test_start_stop():
    timer = Timer()

    # Verify timer is initialized as stopped
    assert timer.get() == 0.0
    assert not timer.is_running()
    step_timing(0.5)
    assert timer.get() == 0.0
    assert not timer.is_running()

    # Verify timer increments after it's started
    timer.start()
    step_timing(0.5)
    assert timer.get() == pytest.approx(0.5)
    assert timer.is_running()

    # Verify timer stops incrementing after it's stopped
    timer.stop()
    step_timing(0.5)
    assert timer.get() == pytest.approx(0.5)
    assert not timer.is_running()


def test_reset():
    timer = Timer()
    timer.start()

    # Advance timer to 500 ms
    assert timer.get() == 0.0
    step_timing(0.5)
    assert timer.get() == pytest.approx(0.5)

    # Verify timer reports 0 ms after reset
    timer.reset()
    assert timer.get() == 0.0

    # Verify timer continues incrementing
    step_timing(0.5)
    assert timer.get() == pytest.approx(0.5)

    # Verify timer doesn't start incrementing after reset if it was stopped
    timer.stop()
    timer.reset()
    step_timing(0.5)
    assert timer.get() == 0.0


def test_reset_with_large_timestamp():
    mock_time = 1_000_000_002
    RobotController.set_time_source(lambda: mock_time)

    timer = Timer()
    timer.start()

    mock_time += 500_000_000
    assert timer.get() == pytest.approx(0.5)

    timer.reset()
    assert timer.get() == 0.0

    mock_time += 500_000_000
    assert timer.get() == pytest.approx(0.5)


def test_has_elapsed():
    timer = Timer()

    # Verify 0 ms has elapsed since timer hasn't started
    assert timer.has_elapsed(0.0)

    # Verify timer doesn't report elapsed time when stopped
    step_timing(0.5)
    assert not timer.has_elapsed(0.4)

    timer.start()

    # Verify timer reports >= 400 ms has elapsed after multiple calls
    step_timing(0.5)
    assert timer.has_elapsed(0.4)
    assert timer.has_elapsed(0.4)


def test_advance_if_elapsed():
    timer = Timer()

    # Verify 0 ms has elapsed since timer hasn't started
    assert timer.advance_if_elapsed(0.0)

    # Verify timer doesn't report elapsed time when stopped
    step_timing(0.5)
    assert not timer.advance_if_elapsed(0.4)

    timer.start()

    # Verify timer reports >= 400 ms has elapsed for only first call
    step_timing(0.5)
    assert timer.advance_if_elapsed(0.4)
    assert not timer.advance_if_elapsed(0.4)

    # Verify timer reports >= 400 ms has elapsed for two calls
    step_timing(1.0)
    assert timer.advance_if_elapsed(0.4)
    assert timer.advance_if_elapsed(0.4)
    assert not timer.advance_if_elapsed(0.4)


def test_advance_if_elapsed_preserves_fractional_period():
    mock_time = {"value": 0}
    RobotController.set_time_source(lambda: mock_time["value"])

    timer = Timer()
    timer.start()

    period = 1.0 / 60.0

    for i in range(1, 61):
        mock_time["value"] = (i * 1_000_000_000 + 59) // 60 + 100

        assert timer.advance_if_elapsed(period)
        assert not timer.advance_if_elapsed(period)

    assert timer.get() == pytest.approx(100e-9, abs=1e-12)


def test_advance_if_elapsed_progresses_with_sub_nanosecond_period():
    mock_time = {"value": 0}
    RobotController.set_time_source(lambda: mock_time["value"])

    timer = Timer()
    timer.start()

    mock_time["value"] = 1
    period = 0.1e-9

    for _ in range(10):
        assert timer.advance_if_elapsed(period)

    assert not timer.advance_if_elapsed(period)
    assert timer.get() == pytest.approx(0.0, abs=1e-12)


def test_get_monotonic_timestamp():
    start = Timer.get_monotonic_timestamp()
    step_timing(0.5)
    end = Timer.get_monotonic_timestamp()
    assert end == pytest.approx(start + 0.5)


def test_restart_timing_preserves_paused_clock():
    assert is_timing_paused()

    step_timing(0.5)
    before_restart = Timer.get_monotonic_timestamp()

    restart_timing()

    assert is_timing_paused()
    assert Timer.get_monotonic_timestamp() == pytest.approx(before_restart)

    step_timing(0.5)
    assert Timer.get_monotonic_timestamp() == pytest.approx(before_restart + 0.5)
