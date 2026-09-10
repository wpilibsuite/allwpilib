import pathlib


def _make_robot_module(pytester):
    pytester.makepyfile(robot_module="""
import wpilib


class DummyRobot(wpilib.TimedRobot):
    def __init__(self):
        super().__init__()
        self.did_init = True


class AutonomousPeriodicFailed(wpilib.TimedRobot):
    def autonomous_periodic(self):
        assert False


class TeleopPeriodicFailed(wpilib.TimedRobot):
    def teleop_periodic(self):
        assert False


class TeleopInitFailed(wpilib.TimedRobot):
    def teleop_init(self):
        assert False


class IterativeStateRobot(wpilib.TimedRobot):
    def disabled_init(self):
        self.did_disabled_init = True

    def disabled_periodic(self):
        self.did_disabled_periodic = True

    def autonomous_init(self):
        self.did_auto_init = True

    def autonomous_periodic(self):
        self.did_auto_periodic = True

    def teleop_init(self):
        self.did_teleop_init = True

    def teleop_periodic(self):
        self.did_teleop_periodic = True
""")


def _configure_robot_testing_plugin(pytester, robot_class="DummyRobot"):
    pytester.makeconftest(f"""
import pathlib

from wpilib.testing.pytest_plugin import RobotTestingPlugin

from robot_module import {robot_class}


def pytest_configure(config):
    robot_file = pathlib.Path(__file__).resolve()
    config.pluginmanager.register(RobotTestingPlugin({robot_class}, robot_file, False))
""")


def _configure_isolated_plugin(
    pytester,
    parallelism=1,
    robot_class="DummyRobot",
    robot_module="robot_module",
    robot_file_name=None,
):
    if robot_file_name is None:
        robot_file = "pathlib.Path(__file__).resolve()"
    else:
        robot_file = f"pathlib.Path(__file__).parent / {robot_file_name!r}"

    pytester.makeconftest(f"""
import pathlib

from wpilib.testing.pytest_isolated_tests_plugin import IsolatedTestsPlugin

from {robot_module} import {robot_class}

def pytest_configure(config):
    if "--no-header" in config.invocation_params.args:
        return
    robot_file = {robot_file}
    config.pluginmanager.register(
        IsolatedTestsPlugin({robot_class}, robot_file, False, False, {parallelism})
    )
""")
