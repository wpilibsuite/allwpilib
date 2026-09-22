"""Collection-time discovery shared by the in-process and isolated plugins."""

import inspect
import multiprocessing
import pathlib
import pickle
import threading
import traceback

import hal
import pytest
import wpilib
from wpilib.simulation import DriverStationSim

from . import OpMode

# Only picklable metadata crosses the process boundary (not HAL_OpModeOption).
OpModes = list[OpMode]
_DISCOVERY_TIMEOUT = 30


def _discover_opmodes(robot_class_data: bytes, robot_file: pathlib.Path, pipe):
    # Set this before importing user code, just as isolated test workers do.
    import robotpy.main
    from pytest_reraise import Reraise

    from .controller import RobotTestController
    from .pytest_plugin import _create_robot

    robotpy.main.robot_py_path = robot_file
    reraise = Reraise()
    try:
        robot_class = pickle.loads(robot_class_data)
        robot = _create_robot(robot_class)
        control = RobotTestController(reraise, robot)
        with control.run_robot():
            options = [
                OpMode(
                    hal.RobotMode(int(hal.opmode_get_robot_mode(option.id))),
                    option.name,
                )
                for option in DriverStationSim.get_opmode_options()
            ]
            pipe.send((options, None))
            # The parent terminates us after receiving the result. Keep the
            # robot alive to avoid native cleanup/interpreter shutdown deadlocks.
            threading.Event().wait()
    except BaseException:
        error = reraise.exception
        if error is not None:
            details = "".join(traceback.format_exception(error))
        else:
            details = traceback.format_exc()
        pipe.send((None, details))
        threading.Event().wait()


def discover_opmodes(
    robot_class: type[wpilib.RobotBase], robot_file: pathlib.Path
) -> OpModes:
    ctx = multiprocessing.get_context("spawn")
    parent, child = ctx.Pipe(duplex=False)
    process = ctx.Process(
        target=_discover_opmodes,
        args=(pickle.dumps(robot_class), robot_file, child),
    )
    process.start()
    child.close()
    try:
        if not parent.poll(_DISCOVERY_TIMEOUT):
            raise pytest.UsageError(
                f"Robot opmode discovery timed out after {_DISCOVERY_TIMEOUT} seconds"
            )
        try:
            options, error = parent.recv()
        except EOFError:
            process.join(timeout=1)
            raise pytest.UsageError(
                f"Robot opmode discovery exited without a result (exit code {process.exitcode})"
            ) from None
        if error is not None:
            raise pytest.UsageError(f"Robot opmode discovery failed:\n{error}")
        # Native options come from a hash map; give pytest stable, readable IDs.
        return sorted(
            options,
            key=lambda option: (int(option.mode), option.name),
        )
    finally:
        parent.close()
        if process.is_alive():
            process.kill()
        process.join(timeout=1)
        process.close()


class OpModeTestingPlugin:
    _robot_class: type[wpilib.RobotBase]
    _robot_file: pathlib.Path
    _opmodes: OpModes | None = None

    @pytest.hookimpl(trylast=True)
    def pytest_generate_tests(self, metafunc: pytest.Metafunc):
        if "opmode" not in metafunc.fixturenames:
            return
        # Pytest has no public API for resolving a fixture during collection.
        # Only parametrize our fixture, not a user's fixture or direct parameter
        # that happens to have the same name. Respect prior parametrization too.
        definitions = metafunc._arg2fixturedefs.get("opmode")
        if not definitions:
            return
        func = definitions[-1].func
        if getattr(func, "__func__", func) is not inspect.unwrap(
            OpModeTestingPlugin.opmode
        ):
            return
        if any("opmode" in call.params for call in metafunc._calls):
            return
        if self._opmodes is None:
            self._opmodes = discover_opmodes(self._robot_class, self._robot_file)
        metafunc.parametrize(
            "opmode",
            [
                pytest.param(opmode, id=f"{opmode.mode.name}-{opmode.name}")
                for opmode in self._opmodes
            ],
            indirect=True,
        )

    @pytest.fixture
    def opmode(self, request: pytest.FixtureRequest, robot) -> OpMode:
        """An :class:`wpilib.testing.OpMode`, parametrized over startup modes.

        Use with :meth:`RobotTestController.step_timing`'s ``opmode`` argument.
        Each parameter gets a fresh robot, and robots without published modes
        skip tests using this fixture. Discovery starts a separate robot process
        during collection; opmodes registered later during operation are not
        included.
        """
        return request.param
