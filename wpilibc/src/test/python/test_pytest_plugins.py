import pathlib
import sys

import pytest

from pytest_plugin_test_helpers import (
    _configure_isolated_plugin,
    _configure_robot_testing_plugin,
    _make_robot_module,
)


def test_robot_testing_plugin_success(pytester):
    _make_robot_module(pytester)
    _configure_robot_testing_plugin(pytester)
    pytester.makepyfile(test_success="""
def test_robot_fixture(robot):
    assert robot.did_init
""")

    result = pytester.runpytest("-vv")

    result.assert_outcomes(passed=1)


def test_robot_testing_plugin_failure_shows_output(pytester):
    _make_robot_module(pytester)
    _configure_robot_testing_plugin(pytester)
    pytester.makepyfile(test_failure="""
def test_robot_failure(robot):
    print("checked failure output")
    assert False
""")

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
    pytester.makepyfile(test_isolated="""
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
""")

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


def test_isolated_plugin_uses_robot_directory_during_robot_import(pytester):
    robot_dir = pytester.path / "robot_project"
    robot_dir.mkdir()
    robot_dir.joinpath("__init__.py").touch()
    robot_dir.joinpath("robot.py").write_text("""
import wpilib

IMPORT_OPERATING_DIRECTORY = wpilib.get_operating_directory()


class ImportDirectoryRobot(wpilib.TimedRobot):
    pass
""")
    _configure_isolated_plugin(
        pytester,
        robot_class="ImportDirectoryRobot",
        robot_module="robot_project.robot",
        robot_file_name="robot_project/robot.py",
    )
    pytester.makepyfile(test_isolated="""
import pathlib

from robot_project.robot import IMPORT_OPERATING_DIRECTORY


def test_operating_directory(robot, robot_file):
    assert pathlib.Path(IMPORT_OPERATING_DIRECTORY) == robot_file.parent.absolute()
""")

    result = pytester.runpytest_subprocess("-vv")

    result.assert_outcomes(passed=1)


def test_isolated_plugin_assertion_rendering(pytester):
    _make_robot_module(pytester)
    _configure_isolated_plugin(pytester)
    pytester.makepyfile(test_isolated="""
def test_robot_assertion_rendering(robot):
    assert "x" == "y"
""")

    result = pytester.runpytest_subprocess("-vv")

    result.assert_outcomes(failed=1)
    result.stdout.fnmatch_lines(
        [
            "*test_isolated.py::test_robot_assertion_rendering FAILED*",
            "*assert 'x' == 'y'*",
        ]
    )
    assert not any("_pytest/config/__init__.py" in line for line in result.outlines)


def test_isolated_plugin_no_duplicate_verbose_output(pytester):
    _make_robot_module(pytester)
    _configure_isolated_plugin(pytester)
    pytester.makepyfile(test_isolated="""
def test_non_robot():
    assert True


def test_robot_one(robot):
    assert robot is not None


def test_robot_two(robot):
    assert robot is not None
""")

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
    pytester.makepyfile(test_isolated="""
import os
import signal


def test_robot_signal_exit(robot):
    os.kill(os.getpid(), signal.SIGTERM)
""")

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
    pytester.makepyfile(test_isolated="""
def test_non_robot():
    assert True


def test_robot_one(robot):
    assert robot is not None


def test_robot_two(robot):
    assert robot is not None
""")

    result = pytester.runpytest_subprocess()

    result.assert_outcomes(passed=3)
    assert (
        sum(1 for line in result.outlines if line.startswith("test_isolated.py")) == 1
    )


def test_isolated_plugin_reports_worker_collection_exit(pytester):
    _make_robot_module(pytester)
    _configure_isolated_plugin(pytester)
    with pytester.path.joinpath("conftest.py").open("a") as fp:
        fp.write("""


def pytest_collection_modifyitems(config, items):
    if "--no-header" in config.invocation_params.args:
        items.clear()
""")
    pytester.makepyfile(test_isolated="""
def test_robot(robot):
    assert robot is not None
""")

    result = pytester.runpytest_subprocess("-v")

    result.assert_outcomes(failed=1)
    result.stdout.fnmatch_lines(["*subprocess exited with exit code 5*"])


