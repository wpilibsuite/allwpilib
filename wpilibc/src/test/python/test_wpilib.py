import pytest
import re

import telemetry
import tunable
import wpilib


def test_telemetry_tunable_flat_namespace():
    assert wpilib.Telemetry is telemetry.Telemetry
    assert wpilib.TelemetryRegistry is telemetry.TelemetryRegistry
    assert wpilib.Tunables is tunable.Tunables
    assert wpilib.Selectable is tunable.Selectable


def test_motorcontrollergroup():
    t1 = wpilib.Talon(7)
    t2 = wpilib.Talon(8)
    g = wpilib.MotorControllerGroup(t1, t2)

    g.set_throttle(1)
    assert t1.get_throttle() == pytest.approx(1)
    assert t2.get_throttle() == pytest.approx(1)

    g.set_throttle(-1)
    assert t1.get_throttle() == pytest.approx(-1)
    assert t2.get_throttle() == pytest.approx(-1)


def test_motorcontrollergroup_error():
    with pytest.raises(
        TypeError, match=re.escape("Argument 1 must be a MotorController (got '1')")
    ):
        wpilib.MotorControllerGroup(1)
