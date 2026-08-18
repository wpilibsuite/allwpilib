import pytest
import re
import wpilib


def test_sendable_chooser():
    chooser = wpilib.SendableChooser()
    assert chooser.get_selected() is None

    chooser.set_default_option("option", True)
    assert chooser.get_selected() is True


def test_motorcontrollergroup_error():
    with pytest.raises(
        TypeError, match=re.escape("Argument 1 must be a MotorController (got '1')")
    ):
        wpilib.MotorControllerGroup(1)
