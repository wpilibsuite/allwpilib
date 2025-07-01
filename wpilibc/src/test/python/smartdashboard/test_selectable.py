"""Tests for :class:`wpilib.Selectable`."""

import time

import pytest

from ntcore import NetworkTableInstance
from wpilib import Selectable, TunableRegistry, Tunables


@pytest.fixture
def chooser() -> Selectable[int]:
    chooser = Selectable()
    for i in range(1, 4):
        chooser.add(str(i), i)
    return chooser


@pytest.mark.parametrize("value", [0, 1, 2, 3])
def test_returns_selected(
    nt: NetworkTableInstance, chooser: Selectable[int], value: int
):
    chooser.add_default("0", 0)
    name = f"ReturnsSelectedChooser{value}"

    with nt.get_string_topic(f"/Tunables/{name}/selected/tune").publish() as pub:
        Tunables.publish(name, chooser)
        print("set", value)
        pub.set(str(value))
        nt.flush()
        for _ in range(10):
            TunableRegistry.update()
            if chooser.get_selected() == value:
                break
            time.sleep(0.01)
        print("get", chooser.get_selected())
        assert value == chooser.get_selected()
        Tunables.remove(name)


def test_default_is_returned_on_no_select(chooser: Selectable[int]):
    chooser.add_default("4", 4)
    assert 4 == chooser.get_selected()


def test_default_constructable_is_returned_on_no_select_and_no_default(
    chooser: Selectable[int],
):
    assert chooser.get_selected() is None


def test_change_listener(nt: NetworkTableInstance, chooser: Selectable[int]):
    current_val = [0]

    def on_change(val):
        current_val[0] = val

    chooser.on_change(on_change)
    Tunables.publish("ChangeListenerChooser", chooser)
    nt.get_entry("/Tunables/ChangeListenerChooser/selected/tune").set_string("3")
    nt.flush()
    for _ in range(10):
        TunableRegistry.update()
        if current_val[0] == 3:
            break
        time.sleep(0.01)
    assert 3 == current_val[0]
    Tunables.remove("ChangeListenerChooser")
