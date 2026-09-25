import pytest
import wpilib
from wpilib_drivers import RevColorSensorV2


def test_rev_color_sensor_v2_api():
    assert RevColorSensorV2.DEFAULT_ADDRESS == 0x39

    with pytest.raises(ValueError):
        RevColorSensorV2(wpilib.I2C.Port.PORT_0, -1)
    with pytest.raises(ValueError):
        RevColorSensorV2(wpilib.I2C.Port.PORT_0, 0x80)

    sensor = RevColorSensorV2(wpilib.I2C.Port.PORT_0)
    assert sensor.get_port() == wpilib.I2C.Port.PORT_0
    assert sensor.get_device_address() == RevColorSensorV2.DEFAULT_ADDRESS
    assert sensor.get_failure_count() >= 0
    assert sensor.get_maximum_raw_proximity_value() == 1023

    # Nothing has been read from a device, so the cached measurements are zero.
    assert sensor.get_raw_clear() == 0
    assert sensor.get_raw_red() == 0
    assert sensor.get_raw_green() == 0
    assert sensor.get_raw_blue() == 0
    assert sensor.get_raw_proximity() == 0
    assert sensor.get_proximity() == 0.0

    color = sensor.get_color()
    assert color.red == 0.0
    assert color.green == 0.0
    assert color.blue == 0.0


def test_rev_color_sensor_v2_configuration():
    sensor = RevColorSensorV2(wpilib.I2C.Port.PORT_0)

    sensor.set_gain(RevColorSensorV2.Gain.GAIN_16)
    assert sensor.get_gain() == RevColorSensorV2.Gain.GAIN_16

    sensor.set_led_drive(RevColorSensorV2.LedDrive.PERCENT_25)
    assert sensor.get_led_drive() == RevColorSensorV2.LedDrive.PERCENT_25

    sensor.set_proximity_pulse_count(16)
    assert sensor.get_proximity_pulse_count() == 16

    sensor.set_software_gain(2.0)
    assert sensor.get_software_gain() == pytest.approx(2.0)

    # The integration time rounds up to a whole 2.4 ms cycle.
    sensor.set_integration_time(0.1)
    assert sensor.get_integration_time() == pytest.approx(0.1008)
    assert sensor.get_maximum_raw_color_value() == 42 * 1024

    sensor.set_distance_calibration(186.347, 30403.5, 0.576649)


def test_rev_color_sensor_v2_rejects_invalid_configuration():
    sensor = RevColorSensorV2(wpilib.I2C.Port.PORT_0)

    with pytest.raises(ValueError):
        sensor.set_integration_time(0.0)
    with pytest.raises(ValueError):
        sensor.set_proximity_pulse_count(0)
    with pytest.raises(ValueError):
        sensor.set_proximity_pulse_count(256)
    with pytest.raises(ValueError):
        sensor.set_software_gain(0.0)
