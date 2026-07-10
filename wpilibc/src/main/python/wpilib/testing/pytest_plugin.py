import gc
import pathlib
import typing as T
import weakref

import pytest

from hal import RobotMode
import hal
import hal.simulation
import ntcore
import wpilib
from wpilib.simulation import DriverStationSim, pause_timing, restart_timing
import wpilib.simulation

# TODO: get rid of special-casing.. maybe should register a HAL shutdown hook or something
try:
    import commands2
    from commands2.button.commandgenerichid import _reset_command_generic_hid_data
except ImportError:
    commands2 = None
    _reset_command_generic_hid_data = None

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

        nt_inst = ntcore.NetworkTableInstance.get_default()
        nt_inst.start_local()

        pause_timing()
        restart_timing()

        wpilib.DriverStationBackend.silence_joystick_connection_warning(True)
        DriverStationSim.set_robot_mode(RobotMode.AUTONOMOUS)
        DriverStationSim.set_enabled(False)
        DriverStationSim.notify_new_data()

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
        wpilib.simulation._simulation._reset_motor_safety()

        del robot

        if commands2 is not None:
            commands2.CommandScheduler.reset_instance()
            _reset_command_generic_hid_data()

        # Double-check all objects are destroyed so that HAL handles are released
        gc.collect()

        # Reset DriverStation cached HID wrapper objects after user and command
        # references have had a chance to be released.
        wpilib.DriverStationBackend.reset_cached_hid_data()

        # shutdown networktables before other kinds of global cleanup
        # -> some reset functions will re-register listeners, so it's important
        #    to do this before so that the listeners are active on the current
        #    NetworkTables instance
        nt_inst.stop_local()
        nt_inst._reset()

        # Cleanup WPILib globals
        # -> preferences, SmartDashboard, MotorSafety
        wpilib.simulation._simulation._reset_wpilib_simulation_data()
        wpilib._wpilib._clear_smart_dashboard_data()

        # Cancel all periodic callbacks
        hal.simulation.cancel_all_sim_periodic_callbacks()

        # Reset the HAL handles
        hal.simulation.reset_global_handles()

        # Reset the HAL data
        hal.simulation.reset_all_sim_data()

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
