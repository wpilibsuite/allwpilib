import threading

import pytest

from wpilib import TimedRobot
from wpilib.simulation import (
    DriverStationSim,
    pauseTiming,
    resumeTiming,
    setProgramStarted,
    stepTiming,
    waitForProgramStart,
)
from hal._wpiHal import _RobotMode

_PERIOD = 0.02  # 20 ms


@pytest.fixture(autouse=True)
def timed_robot_setup(wpilib_state):
    pauseTiming()
    setProgramStarted(False)
    yield
    resumeTiming()


class MockRobot(TimedRobot):
    def __init__(self):
        super().__init__(_PERIOD)
        self.simulation_init_count = 0
        self.disabled_init_count = 0
        self.autonomous_init_count = 0
        self.teleop_init_count = 0
        self.utility_init_count = 0

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

    def simulationInit(self):
        self.simulation_init_count += 1

    def disabledInit(self):
        self.disabled_init_count += 1

    def autonomousInit(self):
        self.autonomous_init_count += 1

    def teleopInit(self):
        self.teleop_init_count += 1

    def utilityInit(self):
        self.utility_init_count += 1

    def robotPeriodic(self):
        self.robot_periodic_count += 1

    def simulationPeriodic(self):
        self.simulation_periodic_count += 1

    def disabledPeriodic(self):
        self.disabled_periodic_count += 1

    def autonomousPeriodic(self):
        self.autonomous_periodic_count += 1

    def teleopPeriodic(self):
        self.teleop_periodic_count += 1

    def utilityPeriodic(self):
        self.utility_periodic_count += 1

    def disabledExit(self):
        self.disabled_exit_count += 1

    def autonomousExit(self):
        self.autonomous_exit_count += 1

    def teleopExit(self):
        self.teleop_exit_count += 1

    def utilityExit(self):
        self.utility_exit_count += 1


def test_disabled_mode():
    robot = MockRobot()
    robot_thread = threading.Thread(target=robot.startCompetition, daemon=True)
    robot_thread.start()
    waitForProgramStart()

    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()

    assert robot.simulation_init_count == 1
    assert robot.disabled_init_count == 0
    assert robot.robot_periodic_count == 0
    assert robot.simulation_periodic_count == 0
    assert robot.disabled_periodic_count == 0

    stepTiming(_PERIOD)

    assert robot.simulation_init_count == 1
    assert robot.disabled_init_count == 1
    assert robot.autonomous_init_count == 0
    assert robot.teleop_init_count == 0
    assert robot.utility_init_count == 0

    assert robot.robot_periodic_count == 1
    assert robot.simulation_periodic_count == 1
    assert robot.disabled_periodic_count == 1
    assert robot.autonomous_periodic_count == 0
    assert robot.teleop_periodic_count == 0
    assert robot.utility_periodic_count == 0

    assert robot.disabled_exit_count == 0

    stepTiming(_PERIOD)

    assert robot.robot_periodic_count == 2
    assert robot.simulation_periodic_count == 2
    assert robot.disabled_periodic_count == 2

    robot.endCompetition()
    robot_thread.join()


def test_autonomous_mode():
    robot = MockRobot()
    robot_thread = threading.Thread(target=robot.startCompetition, daemon=True)
    robot_thread.start()
    waitForProgramStart()

    DriverStationSim.setEnabled(True)
    DriverStationSim.setRobotMode(_RobotMode.AUTONOMOUS)
    DriverStationSim.notifyNewData()

    assert robot.simulation_init_count == 1
    assert robot.disabled_init_count == 0
    assert robot.autonomous_init_count == 0
    assert robot.robot_periodic_count == 0

    stepTiming(_PERIOD)

    assert robot.simulation_init_count == 1
    assert robot.disabled_init_count == 0
    assert robot.autonomous_init_count == 1
    assert robot.teleop_init_count == 0
    assert robot.utility_init_count == 0

    assert robot.robot_periodic_count == 1
    assert robot.simulation_periodic_count == 1
    assert robot.disabled_periodic_count == 0
    assert robot.autonomous_periodic_count == 1
    assert robot.teleop_periodic_count == 0
    assert robot.utility_periodic_count == 0

    assert robot.autonomous_exit_count == 0

    stepTiming(_PERIOD)

    assert robot.robot_periodic_count == 2
    assert robot.simulation_periodic_count == 2
    assert robot.autonomous_periodic_count == 2

    robot.endCompetition()
    robot_thread.join()


def test_teleop_mode():
    robot = MockRobot()
    robot_thread = threading.Thread(target=robot.startCompetition, daemon=True)
    robot_thread.start()
    waitForProgramStart()

    DriverStationSim.setEnabled(True)
    DriverStationSim.setRobotMode(_RobotMode.TELEOPERATED)
    DriverStationSim.notifyNewData()

    assert robot.simulation_init_count == 1
    assert robot.teleop_init_count == 0
    assert robot.robot_periodic_count == 0

    stepTiming(_PERIOD)

    assert robot.disabled_init_count == 0
    assert robot.autonomous_init_count == 0
    assert robot.teleop_init_count == 1
    assert robot.utility_init_count == 0

    assert robot.robot_periodic_count == 1
    assert robot.simulation_periodic_count == 1
    assert robot.disabled_periodic_count == 0
    assert robot.autonomous_periodic_count == 0
    assert robot.teleop_periodic_count == 1
    assert robot.utility_periodic_count == 0

    stepTiming(_PERIOD)

    assert robot.robot_periodic_count == 2
    assert robot.teleop_periodic_count == 2

    robot.endCompetition()
    robot_thread.join()


