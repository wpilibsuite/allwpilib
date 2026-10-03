import json
import subprocess
import sys

import pytest

from pytest_plugin_test_helpers import (
    _configure_isolated_plugin,
    _configure_robot_testing_plugin,
)


def _make_opmode_robot(pytester, *, fail_stage=None, startup=False):
    pytester.makepyfile(robot_module=f"""
import json
import os
import pathlib

import hal
import wpilib
from hal import RobotMode


def record(name, event):
    with pathlib.Path(__file__).with_name(f"{{name}}.jsonl").open("a") as fp:
        fp.write(json.dumps([event, os.getpid()]) + "\\n")


class CheckedOpMode(wpilib.OpMode):
    def __init__(self, name, mode):
        super().__init__()
        self.name = name
        self.mode = mode
        self.did_disabled = False
        self.did_start = False
        self.did_periodic = False
        self.event("construct")

    def event(self, event):
        record(self.name, event)
        if self.name == "Auto" and event == {fail_stage!r}:
            raise ValueError("opmode failed in " + event)

    def disabled_periodic(self):
        self.did_disabled = True

    def start(self):
        assert self.did_disabled
        assert wpilib.RobotState.is_enabled()
        assert int(hal.opmode_get_robot_mode(wpilib.RobotState.get_opmode_id())) == int(self.mode)
        self.did_start = True
        self.event("start")

    def periodic(self):
        assert self.did_start
        self.did_periodic = True
        # Don't generate hundreds of log entries
        if not getattr(self, "recorded_periodic", False):
            self.recorded_periodic = True
            self.event("periodic")

    def end(self):
        assert self.did_periodic
        assert wpilib.RobotState.is_disabled()
        self.event("end")


class TestRobot(wpilib.OpModeRobot):
    def __init__(self):
        super().__init__()
        record("robot", "construct")
        # Discovery and tests must each have clean HAL handles.
        self.output = wpilib.DigitalOutput(0)
        if not {startup!r}:
            self.register_modes()

    def simulation_init(self):
        if {startup!r}:
            import time
            time.sleep(0.02)
            self.register_modes()

    def register_modes(self):
        for name, mode in [
            ("Utility", RobotMode.UTILITY),
            ("Teleop", RobotMode.TELEOPERATED),
            ("Auto", RobotMode.AUTONOMOUS),
        ]:
            self.add_opmode_factory(
                mode, name, lambda name=name, mode=mode: CheckedOpMode(name, mode)
            )
        self.publish_opmodes()
""")


def _configure(pytester, isolated):
    if isolated:
        _configure_isolated_plugin(pytester, robot_class="TestRobot")
    else:
        _configure_robot_testing_plugin(pytester, robot_class="TestRobot")


def _events(pytester, name):
    return [
        json.loads(line)
        for line in (pytester.path / f"{name}.jsonl").read_text().splitlines()
    ]


@pytest.mark.parametrize("isolated", [False, True])
@pytest.mark.parametrize("startup", [False, True])
def test_builtin_opmodes_run_complete_lifecycle(pytester, isolated, startup):
    # Missing selection, enable/disable transitions, utility support, or cleanup
    # must prevent one of the lifecycle records below.
    _make_opmode_robot(pytester, startup=startup)
    _configure(pytester, isolated)
    pytester.makepyfile(test_robot="from wpilib.testing.robot_tests import *")

    result = pytester.runpytest_subprocess("-vv")

    result.assert_outcomes(passed=7)
    pids = []
    for name in ["Auto", "Teleop", "Utility"]:
        events = _events(pytester, name)
        assert [event for event, _ in events[:4]] == [
            "construct",
            "start",
            "periodic",
            "end",
        ]
        pids.append(events[0][1])
    if isolated:
        assert len(set(pids)) == 3
    # One discovery robot plus one per test, not rediscovery in every worker.
    robots = _events(pytester, "robot")
    assert len(robots) == 8
    assert robots[0][1] not in pids


@pytest.mark.parametrize("isolated", [False, True])
@pytest.mark.parametrize("stage", ["construct", "start", "periodic", "end"])
def test_failing_opmode_does_not_prevent_others(pytester, isolated, stage):
    _make_opmode_robot(pytester, fail_stage=stage)
    _configure(pytester, isolated)
    pytester.makepyfile(test_robot="from wpilib.testing.robot_tests import *")

    result = pytester.runpytest_subprocess("-vv", "-k", "test_opmode")

    result.assert_outcomes(passed=2, failed=1, deselected=4)
    result.stdout.fnmatch_lines([f"*ValueError: opmode failed in {stage}*"])
    for name in ["Teleop", "Utility"]:
        assert "end" in [event for event, _ in _events(pytester, name)]


