import pytest

from ntcore import NetworkTableInstance
from wpilib import SendableChooser, SmartDashboard


@pytest.fixture
def chooser() -> SendableChooser:
    chooser = SendableChooser()
    for i in range(1, 4):
        chooser.add_option(str(i), i)
    return chooser


@pytest.mark.parametrize("value", [0, 1, 2, 3])
def test_returns_selected(
    nt: NetworkTableInstance, chooser: SendableChooser, value: int
):
    chooser.set_default_option("0", 0)

    with nt.get_string_topic(
        "/SmartDashboard/ReturnsSelectedChooser/selected"
    ).publish() as pub:
        SmartDashboard.put_data("ReturnsSelectedChooser", chooser)
        SmartDashboard.update_values()
        print("set", value)
        pub.set(str(value))
        SmartDashboard.update_values()
        print("get", chooser.get_selected())
        assert value == chooser.get_selected()


def test_default_is_returned_on_no_select(chooser: SendableChooser):
    chooser.set_default_option("4", 4)
    assert 4 == chooser.get_selected()


def test_default_constructable_is_returned_on_no_select_and_no_default(
    chooser: SendableChooser,
):
    assert chooser.get_selected() is None


def test_change_listener(nt: NetworkTableInstance, chooser: SendableChooser):
    current_val = [0]

    def on_change(val):
        current_val[0] = val

    chooser.on_change(on_change)
    SmartDashboard.put_data("ChangeListenerChooser", chooser)
    SmartDashboard.update_values()
    SmartDashboard.put_string("ChangeListenerChooser/selected", "3")
    SmartDashboard.update_values()
    assert 3 == current_val[0]
