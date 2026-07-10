import threading

import pytest

from wpilib import TimesliceRobot
from wpilib.simulation import (
    DriverStationSim,
    pause_timing,
    resume_timing,
    set_program_started,
    step_timing,
    wait_for_program_start,
)


@pytest.fixture(autouse=True)
def timeslice_setup(wpilib_state):
    pause_timing()
    set_program_started(False)
    yield
    resume_timing()


class MockRobot(TimesliceRobot):
    def __init__(self):
        super().__init__(0.002, 0.005)
        self.robot_periodic_count = 0

    def robot_periodic(self):
        self.robot_periodic_count += 1


def test_schedule():
    robot = MockRobot()

    callback_count1 = 0
    callback_count2 = 0

    def callback1() -> None:
        nonlocal callback_count1
        callback_count1 += 1

    def callback2() -> None:
        nonlocal callback_count2
        callback_count2 += 1

    robot.schedule(callback1, 0.0005)
    robot.schedule(callback2, 0.001)

    robot_thread = threading.Thread(target=robot.start_competition, daemon=True)
    robot_thread.start()
    wait_for_program_start()

    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()

    # First 5 ms: no callbacks fired yet (delayed by one period)
    step_timing(0.005)
    assert robot.robot_periodic_count == 0
    assert callback_count1 == 0
    assert callback_count2 == 0

    # Step to 1.5 ms into next period — nothing yet
    step_timing(0.0015)
    assert robot.robot_periodic_count == 0
    assert callback_count1 == 0
    assert callback_count2 == 0

    # Step to 2.25 ms — callback1 fires (offset 2 ms, period 0.5 ms)
    step_timing(0.00075)
    assert robot.robot_periodic_count == 0
    assert callback_count1 == 1
    assert callback_count2 == 0

    # Step to 2.75 ms — callback2 fires (offset 2.5 ms, period 1 ms)
    step_timing(0.0005)
    assert robot.robot_periodic_count == 0
    assert callback_count1 == 1
    assert callback_count2 == 1

    robot.end_competition()
    robot_thread.join()


def test_schedule_overrun():
    robot = MockRobot()

    robot.schedule(lambda: None, 0.0005)
    robot.schedule(lambda: None, 0.001)

    # offset = 2 ms + 0.5 ms + 1 ms = 3.5 ms; 3.5 ms + 3 ms = 6.5 ms > 5 ms max
    with pytest.raises(RuntimeError):
        robot.schedule(lambda: None, 0.003)

    robot.end_competition()