@pytest.mark.parametrize("isolated", [False, True])
def test_opmode_collection_does_not_construct_modes(pytester, isolated):
    _make_opmode_robot(pytester, fail_stage="construct")
    _configure(pytester, isolated)
    pytester.makepyfile(test_robot="from wpilib.testing.robot_tests import *")

    result = pytester.runpytest_subprocess("--collect-only", "-q")

    assert result.ret == pytest.ExitCode.OK
    for name, mode in [
        ("Auto", "AUTONOMOUS"),
        ("Teleop", "TELEOPERATED"),
        ("Utility", "UTILITY"),
    ]:
        assert f"test_robot.py::test_opmode[{mode}-{name}]" in result.outlines
        assert not (pytester.path / f"{name}.jsonl").exists()
    assert len(_events(pytester, "robot")) == 1


@pytest.mark.parametrize("isolated", [False, True])
def test_discovery_is_lazy_for_tests_without_opmode_fixture(pytester, isolated):
    _make_opmode_robot(pytester)
    _configure(pytester, isolated)
    pytester.makepyfile(test_robot="def test_robot(robot): pass")

    result = pytester.runpytest_subprocess("-q")

    result.assert_outcomes(passed=1)
    assert len(_events(pytester, "robot")) == 1


@pytest.mark.parametrize("isolated", [False, True])
def test_custom_opmode_fixture_and_node_selection(pytester, isolated):
    _make_opmode_robot(pytester)
    _configure(pytester, isolated)
    pytester.makepyfile(test_robot="""
def test_selected(control, opmode):
    from hal import RobotMode
    from wpilib.testing import OpMode

    assert opmode == OpMode(mode=RobotMode.UTILITY, name="Utility")
    with control.run_robot():
        control.step_timing(seconds=0.5, opmode=opmode, enabled=False)
        control.step_timing(seconds=0.5, opmode=opmode, enabled=True)
        control.step_timing(seconds=0.5, opmode=opmode, enabled=False)
""")

    result = pytester.runpytest_subprocess(
        "-q", "test_robot.py::test_selected[UTILITY-Utility]"
    )

    result.assert_outcomes(passed=1)
    assert "end" in [event for event, _ in _events(pytester, "Utility")]
    assert not (pytester.path / "Auto.jsonl").exists()
    assert not (pytester.path / "Teleop.jsonl").exists()


