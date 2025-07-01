import pytest
import re
import weakref

import wpilib


def test_selectable():
    chooser = wpilib.Selectable()
    assert chooser.get_selected() is None

    chooser.add_default("option", True)
    assert chooser.get_selected() is True


def test_tunables_publish_retains_value(nt):
    value = wpilib.Selectable()
    value.add_default("option", True)
    ref = weakref.ref(value)
    wpilib.Tunables.publish("selectable", value)
    del value
    assert bool(ref) is True
    wpilib.Tunables.remove("selectable")


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
