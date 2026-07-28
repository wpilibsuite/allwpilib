import pytest

from wpilib import DriverStationDisplay, Gamepad, DSGamepadChooser
from wpilib.simulation import DriverStationSim, GamepadSim


@pytest.fixture(autouse=True)
def reset_driver_station_sim():
    DriverStationSim.reset_data()
    DriverStationDisplay.set_mode(DriverStationDisplay.Mode.LINE)


def tap(sim: GamepadSim, chooser: DSGamepadChooser, button: Gamepad.Button) -> None:
    sim.set_button(button, True)
    sim.notify_new_data()
    chooser.update()

    sim.set_button(button, False)
    sim.notify_new_data()
    chooser.update()


def test_add_options_selects_first_option_by_default() -> None:
    chooser = DSGamepadChooser(0)

    selectable = chooser.add_options("Auto", ["Left", "Center", "Right"])

    assert selectable.get_selected() == "Left"
    assert chooser.get_selected("Auto") == "Left"
    assert chooser.get_selected_index("Auto") == 0
    assert chooser.get_selectable_names() == ["Auto"]
    assert chooser.get_selected_selectable().get_name() == selectable.get_name()


def test_add_integer_options_creates_range() -> None:
    chooser = DSGamepadChooser(0)

    selectable = chooser.add_integer_options("Delay", -2, 3, 2)

    assert selectable.get_options() == ["-2", "0", "2", "3"]
    assert selectable.get_selected() == "-2"
    assert chooser.get_selected_integer("Delay") == -2


def test_add_double_options_creates_range() -> None:
    chooser = DSGamepadChooser(0)

    selectable = chooser.add_double_options("Speed", 0.0, 1.0, 0.3)

    assert selectable.get_options() == ["0.0", "0.3", "0.6", "0.9", "1.0"]
    assert selectable.get_selected() == "0.0"
    assert chooser.get_selected_double("Speed") == pytest.approx(0.0)


def test_dpad_moves_between_selectables_and_options() -> None:
    gamepad = Gamepad(0)
    sim = GamepadSim(gamepad)
    chooser = DSGamepadChooser(gamepad)
    auto = chooser.add_options("Auto", ["Left", "Center", "Right"])
    delay = chooser.add_integer_options("Delay", 0, 2, 1)

    tap(sim, chooser, Gamepad.Button.DPAD_RIGHT)
    assert auto.get_selected() == "Center"
    assert delay.get_selected() == "0"
    assert chooser.get_selected_selectable().get_name() == "Auto"

    tap(sim, chooser, Gamepad.Button.DPAD_DOWN)
    assert chooser.get_selected_selectable().get_name() == "Delay"

    tap(sim, chooser, Gamepad.Button.DPAD_RIGHT)
    assert delay.get_selected() == "1"
    assert auto.get_selected() == "Center"

    tap(sim, chooser, Gamepad.Button.DPAD_LEFT)
    tap(sim, chooser, Gamepad.Button.DPAD_LEFT)
    assert delay.get_selected() == "2"

    tap(sim, chooser, Gamepad.Button.DPAD_UP)
    assert chooser.get_selected_selectable().get_name() == "Auto"

    tap(sim, chooser, Gamepad.Button.DPAD_LEFT)
    tap(sim, chooser, Gamepad.Button.DPAD_LEFT)
    assert auto.get_selected() == "Right"


def test_rejects_invalid_selectables() -> None:
    chooser = DSGamepadChooser(0)

    with pytest.raises(ValueError):
        chooser.add_options("", ["A"])
    with pytest.raises(ValueError):
        chooser.add_options("Empty", [])
    with pytest.raises(ValueError):
        chooser.add_integer_options("Bad", 0, 2, 0)
    with pytest.raises(ValueError):
        chooser.add_double_options("Bad", 0.0, 1.0, 0.0)

    chooser.add_options("Auto", ["A"])
    with pytest.raises(ValueError):
        chooser.add_options("Auto", ["B"])
    with pytest.raises(ValueError):
        chooser.get_selected("Missing")
