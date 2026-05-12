import pytest

from wpilib import NiDsXboxController
from wpilib.simulation import NiDsXboxControllerSim


def _button_test(joy, joysim, set_fn, get_fn, get_pressed_fn, get_released_fn):
    set_fn(False)
    joysim.notifyNewData()
    assert not get_fn()
    get_pressed_fn()
    get_released_fn()

    set_fn(True)
    joysim.notifyNewData()
    assert get_fn()
    assert get_pressed_fn()
    assert not get_released_fn()

    set_fn(False)
    joysim.notifyNewData()
    assert not get_fn()
    assert not get_pressed_fn()
    assert get_released_fn()


def _axis_test(joy, joysim, set_fn, get_fn):
    set_fn(0.35)
    joysim.notifyNewData()
    assert get_fn() == pytest.approx(0.35, abs=0.001)


def test_get_left_bumper_button():
    joy = NiDsXboxController(1)
    joysim = NiDsXboxControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setLeftBumperButton, joy.getLeftBumperButton,
        joy.getLeftBumperButtonPressed, joy.getLeftBumperButtonReleased,
    )


def test_get_right_bumper_button():
    joy = NiDsXboxController(1)
    joysim = NiDsXboxControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setRightBumperButton, joy.getRightBumperButton,
        joy.getRightBumperButtonPressed, joy.getRightBumperButtonReleased,
    )


def test_get_left_stick_button():
    joy = NiDsXboxController(1)
    joysim = NiDsXboxControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setLeftStickButton, joy.getLeftStickButton,
        joy.getLeftStickButtonPressed, joy.getLeftStickButtonReleased,
    )


def test_get_right_stick_button():
    joy = NiDsXboxController(1)
    joysim = NiDsXboxControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setRightStickButton, joy.getRightStickButton,
        joy.getRightStickButtonPressed, joy.getRightStickButtonReleased,
    )


def test_get_a_button():
    joy = NiDsXboxController(1)
    joysim = NiDsXboxControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setAButton, joy.getAButton,
        joy.getAButtonPressed, joy.getAButtonReleased,
    )


def test_get_b_button():
    joy = NiDsXboxController(1)
    joysim = NiDsXboxControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setBButton, joy.getBButton,
        joy.getBButtonPressed, joy.getBButtonReleased,
    )


def test_get_x_button():
    joy = NiDsXboxController(1)
    joysim = NiDsXboxControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setXButton, joy.getXButton,
        joy.getXButtonPressed, joy.getXButtonReleased,
    )


def test_get_y_button():
    joy = NiDsXboxController(1)
    joysim = NiDsXboxControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setYButton, joy.getYButton,
        joy.getYButtonPressed, joy.getYButtonReleased,
    )


def test_get_back_button():
    joy = NiDsXboxController(1)
    joysim = NiDsXboxControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setBackButton, joy.getBackButton,
        joy.getBackButtonPressed, joy.getBackButtonReleased,
    )


def test_get_start_button():
    joy = NiDsXboxController(1)
    joysim = NiDsXboxControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setStartButton, joy.getStartButton,
        joy.getStartButtonPressed, joy.getStartButtonReleased,
    )


def test_get_left_x():
    joy = NiDsXboxController(2)
    joysim = NiDsXboxControllerSim(joy)
    _axis_test(joy, joysim, joysim.setLeftX, joy.getLeftX)


def test_get_right_x():
    joy = NiDsXboxController(2)
    joysim = NiDsXboxControllerSim(joy)
    _axis_test(joy, joysim, joysim.setRightX, joy.getRightX)


def test_get_left_y():
    joy = NiDsXboxController(2)
    joysim = NiDsXboxControllerSim(joy)
    _axis_test(joy, joysim, joysim.setLeftY, joy.getLeftY)


def test_get_right_y():
    joy = NiDsXboxController(2)
    joysim = NiDsXboxControllerSim(joy)
    _axis_test(joy, joysim, joysim.setRightY, joy.getRightY)


def test_get_left_trigger_axis():
    joy = NiDsXboxController(2)
    joysim = NiDsXboxControllerSim(joy)
    _axis_test(joy, joysim, joysim.setLeftTriggerAxis, joy.getLeftTriggerAxis)


def test_get_right_trigger_axis():
    joy = NiDsXboxController(2)
    joysim = NiDsXboxControllerSim(joy)
    _axis_test(joy, joysim, joysim.setRightTriggerAxis, joy.getRightTriggerAxis)
