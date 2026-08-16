import threading

import pytest

from wpilib import TimedRobot
from wpilib.simulation import (
    DriverStationSim,
    pause_timing,
    resume_timing,
    set_program_started,
    step_timing,
    wait_for_program_start,
)
from hal import RobotMode

_PERIOD = 0.02  # 20 ms


@pytest.fixture(autouse=True)
def timed_robot_setup(wpilib_state):
    pause_timing()
    set_program_started(False)
    yield
    resume_timing()


class MockRobot(TimedRobot):
    def __init__(self):
        super().__init__(_PERIOD)
        self.simulation_init_count = 0
        self.disabled_enter_count = 0
        self.autonomous_enter_count = 0
        self.teleop_enter_count = 0
        self.utility_enter_count = 0

        self.disabled_exit_count = 0
        self.autonomous_exit_count = 0
        self.teleop_exit_count = 0
        self.utility_exit_count = 0

        self.robot_periodic_count = 0
        self.simulation_periodic_count = 0
        self.disabled_periodic_count = 0
        self.autonomous_periodic_count = 0
        self.teleop_periodic_count = 0
        self.utility_periodic_count = 0

    def simulation_init(self):
        self.simulation_init_count += 1

    def disabled_enter(self):
        self.disabled_enter_count += 1

    def autonomous_enter(self):
        self.autonomous_enter_count += 1

    def teleop_enter(self):
        self.teleop_enter_count += 1

    def utility_enter(self):
        self.utility_enter_count += 1

    def robot_periodic(self):
        self.robot_periodic_count += 1

    def simulation_periodic(self):
        self.simulation_periodic_count += 1

    def disabled_periodic(self):
        self.disabled_periodic_count += 1

    def autonomous_periodic(self):
        self.autonomous_periodic_count += 1

    def teleop_periodic(self):
        self.teleop_periodic_count += 1

    def utility_periodic(self):
        self.utility_periodic_count += 1

    def disabled_exit(self):
        self.disabled_exit_count += 1

    def autonomous_exit(self):
        self.autonomous_exit_count += 1

    def teleop_exit(self):
        self.teleop_exit_count += 1

    def utility_exit(self):
        self.utility_exit_count += 1


def test_disabled_mode():
    robot = MockRobot()
    robot_thread = threading.Thread(target=robot.start_competition, daemon=True)
    robot_thread.start()
    wait_for_program_start()

    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()

    assert robot.simulation_init_count == 1
    assert robot.disabled_enter_count == 0
    assert robot.robot_periodic_count == 0
    assert robot.simulation_periodic_count == 0
    assert robot.disabled_periodic_count == 0

    step_timing(_PERIOD)

    assert robot.simulation_init_count == 1
    assert robot.disabled_enter_count == 1
    assert robot.autonomous_enter_count == 0
    assert robot.teleop_enter_count == 0
    assert robot.utility_enter_count == 0

    assert robot.robot_periodic_count == 1
    assert robot.simulation_periodic_count == 1
    assert robot.disabled_periodic_count == 1
    assert robot.autonomous_periodic_count == 0
    assert robot.teleop_periodic_count == 0
    assert robot.utility_periodic_count == 0

    assert robot.disabled_exit_count == 0

    step_timing(_PERIOD)

    assert robot.robot_periodic_count == 2
    assert robot.simulation_periodic_count == 2
    assert robot.disabled_periodic_count == 2

    robot.end_competition()
    robot_thread.join()


def test_autonomous_mode():
    robot = MockRobot()
    robot_thread = threading.Thread(target=robot.start_competition, daemon=True)
    robot_thread.start()
    wait_for_program_start()

    DriverStationSim.set_enabled(True)
    DriverStationSim.set_robot_mode(RobotMode.AUTONOMOUS)
    DriverStationSim.notify_new_data()

    assert robot.simulation_init_count == 1
    assert robot.disabled_enter_count == 0
    assert robot.autonomous_enter_count == 0
    assert robot.robot_periodic_count == 0

    step_timing(_PERIOD)

    assert robot.simulation_init_count == 1
    assert robot.disabled_enter_count == 0
    assert robot.autonomous_enter_count == 1
    assert robot.teleop_enter_count == 0
    assert robot.utility_enter_count == 0

    assert robot.robot_periodic_count == 1
    assert robot.simulation_periodic_count == 1
    assert robot.disabled_periodic_count == 0
    assert robot.autonomous_periodic_count == 1
    assert robot.teleop_periodic_count == 0
    assert robot.utility_periodic_count == 0

    assert robot.autonomous_exit_count == 0

    step_timing(_PERIOD)

    assert robot.robot_periodic_count == 2
    assert robot.simulation_periodic_count == 2
    assert robot.autonomous_periodic_count == 2

    robot.end_competition()
    robot_thread.join()


def test_teleop_mode():
    robot = MockRobot()
    robot_thread = threading.Thread(target=robot.start_competition, daemon=True)
    robot_thread.start()
    wait_for_program_start()

    DriverStationSim.set_enabled(True)
    DriverStationSim.set_robot_mode(RobotMode.TELEOPERATED)
    DriverStationSim.notify_new_data()

    assert robot.simulation_init_count == 1
    assert robot.teleop_enter_count == 0
    assert robot.robot_periodic_count == 0

    step_timing(_PERIOD)

    assert robot.disabled_enter_count == 0
    assert robot.autonomous_enter_count == 0
    assert robot.teleop_enter_count == 1
    assert robot.utility_enter_count == 0

    assert robot.robot_periodic_count == 1
    assert robot.simulation_periodic_count == 1
    assert robot.disabled_periodic_count == 0
    assert robot.autonomous_periodic_count == 0
    assert robot.teleop_periodic_count == 1
    assert robot.utility_periodic_count == 0

    step_timing(_PERIOD)

    assert robot.robot_periodic_count == 2
    assert robot.teleop_periodic_count == 2

    robot.end_competition()
    robot_thread.join()