def test_isolated_plugin_maxfail_stops_early(pytester):
    _make_robot_module(pytester)
    _configure_isolated_plugin(pytester)
    pytester.makepyfile(test_isolated="""
def test_robot_first(robot):
    assert False


def test_robot_second(robot):
    assert False
""")

    result = pytester.runpytest_subprocess("-v", "-x")

    result.assert_outcomes(failed=1)
    assert not any("test_robot_second" in line for line in result.outlines)


def test_isolated_plugin_maxfail_stops_deferred_tests(pytester):
    _make_robot_module(pytester)
    _configure_isolated_plugin(pytester, parallelism=2)
    pytester.makepyfile(test_isolated="""
import pathlib
import time


def test_robot_failure(robot):
    assert False


def test_plain_waits_for_failure():
    time.sleep(1)


def test_plain_must_not_run():
    pathlib.Path("plain-ran").touch()
""")

    result = pytester.runpytest_subprocess("-v", "-x")

    assert result.parseoutcomes()["failed"] == 1
    assert result.ret == pytest.ExitCode.TESTS_FAILED
    assert not (pytester.path / "plain-ran").exists()


@pytest.mark.parametrize("isolated", [False, True])
def test_builtin_tests_module(pytester, isolated):
    _make_robot_module(pytester)
    if isolated:
        _configure_isolated_plugin(pytester, robot_class="DummyRobot")
    else:
        _configure_robot_testing_plugin(pytester, robot_class="DummyRobot")
    pytester.makepyfile(pyfrc_test="from wpilib.testing.robot_tests import *\n")

    result = pytester.runpytest_subprocess("-q")

    result.assert_outcomes(passed=4, skipped=1)


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


@pytest.mark.parametrize(
    "a_fixture, b_fixture",
    [("robot", "robot"), ("robot", ""), ("", "robot")],
    ids=["RR", "RN", "NR"],
)
def test_unordered_tests_still_run_in_parallel(pytester, a_fixture, b_fixture):
    """
    Tests WITHOUT @pytest.mark.order must not be serialised by order-marker
    support.  With parallelism=2, two 1.5 s tests must overlap in wall-clock
    time.

    NR is the case collection order alone cannot deliver: the plain test is
    collected first, so following the given order would run it to completion
    before the subprocess was even spawned.  Within a group the run loop is free
    to schedule for throughput, so it starts the isolated test before running any
    in-process test and the two overlap regardless of which was collected first.

    NN is omitted: two in-process tests both run here, and this process runs one
    test at a time -- serial by design, and no scheduling can change it.
    """
    _make_robot_module(pytester)
    _configure_isolated_plugin(pytester, parallelism=2)

    def params(f):
        return f"({f})" if f else "()"

    pytester.makepyfile(test_parallel_execution=f"""\
import pathlib
import time


def test_a{params(a_fixture)}:
    pathlib.Path("a_start.txt").write_text(str(time.monotonic()))
    time.sleep(1.5)
    pathlib.Path("a_end.txt").write_text(str(time.monotonic()))


def test_b{params(b_fixture)}:
    pathlib.Path("b_start.txt").write_text(str(time.monotonic()))
    time.sleep(1.5)
    pathlib.Path("b_end.txt").write_text(str(time.monotonic()))
""")

    result = pytester.runpytest_subprocess("-vv")
    result.assert_outcomes(passed=2)

    root = pathlib.Path(pytester.path)
    a_end = float((root / "a_end.txt").read_text())
    b_start = float((root / "b_start.txt").read_text())
    assert (
        b_start < a_end
    ), f"Expected parallel: b_start={b_start:.3f} a_end={a_end:.3f}"
