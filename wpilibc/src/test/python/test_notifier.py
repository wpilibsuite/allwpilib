# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import threading

import pytest

from wpilib import Notifier
from wpilib.simulation import pauseTiming, restartTiming, resumeTiming, stepTiming

# These tests fail because of race conditions in simulation
if False:

    class AtomicInteger:
        def __init__(self) -> None:
            self.lock = threading.Lock()
            self.val = 0

        def get(self) -> int:
            with self.lock:
                return self.val

        def getAndIncrement(self) -> int:
            with self.lock:
                val = self.val
                self.val += 1
                return val

    @pytest.fixture
    def counter():
        return AtomicInteger()

    @pytest.fixture
    def notifier(counter):
        pauseTiming()
        restartTiming()
        n = Notifier(counter.getAndIncrement)
        yield n
        del n
        resumeTiming()

    @pytest.mark.xfail(strict=False)
    def test_testStartPeriodicAndStop(counter: AtomicInteger, notifier: Notifier):
        notifier.startPeriodic(1.0)

        stepTiming(10)

        notifier.stop()
        assert counter.get() == 10

        stepTiming(3)

        assert counter.get() == 10

    @pytest.mark.xfail(strict=False)
    def test_testStartSingle(counter, notifier):
        notifier.startSingle(1.0)

        stepTiming(10)

        assert counter.get() == 1
