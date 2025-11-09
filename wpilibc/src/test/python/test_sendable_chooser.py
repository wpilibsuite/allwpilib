import pytest

from ntcore import NetworkTableInstance
from wpilib import SendableChooser, SmartDashboard


@pytest.fixture
def chooser() -> SendableChooser:
    chooser = SendableChooser()
    for i in range(1, 4):
        chooser.addOption(str(i), i)
    return chooser


@pytest.mark.parametrize("value", [0, 1, 2, 3])
def test_returns_selected(
    nt: NetworkTableInstance, chooser: SendableChooser, value: int
):
    chooser.setDefaultOption("0", 0)

    with nt.getStringTopic(
        "/SmartDashboard/ReturnsSelectedChooser/selected"
    ).publish() as pub:
        SmartDashboard.putData("ReturnsSelectedChooser", chooser)
        SmartDashboard.updateValues()
        print("set", value)
        pub.set(str(value))
        SmartDashboard.updateValues()
        print("get", chooser.getSelected())
        assert value == chooser.getSelected()


def test_default_is_returned_on_no_select(chooser: SendableChooser):
    chooser.setDefaultOption("4", 4)
    assert 4 == chooser.getSelected()


def test_default_constructable_is_returned_on_no_select_and_no_default(
    chooser: SendableChooser,
):
    assert chooser.getSelected() is None


def test_change_listener(nt: NetworkTableInstance, chooser: SendableChooser):
    current_val = [0]

    def on_change(val):
        current_val[0] = val

    chooser.onChange(on_change)
    SmartDashboard.putData("ChangeListenerChooser", chooser)
    SmartDashboard.updateValues()
    SmartDashboard.putString("ChangeListenerChooser/selected", "3")
    SmartDashboard.updateValues()
    assert 3 == current_val[0]