def test_utility_mode():
    robot = MockRobot()
    robot_thread = threading.Thread(target=robot.start_competition, daemon=True)
    robot_thread.start()
    wait_for_program_start()

    DriverStationSim.set_enabled(True)
    DriverStationSim.set_robot_mode(RobotMode.UTILITY)
    DriverStationSim.notify_new_data()

    assert robot.simulation_init_count == 1
    assert robot.utility_enter_count == 0
    assert robot.robot_periodic_count == 0

    step_timing(_PERIOD)

    assert robot.utility_enter_count == 1
    assert robot.robot_periodic_count == 1
    assert robot.utility_periodic_count == 1

    step_timing(_PERIOD)

    assert robot.robot_periodic_count == 2
    assert robot.utility_periodic_count == 2

    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()
    step_timing(_PERIOD)

    assert robot.disabled_enter_count == 1
    assert robot.robot_periodic_count == 3
    assert robot.disabled_periodic_count == 1
    assert robot.utility_exit_count == 1

    robot.end_competition()
    robot_thread.join()


def test_mode_change():
    robot = MockRobot()
    robot_thread = threading.Thread(target=robot.start_competition, daemon=True)
    robot_thread.start()
    wait_for_program_start()

    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()

    assert robot.disabled_enter_count == 0
    assert robot.disabled_exit_count == 0

    step_timing(_PERIOD)

    assert robot.disabled_enter_count == 1
    assert robot.disabled_exit_count == 0

    DriverStationSim.set_enabled(True)
    DriverStationSim.set_robot_mode(RobotMode.AUTONOMOUS)
    DriverStationSim.notify_new_data()
    step_timing(_PERIOD)

    assert robot.disabled_enter_count == 1
    assert robot.autonomous_enter_count == 1
    assert robot.teleop_enter_count == 0
    assert robot.utility_enter_count == 0
    assert robot.disabled_exit_count == 1
    assert robot.autonomous_exit_count == 0

    DriverStationSim.set_robot_mode(RobotMode.TELEOPERATED)
    DriverStationSim.notify_new_data()
    step_timing(_PERIOD)

    assert robot.autonomous_enter_count == 1
    assert robot.teleop_enter_count == 1
    assert robot.utility_enter_count == 0
    assert robot.autonomous_exit_count == 1
    assert robot.teleop_exit_count == 0

    DriverStationSim.set_robot_mode(RobotMode.UTILITY)
    DriverStationSim.notify_new_data()
    step_timing(_PERIOD)

    assert robot.utility_enter_count == 1
    assert robot.teleop_exit_count == 1
    assert robot.utility_exit_count == 0

    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()
    step_timing(_PERIOD)

    assert robot.disabled_enter_count == 2
    assert robot.autonomous_enter_count == 1
    assert robot.teleop_enter_count == 1
    assert robot.utility_enter_count == 1
    assert robot.disabled_exit_count == 1
    assert robot.autonomous_exit_count == 1
    assert robot.teleop_exit_count == 1
    assert robot.utility_exit_count == 1

    robot.end_competition()
    robot_thread.join()


def test_add_periodic():
    robot = MockRobot()
    callback_count = 0

    def on_periodic() -> None:
        nonlocal callback_count
        callback_count += 1

    robot.add_periodic(on_periodic, _PERIOD / 2.0)

    robot_thread = threading.Thread(target=robot.start_competition, daemon=True)
    robot_thread.start()
    wait_for_program_start()

    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()

    assert robot.disabled_enter_count == 0
    assert robot.disabled_periodic_count == 0
    assert callback_count == 0

    step_timing(_PERIOD / 2.0)

    assert robot.disabled_enter_count == 0
    assert robot.disabled_periodic_count == 0
    assert callback_count == 1

    step_timing(_PERIOD / 2.0)

    assert robot.disabled_enter_count == 1
    assert robot.disabled_periodic_count == 1
    assert callback_count == 2

    robot.end_competition()
    robot_thread.join()


def test_add_periodic_with_offset():
    robot = MockRobot()
    callback_count = 0

    def on_periodic() -> None:
        nonlocal callback_count
        callback_count += 1

    robot.add_periodic(on_periodic, _PERIOD / 2.0, _PERIOD / 4.0)

    robot_thread = threading.Thread(target=robot.start_competition, daemon=True)
    robot_thread.start()
    wait_for_program_start()

    DriverStationSim.set_enabled(False)
    DriverStationSim.notify_new_data()

    assert robot.disabled_enter_count == 0
    assert robot.disabled_periodic_count == 0
    assert callback_count == 0

    step_timing(_PERIOD * 3.0 / 8.0)

    assert robot.disabled_enter_count == 0
    assert robot.disabled_periodic_count == 0
    assert callback_count == 0

    step_timing(_PERIOD * 3.0 / 8.0)

    assert robot.disabled_enter_count == 0
    assert robot.disabled_periodic_count == 0
    assert callback_count == 1

    step_timing(_PERIOD / 4.0)

    assert robot.disabled_enter_count == 1
    assert robot.disabled_periodic_count == 1
    assert callback_count == 1

    step_timing(_PERIOD / 4.0)

    assert robot.disabled_enter_count == 1
    assert robot.disabled_periodic_count == 1
    assert callback_count == 2

    robot.end_competition()
    robot_thread.join()
