import pytest

from wpilib import DriverStationBackend, Joystick
from wpilib.simulation import DriverStationSim, stepTiming


@pytest.mark.parametrize(
    "axes_max, buttons_max, povs_max, expected",
    [
        (0, 0, 0, False),
        (1, 0, 0, True),
        (0, 1, 0, True),
        (0, 0, 1, True),
        (1, 1, 1, True),
        (4, 10, 1, True),
    ],
)
def test_is_joystick_connected(wpilib_state, axes_max, buttons_max, povs_max, expected):
    DriverStationSim.setJoystickAxesMaximumIndex(1, axes_max)
    DriverStationSim.setJoystickButtonsMaximumIndex(1, buttons_max)
    DriverStationSim.setJoystickPOVsMaximumIndex(1, povs_max)
    DriverStationSim.notifyNewData()

    assert DriverStationBackend.isJoystickConnected(1) == expected


@pytest.mark.parametrize(
    "fms_attached, silenced, expected_silenced, expected_warning", 
    [
        (False, True, True, ""),
        (False, False, False, "Warning: Joystick on port 0 not available, check if all controllers are plugged in"),
        (True, True, False, "Warning: Joystick on port 0 not available, check if all controllers are plugged in"), # FMS overrides silence
        (True, False, False, "Warning: Joystick on port 0 not available, check if all controllers are plugged in"),
    ],
)
def test_joystick_connection_warnings(
    wpilib_state, fms_attached, silenced, expected_silenced, expected_warning, capsys
):
    DriverStationSim.setFmsAttached(fms_attached)
    DriverStationSim.notifyNewData()
    DriverStationBackend.silenceJoystickConnectionWarning(silenced)

    joystick = Joystick(0)
    joystick.getRawButton(1)

    stepTiming(1.0)
    assert (
        DriverStationBackend.isJoystickConnectionWarningSilenced() == expected_silenced
    )
    
    # Capture stderr to check for warnings
    captured = capsys.readouterr()
    assert expected_warning in captured.err