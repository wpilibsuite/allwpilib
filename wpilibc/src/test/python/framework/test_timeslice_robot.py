import threading

import pytest

from wpilib import TimesliceRobot
from wpilib.simulation import (
    DriverStationSim,
    pauseTiming,
    resumeTiming,
    setProgramStarted,
    stepTiming,
    waitForProgramStart,
)


@pytest.fixture(autouse=True)
def timeslice_setup(wpilib_state):
    pauseTiming()
    setProgramStarted(False)
    yield
    resumeTiming()


class MockRobot(TimesliceRobot):
    def __init__(self):
        super().__init__(0.002, 0.005)
        self.robot_periodic_count = 0

    def robotPeriodic(self):
        self.robot_periodic_count += 1


def test_schedule():
    robot = MockRobot()

    callback_count1 = [0]
    callback_count2 = [0]

    robot.schedule(
        lambda: callback_count1.__setitem__(0, callback_count1[0] + 1), 0.0005
    )
    robot.schedule(
        lambda: callback_count2.__setitem__(0, callback_count2[0] + 1), 0.001
    )

    robot_thread = threading.Thread(target=robot.startCompetition, daemon=True)
    robot_thread.start()
    waitForProgramStart()

    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()

    # First 5 ms: no callbacks fired yet (delayed by one period)
    stepTiming(0.005)
    assert robot.robot_periodic_count == 0
    assert callback_count1[0] == 0
    assert callback_count2[0] == 0

    # Step to 1.5 ms into next period — nothing yet
    stepTiming(0.0015)
    assert robot.robot_periodic_count == 0
    assert callback_count1[0] == 0
    assert callback_count2[0] == 0

    # Step to 2.25 ms — callback1 fires (offset 2 ms, period 0.5 ms)
    stepTiming(0.00075)
    assert robot.robot_periodic_count == 0
    assert callback_count1[0] == 1
    assert callback_count2[0] == 0

    # Step to 2.75 ms — callback2 fires (offset 2.5 ms, period 1 ms)
    stepTiming(0.0005)
    assert robot.robot_periodic_count == 0
    assert callback_count1[0] == 1
    assert callback_count2[0] == 1

    robot.endCompetition()
    robot_thread.join()


def test_schedule_overrun():
    robot = MockRobot()

    robot.schedule(lambda: None, 0.0005)
    robot.schedule(lambda: None, 0.001)

    # offset = 2 ms + 0.5 ms + 1 ms = 3.5 ms; 3.5 ms + 3 ms = 6.5 ms > 5 ms max
    with pytest.raises(RuntimeError):
        robot.schedule(lambda: None, 0.003)

    robot.endCompetition()