def test_utility_mode():
    robot = MockRobot()
    robot_thread = threading.Thread(target=robot.startCompetition, daemon=True)
    robot_thread.start()
    waitForProgramStart()

    DriverStationSim.setEnabled(True)
    DriverStationSim.setRobotMode(_RobotMode.UTILITY)
    DriverStationSim.notifyNewData()

    assert robot.simulation_init_count == 1
    assert robot.utility_init_count == 0
    assert robot.robot_periodic_count == 0

    stepTiming(_PERIOD)

    assert robot.utility_init_count == 1
    assert robot.robot_periodic_count == 1
    assert robot.utility_periodic_count == 1

    stepTiming(_PERIOD)

    assert robot.robot_periodic_count == 2
    assert robot.utility_periodic_count == 2

    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()
    stepTiming(_PERIOD)

    assert robot.disabled_init_count == 1
    assert robot.robot_periodic_count == 3
    assert robot.disabled_periodic_count == 1
    assert robot.utility_exit_count == 1

    robot.endCompetition()
    robot_thread.join()


def test_mode_change():
    robot = MockRobot()
    robot_thread = threading.Thread(target=robot.startCompetition, daemon=True)
    robot_thread.start()
    waitForProgramStart()

    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()

    assert robot.disabled_init_count == 0
    assert robot.disabled_exit_count == 0

    stepTiming(_PERIOD)

    assert robot.disabled_init_count == 1
    assert robot.disabled_exit_count == 0

    DriverStationSim.setEnabled(True)
    DriverStationSim.setRobotMode(_RobotMode.AUTONOMOUS)
    DriverStationSim.notifyNewData()
    stepTiming(_PERIOD)

    assert robot.disabled_init_count == 1
    assert robot.autonomous_init_count == 1
    assert robot.teleop_init_count == 0
    assert robot.utility_init_count == 0
    assert robot.disabled_exit_count == 1
    assert robot.autonomous_exit_count == 0

    DriverStationSim.setRobotMode(_RobotMode.TELEOPERATED)
    DriverStationSim.notifyNewData()
    stepTiming(_PERIOD)

    assert robot.autonomous_init_count == 1
    assert robot.teleop_init_count == 1
    assert robot.utility_init_count == 0
    assert robot.autonomous_exit_count == 1
    assert robot.teleop_exit_count == 0

    DriverStationSim.setRobotMode(_RobotMode.UTILITY)
    DriverStationSim.notifyNewData()
    stepTiming(_PERIOD)

    assert robot.utility_init_count == 1
    assert robot.teleop_exit_count == 1
    assert robot.utility_exit_count == 0

    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()
    stepTiming(_PERIOD)

    assert robot.disabled_init_count == 2
    assert robot.autonomous_init_count == 1
    assert robot.teleop_init_count == 1
    assert robot.utility_init_count == 1
    assert robot.disabled_exit_count == 1
    assert robot.autonomous_exit_count == 1
    assert robot.teleop_exit_count == 1
    assert robot.utility_exit_count == 1

    robot.endCompetition()
    robot_thread.join()


def test_add_periodic():
    robot = MockRobot()
    callback_count = [0]
    robot.addPeriodic(lambda: callback_count.__setitem__(0, callback_count[0] + 1), _PERIOD / 2.0)

    robot_thread = threading.Thread(target=robot.startCompetition, daemon=True)
    robot_thread.start()
    waitForProgramStart()

    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()

    assert robot.disabled_init_count == 0
    assert robot.disabled_periodic_count == 0
    assert callback_count[0] == 0

    stepTiming(_PERIOD / 2.0)

    assert robot.disabled_init_count == 0
    assert robot.disabled_periodic_count == 0
    assert callback_count[0] == 1

    stepTiming(_PERIOD / 2.0)

    assert robot.disabled_init_count == 1
    assert robot.disabled_periodic_count == 1
    assert callback_count[0] == 2

    robot.endCompetition()
    robot_thread.join()


def test_add_periodic_with_offset():
    robot = MockRobot()
    callback_count = [0]
    robot.addPeriodic(
        lambda: callback_count.__setitem__(0, callback_count[0] + 1),
        _PERIOD / 2.0,
        _PERIOD / 4.0,
    )

    robot_thread = threading.Thread(target=robot.startCompetition, daemon=True)
    robot_thread.start()
    waitForProgramStart()

    DriverStationSim.setEnabled(False)
    DriverStationSim.notifyNewData()

    assert robot.disabled_init_count == 0
    assert robot.disabled_periodic_count == 0
    assert callback_count[0] == 0

    stepTiming(_PERIOD * 3.0 / 8.0)

    assert robot.disabled_init_count == 0
    assert robot.disabled_periodic_count == 0
    assert callback_count[0] == 0

    stepTiming(_PERIOD * 3.0 / 8.0)

    assert robot.disabled_init_count == 0
    assert robot.disabled_periodic_count == 0
    assert callback_count[0] == 1

    stepTiming(_PERIOD / 4.0)

    assert robot.disabled_init_count == 1
    assert robot.disabled_periodic_count == 1
    assert callback_count[0] == 1

    stepTiming(_PERIOD / 4.0)

    assert robot.disabled_init_count == 1
    assert robot.disabled_periodic_count == 1
    assert callback_count[0] == 2

    robot.endCompetition()
    robot_thread.join()
