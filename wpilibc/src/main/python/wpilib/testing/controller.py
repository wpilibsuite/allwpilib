import contextlib
import time
import threading
import typing as T

import pytest

from .. import RobotBase
from ..simulation import (
    DriverStationSim,
    stepTiming,
    stepTimingAsync,
    getProgramStarted,
)
from hal._wpiHal import _RobotMode as RobotMode


class RobotTestController:
    """
    Use this object to control the robot's state during tests
    """

    def __init__(self, reraise, robot: RobotBase):
        self._reraise = reraise

        self._thread: T.Optional[threading.Thread] = None
        self._robot = robot

        self._cond = threading.Condition()
        self._robot_started = False
        self._robot_finished = False

    def _robot_thread(self, robot: RobotBase):
        with self._cond:
            self._robot_started = True
            self._cond.notify_all()

        with self._reraise(catch=True):
            assert robot is not None  # shouldn't happen...

            try:
                robot.startCompetition()
                assert self._robot_finished
            finally:
                # always call endCompetition or python hangs
                robot.endCompetition()
                del robot

    @contextlib.contextmanager
    def run_robot(self):
        """
        Use this in a "with" statement to start your robot code at the
        beginning of the with block, and end your robot code at the end
        of the with block.
        """

        # remove robot reference so it gets cleaned up when gc.collect() is called
        robot = self._robot
        assert robot is not None
        self._robot = None

        self._thread = th = threading.Thread(
            target=self._robot_thread, args=(robot,), daemon=True
        )
        th.start()

        with self._cond:
            # make sure the thread didn't die
            assert self._cond.wait_for(lambda: self._robot_started, timeout=1)

        # This is the same thing that waitForProgramStart does
        for _ in range(1000):
            if getProgramStarted():
                break
            time.sleep(0.001)
        else:
            assert False, "robot never started"

        try:
            # in this block you should tell the sim to do sim things
            yield
        finally:
            self._robot_finished = True
            robot.endCompetition()

            if isinstance(self._reraise.exception, RuntimeError):
                if str(self._reraise.exception).startswith(
                    "HAL: A handle parameter was passed incorrectly"
                ):
                    msg = (
                        "Do not reuse HAL objects in tests! This error may occur if you"
                        " stored a motor/sensor as a global or as a class variable"
                        " outside of a method."
                    )
                    if hasattr(Exception, "add_note"):
                        self._reraise.exception.add_note(f"*** {msg}")
                    else:
                        e = self._reraise.exception
                        self._reraise.reset()
                        raise RuntimeError(msg) from e

        # Increment time by 1 second to ensure that any notifiers fire
        stepTimingAsync(1.0)

        # the robot thread should exit quickly
        th.join(timeout=1)
        if th.is_alive():
            pytest.fail("robot did not exit within 2 seconds")

        self._robot = None
        self._thread = None

    @property
    def robot_is_alive(self) -> bool:
        """
        True if the robot code is alive
        """
        th = self._thread
        if not th:
            return False

        return th.is_alive()

    def step_timing(
        self,
        *,
        seconds: float,
        autonomous: bool,
        enabled: bool,
        assert_alive: bool = True,
    ) -> float:
        """
        This utility will increment simulated time, while pretending that
        there's a driver station connected and delivering new packets
        every 0.2 seconds.

        :param seconds:    Number of seconds to run (will step in increments of 0.2)
        :param autonomous: Tell the robot that it is in autonomous mode
        :param enabled:    Tell the robot that it is enabled

        :returns: Number of seconds time was incremented
        """

        assert self.robot_is_alive, "did you call control.run_robot()?"

        assert seconds > 0

        DriverStationSim.setDsAttached(True)
        DriverStationSim.setRobotMode(
            RobotMode.AUTONOMOUS if autonomous else RobotMode.TELEOPERATED
        )
        DriverStationSim.setEnabled(enabled)

        tm = 0.0

        while tm < seconds + 0.01:
            DriverStationSim.notifyNewData()
            stepTiming(0.2)
            if assert_alive:
                assert self.robot_is_alive
            tm += 0.2

        return tm
