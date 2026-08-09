import pytest
import wpilib
import wpilib_drivers


def test_a301_high_level_api():
    a301 = wpilib_drivers.A301(wpilib.CANBusMap.CAN_S0, 31)

    assert a301.get_bus_id() == 0
    assert a301.get_device_id() == 31

    a301.set_throttle(0.25)
    assert a301.get_throttle() == pytest.approx(0.25)
    a301.set_voltage(6.0)
    assert a301.get_throttle() == pytest.approx(0.5)

    assert a301.set_velocity(125.0) == wpilib_drivers.A301Error.OK
    assert a301.set_relative_position(3.0) == wpilib_drivers.A301Error.OK
    assert (
        a301.set_relative_position_with_speed(3.0, 50.0)
        == wpilib_drivers.A301Error.OK
    )
    assert a301.set_absolute_position(0.25) == wpilib_drivers.A301Error.OK
    assert (
        a301.set_absolute_position_with_speed(0.25, 50.0)
        == wpilib_drivers.A301Error.OK
    )
    assert a301.set_current(10.0) == wpilib_drivers.A301Error.OK
    assert a301.set_relative_encoder_position(2.0) == wpilib_drivers.A301Error.OK
    assert a301.set_absolute_encoder_position(0.1) == wpilib_drivers.A301Error.OK
    assert (
        a301.set_idle_mode(wpilib_drivers.A301.IdleMode.BRAKE)
        == wpilib_drivers.A301Error.OK
    )
    assert (
        a301.enable_absolute_position_continuous_input()
        == wpilib_drivers.A301Error.OK
    )
    assert (
        a301.disable_absolute_position_continuous_input()
        == wpilib_drivers.A301Error.OK
    )
    assert a301.clear_faults() == wpilib_drivers.A301Error.OK

    a301.set_inverted(True)
    assert a301.get_inverted()
    assert (
        a301.set_absolute_encoder_range_offset(0.25)
        == wpilib_drivers.A301Error.OK
    )
    assert (
        a301.set_absolute_encoder_range_offset(0.75)
        == wpilib_drivers.A301Error.PARAM_INVALID
    )

    voltage = a301.get_bus_voltage()
    assert not voltage.is_valid()
    assert voltage.get_error() == wpilib_drivers.A301Error.TIMEOUT
    assert voltage.get(-1.0) == -1.0

    a301.disable()
    assert a301.get_throttle() == pytest.approx(0.0)