@pytest.mark.parametrize("isolated", [False, True])
def test_builtin_opmode_cli_path(pytester, isolated):
    _make_opmode_robot(pytester)
    result = subprocess.run(
        [
            sys.executable,
            "-m",
            "robotpy",
            "--main",
            "robot_module.py",
            "test",
            "--builtin",
            "--isolated" if isolated else "--no-isolation",
            "-j",
            "2",
            "--",
            "-v",
        ],
        cwd=pytester.path,
        capture_output=True,
        text=True,
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "7 passed" in result.stdout
    for name in ["Auto", "Teleop", "Utility"]:
        assert "end" in [event for event, _ in _events(pytester, name)]


@pytest.mark.parametrize("isolated", [False, True])
def test_opmode_fail_fast(pytester, isolated):
    _make_opmode_robot(pytester, fail_stage="start")
    _configure(pytester, isolated)
    pytester.makepyfile(test_robot="from wpilib.testing.robot_tests import *")

    result = pytester.runpytest_subprocess("-q", "-x", "-k", "test_opmode")

    result.assert_outcomes(failed=1, deselected=4)
    assert not (pytester.path / "Teleop.jsonl").exists()
    assert not (pytester.path / "Utility.jsonl").exists()


@pytest.mark.parametrize("stage", ["__init__", "simulation_init"])
def test_discovery_reports_startup_exception(pytester, stage):
    pytester.makepyfile(robot_module=f"""
import wpilib

class TestRobot(wpilib.OpModeRobot):
    def {stage}(self):
        {"super().__init__()" if stage == "__init__" else "pass"}
        raise ValueError("broken startup")
""")
    _configure(pytester, False)
    pytester.makepyfile(test_robot="from wpilib.testing.robot_tests import *")

    result = pytester.runpytest_subprocess("--collect-only", "-q", timeout=10)

    assert result.ret != pytest.ExitCode.OK
    result.stdout.fnmatch_lines(["*ValueError: broken startup*"])


@pytest.mark.parametrize("isolated", [False, True])
@pytest.mark.parametrize(
    "source",
    [
        """
import pytest

@pytest.mark.parametrize("opmode", ["custom", "another"])
def test_custom(opmode):
    assert opmode in ("custom", "another")
""",
        """
import pytest

@pytest.fixture(params=["custom", "another"])
def opmode(request):
    return request.param

def test_custom(opmode):
    assert opmode in ("custom", "another")
""",
    ],
)
def test_user_opmode_parameter_or_fixture_is_not_overridden(pytester, isolated, source):
    _make_opmode_robot(pytester)
    _configure(pytester, isolated)
    pytester.makepyfile(test_robot=source)

    result = pytester.runpytest_subprocess("-q")

    result.assert_outcomes(passed=2)
    assert not (pytester.path / "robot.jsonl").exists()


@pytest.mark.parametrize("isolated", [False, True])
def test_legacy_timing_preserves_manually_selected_opmode(pytester, isolated):
    _make_opmode_robot(pytester)
    _configure(pytester, isolated)
    pytester.makepyfile(test_robot="""
from wpilib.simulation import DriverStationSim

def test_manual_selection(control):
    with control.run_robot():
        options = DriverStationSim.get_opmode_options()
        selected = next(option.id for option in options if option.name == "Auto")
        DriverStationSim.set_opmode(selected)
        control.step_timing(seconds=0.5, autonomous=True, enabled=False)
        control.step_timing(seconds=0.5, autonomous=True, enabled=True)
        control.step_timing(seconds=0.5, autonomous=True, enabled=False)
""")

    result = pytester.runpytest_subprocess("-q")

    result.assert_outcomes(passed=1)
    assert "end" in [event for event, _ in _events(pytester, "Auto")]


@pytest.mark.parametrize("isolated", [False, True])
def test_same_name_in_different_robot_modes(pytester, isolated):
    _make_opmode_robot(pytester)
    with (pytester.path / "robot_module.py").open("a") as fp:
        fp.write("""

class TestRobot(TestRobot):
    def register_modes(self):
        self.add_opmode_factory(
            RobotMode.AUTONOMOUS, "Shared",
            lambda: CheckedOpMode("Auto", RobotMode.AUTONOMOUS),
        )
        self.add_opmode_factory(
            RobotMode.UTILITY, "Shared",
            lambda: CheckedOpMode("Utility", RobotMode.UTILITY),
        )
        self.publish_opmodes()
""")
    _configure(pytester, isolated)
    pytester.makepyfile(test_robot="""
from wpilib.testing.robot_tests import test_opmode

def test_named_opmode(opmode):
    assert opmode.name == "Shared"
""")

    result = pytester.runpytest_subprocess("-q")

    result.assert_outcomes(passed=4)
    for name in ["Auto", "Utility"]:
        assert "end" in [event for event, _ in _events(pytester, name)]


@pytest.mark.parametrize("isolated", [False, True])
def test_missing_opmode_error_includes_name_and_robot_mode(pytester, isolated):
    _make_opmode_robot(pytester)
    _configure(pytester, isolated)
    pytester.makepyfile(test_robot="""
def test_missing(control):
    from hal import RobotMode
    from wpilib.testing import OpMode

    with control.run_robot():
        control.step_timing(
            seconds=0.5,
            opmode=OpMode(RobotMode.UTILITY, "Missing mode"),
            enabled=False,
        )
""")

    result = pytester.runpytest_subprocess("-q")

    result.assert_outcomes(failed=1)
    result.stdout.fnmatch_lines(["*ValueError:*Missing mode*UTILITY*not published*"])


def test_discovery_timeout_terminates_child(pytester):
    pytester.makepyfile(robot_module="""
import threading
import wpilib

class TestRobot(wpilib.OpModeRobot):
    def __init__(self):
        threading.Event().wait()
""")
    pytester.makepyfile(test_discovery="""
import multiprocessing
import pathlib
import time

import pytest
import wpilib.testing._opmode as discovery
from robot_module import TestRobot as HangingRobot


def test_timeout(monkeypatch):
    monkeypatch.setattr(discovery, "_DISCOVERY_TIMEOUT", 1)
    children = multiprocessing.active_children()
    start = time.monotonic()
    with pytest.raises(pytest.UsageError, match="discovery timed out"):
        discovery.discover_opmodes(HangingRobot, pathlib.Path("robot_module.py").resolve())
    assert time.monotonic() - start < 5
    assert multiprocessing.active_children() == children
""")

    result = pytester.runpytest_subprocess("-q", timeout=40)

    result.assert_outcomes(passed=1)


@pytest.mark.parametrize("publish_later", [False, True])
def test_discovery_excludes_unpublished_and_later_modes(pytester, publish_later):
    pytester.makepyfile(robot_module=f"""
import wpilib
from hal import RobotMode

class TestRobot(wpilib.OpModeRobot):
    def __init__(self):
        super().__init__()
        self.add_opmode_factory(RobotMode.UTILITY, "Unpublished", self.make_mode)

    def make_mode(self):
        raise AssertionError("This mode should not be tested")

    def robot_periodic(self):
        if {publish_later!r}:
            self.publish_opmodes()
""")
    _configure(pytester, False)
    pytester.makepyfile(test_robot="from wpilib.testing.robot_tests import *")

    result = pytester.runpytest_subprocess("-q")

    result.assert_outcomes(passed=4, skipped=1)


def test_discovery_reports_abnormal_exit(pytester):
    pytester.makepyfile(robot_module="""
import os
import wpilib

class TestRobot(wpilib.OpModeRobot):
    def __init__(self):
        os._exit(17)
""")
    _configure(pytester, False)
    pytester.makepyfile(test_robot="from wpilib.testing.robot_tests import *")

    result = pytester.runpytest_subprocess("--collect-only", "-q", timeout=10)

    assert result.ret != pytest.ExitCode.OK
    result.stdout.fnmatch_lines(["*discovery exited without a result (exit code 17)*"])
