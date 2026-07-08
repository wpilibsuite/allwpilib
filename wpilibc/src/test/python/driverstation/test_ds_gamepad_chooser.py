import pytest

from wpilib import DriverStationDisplay, Gamepad, DSGamepadChooser
from wpilib.simulation import DriverStationSim, GamepadSim


@pytest.fixture(autouse=True)
def reset_driver_station_sim():
    DriverStationSim.resetData()
    DriverStationDisplay.setMode(DriverStationDisplay.Mode.Line)


def tap(sim: GamepadSim, chooser: DSGamepadChooser, button: Gamepad.Button) -> None:
    sim.setButton(button, True)
    sim.notifyNewData()
    chooser.update()

    sim.setButton(button, False)
    sim.notifyNewData()
    chooser.update()


def test_add_options_selects_first_option_by_default() -> None:
    chooser = DSGamepadChooser(0)

    selectable = chooser.addOptions("Auto", ["Left", "Center", "Right"])

    assert selectable.getSelected() == "Left"
    assert chooser.getSelected("Auto") == "Left"
    assert chooser.getSelectedIndex("Auto") == 0
    assert chooser.getSelectableNames() == ["Auto"]
    assert chooser.getSelectedSelectable().getName() == selectable.getName()


def test_add_integer_options_creates_range() -> None:
    chooser = DSGamepadChooser(0)

    selectable = chooser.addIntegerOptions("Delay", -2, 3, 2)

    assert selectable.getOptions() == ["-2", "0", "2", "3"]
    assert selectable.getSelected() == "-2"
    assert chooser.getSelectedInteger("Delay") == -2


def test_add_double_options_creates_range() -> None:
    chooser = DSGamepadChooser(0)

    selectable = chooser.addDoubleOptions("Speed", 0.0, 1.0, 0.3)

    assert selectable.getOptions() == ["0.0", "0.3", "0.6", "0.9", "1.0"]
    assert selectable.getSelected() == "0.0"
    assert chooser.getSelectedDouble("Speed") == pytest.approx(0.0)


def test_dpad_moves_between_selectables_and_options() -> None:
    gamepad = Gamepad(0)
    sim = GamepadSim(gamepad)
    chooser = DSGamepadChooser(gamepad)
    auto = chooser.addOptions("Auto", ["Left", "Center", "Right"])
    delay = chooser.addIntegerOptions("Delay", 0, 2, 1)

    tap(sim, chooser, Gamepad.Button.DPAD_RIGHT)
    assert auto.getSelected() == "Center"
    assert delay.getSelected() == "0"
    assert chooser.getSelectedSelectable().getName() == "Auto"

    tap(sim, chooser, Gamepad.Button.DPAD_DOWN)
    assert chooser.getSelectedSelectable().getName() == "Delay"

    tap(sim, chooser, Gamepad.Button.DPAD_RIGHT)
    assert delay.getSelected() == "1"
    assert auto.getSelected() == "Center"

    tap(sim, chooser, Gamepad.Button.DPAD_LEFT)
    tap(sim, chooser, Gamepad.Button.DPAD_LEFT)
    assert delay.getSelected() == "2"

    tap(sim, chooser, Gamepad.Button.DPAD_UP)
    assert chooser.getSelectedSelectable().getName() == "Auto"

    tap(sim, chooser, Gamepad.Button.DPAD_LEFT)
    tap(sim, chooser, Gamepad.Button.DPAD_LEFT)
    assert auto.getSelected() == "Right"


def test_rejects_invalid_selectables() -> None:
    chooser = DSGamepadChooser(0)

    with pytest.raises(ValueError):
        chooser.addOptions("", ["A"])
    with pytest.raises(ValueError):
        chooser.addOptions("Empty", [])
    with pytest.raises(ValueError):
        chooser.addIntegerOptions("Bad", 0, 2, 0)
    with pytest.raises(ValueError):
        chooser.addDoubleOptions("Bad", 0.0, 1.0, 0.0)

    chooser.addOptions("Auto", ["A"])
    with pytest.raises(ValueError):
        chooser.addOptions("Auto", ["B"])
    with pytest.raises(ValueError):
        chooser.getSelected("Missing")
