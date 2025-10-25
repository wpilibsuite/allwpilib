import pytest
import re
import weakref

import wpilib


def test_sendable_chooser():
    chooser = wpilib.SendableChooser()
    assert chooser.getSelected() is None

    chooser.setDefaultOption("option", True)
    assert chooser.getSelected() is True


def test_smart_dashboard_putdata():
    t = wpilib.Talon(4)
    ref = weakref.ref(t)
    wpilib.SmartDashboard.putData("talon", t)
    del t
    assert bool(ref) is True
    assert wpilib.SmartDashboard.getData("talon") is ref()


def test_motorcontrollergroup():
    t1 = wpilib.Talon(7)
    t2 = wpilib.Talon(8)
    g = wpilib.MotorControllerGroup(t1, t2)

    g.set(1)
    assert t1.get() == pytest.approx(1)
    assert t2.get() == pytest.approx(1)

    g.set(-1)
    assert t1.get() == pytest.approx(-1)
    assert t2.get() == pytest.approx(-1)


def test_motorcontrollergroup_error():
    with pytest.raises(
        TypeError, match=re.escape("Argument 1 must be a MotorController (got '1')")
    ):
        wpilib.MotorControllerGroup(1)
