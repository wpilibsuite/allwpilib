import pytest

import hal.simulation as halsim
from wpiutil import Alert
from wpilib import DriverStationBackend, Joystick
from wpilib.simulation import AlertSim, DriverStationSim, step_timing


def is_driver_station_alert_active(alert_id: str, text: str, level: Alert.Level):
    return any(
        alert.group == "DriverStation"
        and alert.id == alert_id
        and alert.text == text
        and alert.level == level
        and alert.is_active()
        for alert in AlertSim.get_all()
    )


def is_joystick_disconnected_alert_active(stick: int):
    text = (
        f"Joystick on port {stick} not available, "
        "check if all controllers are plugged in"
    )
    return is_driver_station_alert_active(
        f"joystick{stick}Disconnected", text, Alert.Level.HIGH
    )


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
    DriverStationSim.set_joystick_axes_maximum_index(1, axes_max)
    DriverStationSim.set_joystick_buttons_maximum_index(1, buttons_max)
    DriverStationSim.set_joystick_povs_maximum_index(1, povs_max)
    DriverStationSim.notify_new_data()

    assert DriverStationBackend.is_joystick_connected(1) == expected


@pytest.mark.parametrize(
    "fms_attached, silenced, expected_silenced, expected_alert_active",
    [
        (False, True, True, False),
        (False, False, False, True),
        (True, True, False, True),  # FMS overrides silence
        (True, False, False, True),
    ],
)
def test_joystick_connection_alerts(
    wpilib_state,
    fms_attached,
    silenced,
    expected_silenced,
    expected_alert_active,
    capsys,
):
    DriverStationSim.set_joystick_buttons_available(0, 0)
    DriverStationSim.set_joystick_axes_available(0, 0)
    DriverStationSim.set_joystick_povs_available(0, 0)
    DriverStationSim.set_fms_attached(fms_attached)
    DriverStationSim.notify_new_data()
    DriverStationBackend.silence_joystick_connection_alert(silenced)

    joystick = Joystick(0)
    joystick.get_raw_button(1)

    step_timing(1.0)
    assert (
        DriverStationBackend.is_joystick_connection_alert_silenced()
        == expected_silenced
    )
    assert is_joystick_disconnected_alert_active(0) == expected_alert_active

    captured = capsys.readouterr()
    assert "Joystick on port 0 not available" not in captured.err


def test_joystick_resource_alerts(wpilib_state, capsys):
    DriverStationSim.set_joystick_buttons_available(0, 1)
    DriverStationSim.set_joystick_axes_available(0, 1)
    DriverStationSim.set_joystick_povs_available(0, 1)
    halsim.set_joystick_touchpad_counts(0, 1, 1)
    DriverStationSim.notify_new_data()

    DriverStationBackend.get_stick_button(0, 1)
    DriverStationBackend.get_stick_button(0, 2)
    DriverStationBackend.get_stick_axis(0, 1)
    DriverStationBackend.get_stick_pov(0, 1)
    DriverStationBackend.get_stick_touchpad_finger(0, 0, 1)

    assert is_driver_station_alert_active(
        "joystick0ButtonUnavailable",
        "Joystick Button 2 on port 0 not available",
        Alert.Level.MEDIUM,
    )
    assert is_driver_station_alert_active(
        "joystick0AxisUnavailable",
        "Joystick axis 1 on port 0 not available",
        Alert.Level.MEDIUM,
    )
    assert is_driver_station_alert_active(
        "joystick0POVUnavailable",
        "Joystick POV 1 on port 0 not available",
        Alert.Level.MEDIUM,
    )
    assert is_driver_station_alert_active(
        "joystick0TouchpadFingerUnavailable",
        "Joystick touchpad finger 1 on touchpad 0 on port 0 not available",
        Alert.Level.MEDIUM,
    )
    assert not is_joystick_disconnected_alert_active(0)

    captured = capsys.readouterr()
    assert "not available" not in captured.err
