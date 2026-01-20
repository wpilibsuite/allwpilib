import pathlib
import sys

import pytest


def _make_robot_module(pytester):
    pytester.makepyfile(
        robot_module="""
import wpilib


class DummyRobot(wpilib.TimedRobot):
    def __init__(self):
        super().__init__()
        self.did_init = True


class AutonomousPeriodicFailed(wpilib.TimedRobot):
    def autonomousPeriodic(self):
        assert False


class TeleopPeriodicFailed(wpilib.TimedRobot):
    def teleopPeriodic(self):
        assert False


class TeleopInitFailed(wpilib.TimedRobot):
    def teleopInit(self):
        assert False


class IterativeStateRobot(wpilib.TimedRobot):

    def disabledInit(self):
        self.did_disabled_init = True

    def disabledPeriodic(self):
        self.did_disabled_periodic = True

    def autonomousInit(self):
        self.did_auto_init = True

    def autonomousPeriodic(self):
        self.did_auto_periodic = True

    def teleopInit(self):
        self.did_teleop_init = True

    def teleopPeriodic(self):
        self.did_teleop_periodic = True

"""
    )


def _configure_robot_testing_plugin(pytester, robot_class="DummyRobot"):
    pytester.makeconftest(
        f"""
import pathlib

from wpilib.testing.pytest_plugin import RobotTestingPlugin

from robot_module import {robot_class}


def pytest_configure(config):
    robot_file = pathlib.Path(__file__).resolve()
    config.pluginmanager.register(RobotTestingPlugin({robot_class}, robot_file, False))
"""
    )


def _configure_isolated_plugin(pytester, parallelism=1, robot_class="DummyRobot"):
    pytester.makeconftest(
        f"""
import pathlib

from wpilib.testing.pytest_isolated_tests_plugin import IsolatedTestsPlugin

from robot_module import {robot_class}

def pytest_configure(config):
    if "--no-header" in config.invocation_params.args:
        return
    robot_file = pathlib.Path(__file__).resolve()
    config.pluginmanager.register(
        IsolatedTestsPlugin({robot_class}, robot_file, False, False, {parallelism})
    )
"""
    )


def test_robot_testing_plugin_success(pytester):
    _make_robot_module(pytester)
    _configure_robot_testing_plugin(pytester)
    pytester.makepyfile(
        test_success="""
def test_robot_fixture(robot):
    assert robot.did_init
"""
    )

    result = pytester.runpytest("-vv")

    result.assert_outcomes(passed=1)


def test_robot_testing_plugin_failure_shows_output(pytester):
    _make_robot_module(pytester)
    _configure_robot_testing_plugin(pytester)
    pytester.makepyfile(
        test_failure="""
def test_robot_failure(robot):
    print("checked failure output")
    assert False
"""
    )

    result = pytester.runpytest("-vv")

    result.assert_outcomes(failed=1)
    result.stdout.fnmatch_lines(
        [
            "*test_failure.py::test_robot_failure FAILED*",
            "*checked failure output*",
        ]
    )


def test_isolated_plugin_process_and_output(pytester):
    _make_robot_module(pytester)
    _configure_isolated_plugin(pytester)
    pytester.makepyfile(
        test_isolated="""
import os


def test_non_robot_pid():
    with open("non_robot_pid.txt", "w") as fp:
        fp.write(str(os.getpid()))


def test_robot_pid_one(robot):
    with open("robot_pid_one.txt", "w") as fp:
        fp.write(str(os.getpid()))


def test_robot_pid_two(robot):
    with open("robot_pid_two.txt", "w") as fp:
        fp.write(str(os.getpid()))


def test_robot_failure_output(robot):
    print("isolated failure output")
    assert False
"""
    )

    result = pytester.runpytest_subprocess("-vv")

    result.assert_outcomes(passed=3, failed=1)
    result.stdout.fnmatch_lines(
        [
            "*test_isolated.py::test_robot_failure_output FAILED*",
            "*isolated failure output*",
        ]
    )

    root = pathlib.Path(pytester.path)
    main_pid = int(root.joinpath("non_robot_pid.txt").read_text())
    robot_pid_one = int(root.joinpath("robot_pid_one.txt").read_text())
    robot_pid_two = int(root.joinpath("robot_pid_two.txt").read_text())

    assert robot_pid_one != main_pid
    assert robot_pid_two != main_pid
    assert robot_pid_one != robot_pid_two


def test_isolated_plugin_no_duplicate_verbose_output(pytester):
    _make_robot_module(pytester)
    _configure_isolated_plugin(pytester)
    pytester.makepyfile(
        test_isolated="""
def test_non_robot():
    assert True


def test_robot_one(robot):
    assert robot is not None


def test_robot_two(robot):
    assert robot is not None
"""
    )

    result = pytester.runpytest_subprocess("-v")

    result.assert_outcomes(passed=3)
    assert (
        sum(1 for line in result.outlines if "test_isolated.py::test_robot_one" in line)
        == 1
    )
    assert (
        sum(1 for line in result.outlines if "test_isolated.py::test_robot_two" in line)
        == 1
    )


