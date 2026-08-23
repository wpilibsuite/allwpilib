import math

import pytest
import wpilib
from wpilib_drivers import GoBildaPinpoint


def test_gobilda_pinpoint_api():
    assert GoBildaPinpoint.DEFAULT_ADDRESS == 0x31

    with pytest.raises(ValueError):
        GoBildaPinpoint(wpilib.I2C.Port.PORT_0, -1)
    with pytest.raises(ValueError):
        GoBildaPinpoint(wpilib.I2C.Port.PORT_0, 0x80)

    pinpoint = GoBildaPinpoint(wpilib.I2C.Port.PORT_0)
    assert pinpoint.get_port() == wpilib.I2C.Port.PORT_0
    assert pinpoint.get_device_address() == GoBildaPinpoint.DEFAULT_ADDRESS
    assert pinpoint.get_device_status() == GoBildaPinpoint.DeviceStatus.NOT_READY
    assert pinpoint.get_last_failed_register() is None
    assert pinpoint.get_last_failure_reason() is None
    assert pinpoint.get_failure_count() == 0

    pinpoint.set_error_detection_type(GoBildaPinpoint.ErrorDetectionType.NONE)
    pinpoint.set_offsets(0.1, -0.2)
    pinpoint.set_encoder_resolution(GoBildaPinpoint.OdometryPod.SWINGARM)
    pinpoint.set_encoder_resolution(12345.0)
    pinpoint.set_yaw_scalar(1.001)
    pinpoint.set_x_position(1.0)
    pinpoint.set_y_position(-2.0)
    pinpoint.set_heading(math.pi)


def test_gobilda_pinpoint_rejects_invalid_configuration():
    pinpoint = GoBildaPinpoint(wpilib.I2C.Port.PORT_0)

    with pytest.raises(ValueError):
        pinpoint.set_encoder_resolution(0.0)
    with pytest.raises(ValueError):
        pinpoint.set_yaw_scalar(math.inf)
