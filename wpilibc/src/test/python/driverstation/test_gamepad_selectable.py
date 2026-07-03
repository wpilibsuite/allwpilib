import pytest

from wpilib import DriverStationDisplay, Gamepad, GamepadSelectable
from wpilib.simulation import DriverStationSim, GamepadSim


@pytest.fixture(autouse=True)
def reset_driver_station_sim():
    DriverStationSim.resetData()
    DriverStationDisplay.setMode(DriverStationDisplay.Mode.Line)


def tap(sim: GamepadSim, selectable: GamepadSelectable, button: Gamepad.Button) -> None:
    sim.setButton(button, True)
    sim.notifyNewData()
    selectable.update()

    sim.setButton(button, False)
    sim.notifyNewData()
    selectable.update()


def test_add_options_selects_first_option_by_default() -> None:
    selectable = GamepadSelectable(0)

    chooser = selectable.addOptions("Auto", ["Left", "Center", "Right"])

    assert chooser.getSelected() == "Left"
    assert selectable.getSelected("Auto") == "Left"
    assert selectable.getSelectedIndex("Auto") == 0
    assert selectable.getChooserNames() == ["Auto"]
    assert selectable.getSelectedChooser().getName() == chooser.getName()


def test_add_integer_options_creates_range() -> None:
    selectable = GamepadSelectable(0)

    chooser = selectable.addIntegerOptions("Delay", -2, 3, 2)

    assert chooser.getOptions() == ["-2", "0", "2", "3"]
    assert chooser.getSelected() == "-2"
    assert selectable.getSelectedInteger("Delay") == -2


def test_add_double_options_creates_range() -> None:
    selectable = GamepadSelectable(0)

    chooser = selectable.addDoubleOptions("Speed", 0.0, 1.0, 0.3)

    assert chooser.getOptions() == ["0.0", "0.3", "0.6", "0.9", "1.0"]
    assert chooser.getSelected() == "0.0"
    assert selectable.getSelectedDouble("Speed") == pytest.approx(0.0)


def test_dpad_moves_between_choosers_and_options() -> None:
    gamepad = Gamepad(0)
    sim = GamepadSim(gamepad)
    selectable = GamepadSelectable(gamepad)
    auto = selectable.addOptions("Auto", ["Left", "Center", "Right"])
    delay = selectable.addIntegerOptions("Delay", 0, 2, 1)

    tap(sim, selectable, Gamepad.Button.DPAD_RIGHT)
    assert auto.getSelected() == "Center"
    assert delay.getSelected() == "0"
    assert selectable.getSelectedChooser().getName() == "Auto"

    tap(sim, selectable, Gamepad.Button.DPAD_DOWN)
    assert selectable.getSelectedChooser().getName() == "Delay"

    tap(sim, selectable, Gamepad.Button.DPAD_RIGHT)
    assert delay.getSelected() == "1"
    assert auto.getSelected() == "Center"

    tap(sim, selectable, Gamepad.Button.DPAD_LEFT)
    tap(sim, selectable, Gamepad.Button.DPAD_LEFT)
    assert delay.getSelected() == "2"

    tap(sim, selectable, Gamepad.Button.DPAD_UP)
    assert selectable.getSelectedChooser().getName() == "Auto"

    tap(sim, selectable, Gamepad.Button.DPAD_LEFT)
    tap(sim, selectable, Gamepad.Button.DPAD_LEFT)
    assert auto.getSelected() == "Right"


def test_rejects_invalid_choosers() -> None:
    selectable = GamepadSelectable(0)

    with pytest.raises(ValueError):
        selectable.addOptions("", ["A"])
    with pytest.raises(ValueError):
        selectable.addOptions("Empty", [])
    with pytest.raises(ValueError):
        selectable.addIntegerOptions("Bad", 0, 2, 0)
    with pytest.raises(ValueError):
        selectable.addDoubleOptions("Bad", 0.0, 1.0, 0.0)

    selectable.addOptions("Auto", ["A"])
    with pytest.raises(ValueError):
        selectable.addOptions("Auto", ["B"])
    with pytest.raises(ValueError):
        selectable.getSelected("Missing")
