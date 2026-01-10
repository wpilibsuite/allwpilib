import gc
import pathlib
import typing as T
import weakref

import pytest

import hal
import hal.simulation
from hal._wpiHal import _RobotMode as RobotMode
import ntcore
import wpilib
from wpilib.simulation import DriverStationSim, pauseTiming, restartTiming
import wpilib.simulation

# TODO: get rid of special-casing.. maybe should register a HAL shutdown hook or something
try:
    import commands2
except ImportError:
    commands2 = None

from .controller import RobotTestController


class RobotTestingPlugin:
    """
    Pytest plugin. Each documented member function name can be an argument
    to your test functions, and the data that these functions return will
    be passed to your test function.
    """

    def __init__(
        self,
        robot_class: T.Type[wpilib.RobotBase],
        robot_file: pathlib.Path,
        isolated: bool,
    ):
        self.isolated = isolated
        self._robot_file = robot_file
        self._robot_class = robot_class

    #
    # Fixtures
    #
    # Each one of these can be arguments to your test, and the result of the
    # corresponding function will be passed to your test as that argument.
    #

    @pytest.fixture(scope="function")
    def robot(self):
        """
        Your robot instance

        .. note:: RobotPy/WPILib testing infrastructure is really sensitive
                  to ensuring that things get cleaned up properly. Make sure
                  that you don't store references to your robot or other
                  WPILib objects in a global or static context.
        """

        #
        # This function needs to do the same things that RobotBase.main does
        # plus some extra things needed for testing
        #
        # Previously this was separate from robot fixture, but we need to
        # ensure that the robot cleanup happens deterministically relative to
        # when handle cleanup/etc happens, otherwise unnecessary HAL errors will
        # bubble up to the user
        #

        nt_inst = ntcore.NetworkTableInstance.getDefault()
        nt_inst.startLocal()

        pauseTiming()
        restartTiming()

        wpilib.DriverStation.silenceJoystickConnectionWarning(True)
        DriverStationSim.setRobotMode(RobotMode.AUTONOMOUS)
        DriverStationSim.setEnabled(False)
        DriverStationSim.notifyNewData()

        # Create the user's robot instance
        robot = self._robot_class()

        # Tests only get a proxy to ensure cleanup is more reliable
        yield weakref.proxy(robot)

        # If running in separate processes, no need to do cleanup
        if self.isolated:
            # .. and funny enough, in isolated mode we *don't* want the
            # robot to be cleaned up, as that can deadlock
            self._saved_robot = robot
            return

        # HACK: avoid motor safety deadlock
        wpilib.simulation._simulation._resetMotorSafety()

        del robot

        if commands2 is not None:
            commands2.CommandScheduler.resetInstance()

        # Double-check all objects are destroyed so that HAL handles are released
        gc.collect()

        # shutdown networktables before other kinds of global cleanup
        # -> some reset functions will re-register listeners, so it's important
        #    to do this before so that the listeners are active on the current
        #    NetworkTables instance
        nt_inst.stopLocal()
        nt_inst._reset()

        # Cleanup WPILib globals
        # -> preferences, SmartDashboard, MotorSafety
        wpilib.simulation._simulation._resetWpilibSimulationData()
        wpilib._wpilib._clearSmartDashboardData()

        # Cancel all periodic callbacks
        hal.simulation.cancelAllSimPeriodicCallbacks()

        # Reset the HAL handles
        hal.simulation.resetGlobalHandles()

        # Reset the HAL data
        hal.simulation.resetAllSimData()

        # Don't call HAL shutdown! This is only used to cleanup HAL extensions,
        # and functions will only be called the first time (unless re-registered)
        # hal.shutdown()

    @pytest.fixture(scope="function")
    def control(self, reraise, robot: wpilib.RobotBase) -> RobotTestController:
        """
        A pytest fixture that provides control over your robot
        """
        return RobotTestController(reraise, robot)

    @pytest.fixture()
    def robot_file(self) -> pathlib.Path:
        """The absolute filename your robot code is started from"""
        return self._robot_file

    @pytest.fixture()
    def robot_path(self) -> pathlib.Path:
        """The absolute directory that your robot code is located at"""
        return self._robot_file.parent
