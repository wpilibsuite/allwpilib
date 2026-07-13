# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import threading

import pytest

from wpilib import Notifier
from wpilib.simulation import pause_timing, restart_timing, resume_timing, step_timing

# These tests fail because of race conditions in simulation
if False:

    class AtomicInteger:
        def __init__(self) -> None:
            self.lock = threading.Lock()
            self.val = 0

        def get(self) -> int:
            with self.lock:
                return self.val

        def get_and_increment(self) -> int:
            with self.lock:
                val = self.val
                self.val += 1
                return val

    @pytest.fixture
    def counter():
        return AtomicInteger()

    @pytest.fixture
    def notifier(counter):
        pause_timing()
        restart_timing()
        n = Notifier(counter.get_and_increment)
        yield n
        del n
        resume_timing()

    @pytest.mark.xfail(strict=False)
    def test_test_start_periodic_and_stop(counter: AtomicInteger, notifier: Notifier):
        notifier.start_periodic(1.0)

        step_timing(10)

        notifier.stop()
        assert counter.get() == 10

        step_timing(3)

        assert counter.get() == 10

    @pytest.mark.xfail(strict=False)
    def test_test_start_single(counter, notifier):
        notifier.start_single(1.0)

        step_timing(10)

        assert counter.get() == 1