@pytest.mark.skipif(
    sys.platform.startswith("win"),
    reason="Process signal exits do not work on Windows",
)
def test_isolated_plugin_reports_signal_exit(pytester):
    _make_robot_module(pytester)
    _configure_isolated_plugin(pytester)
    pytester.makepyfile(
        test_isolated="""
import os
import signal


def test_robot_signal_exit(robot):
    os.kill(os.getpid(), signal.SIGTERM)
"""
    )

    result = pytester.runpytest_subprocess("-vv")

    result.assert_outcomes(failed=1)
    result.stdout.fnmatch_lines(
        [
            "*test_isolated.py::test_robot_signal_exit FAILED*",
            "*Terminated*",
        ]
    )


def test_isolated_plugin_shows_file_in_non_verbose_output(pytester):
    _make_robot_module(pytester)
    _configure_isolated_plugin(pytester)
    pytester.makepyfile(
        test_isolated="""
def test_non_robot():
    assert True


def test_robot_one(robot):
    assert robot is not None


def test_robot_two(robot):
    assert robot is not None
"""
    )

    result = pytester.runpytest_subprocess()

    result.assert_outcomes(passed=3)
    assert (
        sum(1 for line in result.outlines if line.startswith("test_isolated.py")) == 1
    )


def test_isolated_plugin_maxfail_stops_early(pytester):
    _make_robot_module(pytester)
    _configure_isolated_plugin(pytester)
    pytester.makepyfile(
        test_isolated="""
def test_robot_first(robot):
    assert False


def test_robot_second(robot):
    assert False
"""
    )

    result = pytester.runpytest_subprocess("-v", "-x")

    result.assert_outcomes(failed=1)
    assert not any("test_robot_second" in line for line in result.outlines)


@pytest.mark.parametrize("isolated", [False, True])
def test_builtin_tests_module(pytester, isolated):
    _make_robot_module(pytester)
    if isolated:
        _configure_isolated_plugin(pytester, robot_class="DummyRobot")
    else:
        _configure_robot_testing_plugin(pytester, robot_class="DummyRobot")
    pytester.makepyfile(pyfrc_test="from wpilib.testing.robot_tests import *\n")

    result = pytester.runpytest_subprocess("-q")

    result.assert_outcomes(passed=4)


def _run_robot_suite(pytester, isolated, robot_class, test_source, *args):
    _make_robot_module(pytester)
    if isolated:
        _configure_isolated_plugin(pytester, robot_class=robot_class)
    else:
        _configure_robot_testing_plugin(pytester, robot_class=robot_class)
    pytester.makepyfile(test_robot=test_source)
    return pytester.runpytest_subprocess(*args)


_AUTO_FAILURES = [
    "AutonomousPeriodicFailed",
]

_TELEOP_FAILURES = [
    "TeleopPeriodicFailed",
    "TeleopInitFailed",
]


@pytest.mark.parametrize("isolated", [False, True])
@pytest.mark.parametrize("robot_class", _AUTO_FAILURES)
def test_autonomous_failure_detection(pytester, isolated, robot_class):
    result = _run_robot_suite(
        pytester,
        isolated,
        robot_class,
        """
def test_autonomous_failure(robot, control):
    with control.run_robot():
        control.step_timing(seconds=0.4, autonomous=True, enabled=True)
""",
        "-vv",
    )

    result.assert_outcomes(failed=1)


@pytest.mark.parametrize("isolated", [False, True])
@pytest.mark.parametrize("robot_class", _TELEOP_FAILURES)
def test_teleop_failure_detection(pytester, isolated, robot_class):
    result = _run_robot_suite(
        pytester,
        isolated,
        robot_class,
        """
def test_teleop_failure(robot, control):
    with control.run_robot():
        control.step_timing(seconds=0.4, autonomous=False, enabled=True)
""",
        "-vv",
    )

    result.assert_outcomes(failed=1)


@pytest.mark.parametrize("isolated", [False, True])
@pytest.mark.parametrize("robot_class", ["IterativeStateRobot"])
def test_robot_state_transitions(pytester, isolated, robot_class):
    expected = {
        "IterativeStateRobot": [
            "did_disabled_init",
            "did_disabled_periodic",
            "did_auto_init",
            "did_auto_periodic",
            "did_teleop_init",
            "did_teleop_periodic",
        ],
    }[robot_class]

    result = _run_robot_suite(
        pytester,
        isolated,
        robot_class,
        f"""
def test_state_transitions(robot, control):
    with control.run_robot():
        control.step_timing(seconds=0.4, autonomous=False, enabled=False)
        control.step_timing(seconds=0.4, autonomous=True, enabled=True)
        control.step_timing(seconds=0.4, autonomous=False, enabled=True)

        expected = {{name: True for name in {expected!r}}}
        attrs = {{name: getattr(robot, name, False) for name in {expected!r}}}
        assert expected == attrs
""",
        "-vv",
    )

    result.assert_outcomes(passed=1)
