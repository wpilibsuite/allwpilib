import pytest

from wpilib import NiDsPS5Controller
from wpilib.simulation import NiDsPS5ControllerSim


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


def test_get_square_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setSquareButton, joy.getSquareButton,
        joy.getSquareButtonPressed, joy.getSquareButtonReleased,
    )


def test_get_cross_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setCrossButton, joy.getCrossButton,
        joy.getCrossButtonPressed, joy.getCrossButtonReleased,
    )


def test_get_circle_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setCircleButton, joy.getCircleButton,
        joy.getCircleButtonPressed, joy.getCircleButtonReleased,
    )


def test_get_triangle_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setTriangleButton, joy.getTriangleButton,
        joy.getTriangleButtonPressed, joy.getTriangleButtonReleased,
    )


def test_get_l1_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setL1Button, joy.getL1Button,
        joy.getL1ButtonPressed, joy.getL1ButtonReleased,
    )


def test_get_r1_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setR1Button, joy.getR1Button,
        joy.getR1ButtonPressed, joy.getR1ButtonReleased,
    )


def test_get_l2_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setL2Button, joy.getL2Button,
        joy.getL2ButtonPressed, joy.getL2ButtonReleased,
    )


def test_get_r2_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setR2Button, joy.getR2Button,
        joy.getR2ButtonPressed, joy.getR2ButtonReleased,
    )


def test_get_create_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setCreateButton, joy.getCreateButton,
        joy.getCreateButtonPressed, joy.getCreateButtonReleased,
    )


def test_get_options_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setOptionsButton, joy.getOptionsButton,
        joy.getOptionsButtonPressed, joy.getOptionsButtonReleased,
    )


def test_get_l3_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setL3Button, joy.getL3Button,
        joy.getL3ButtonPressed, joy.getL3ButtonReleased,
    )


def test_get_r3_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setR3Button, joy.getR3Button,
        joy.getR3ButtonPressed, joy.getR3ButtonReleased,
    )


def test_get_ps_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setPSButton, joy.getPSButton,
        joy.getPSButtonPressed, joy.getPSButtonReleased,
    )


def test_get_touchpad_button():
    joy = NiDsPS5Controller(1)
    joysim = NiDsPS5ControllerSim(joy)
    _button_test(
        joy, joysim,
        joysim.setTouchpadButton, joy.getTouchpadButton,
        joy.getTouchpadButtonPressed, joy.getTouchpadButtonReleased,
    )


def test_get_left_x():
    joy = NiDsPS5Controller(2)
    joysim = NiDsPS5ControllerSim(joy)
    _axis_test(joy, joysim, joysim.setLeftX, joy.getLeftX)


def test_get_right_x():
    joy = NiDsPS5Controller(2)
    joysim = NiDsPS5ControllerSim(joy)
    _axis_test(joy, joysim, joysim.setRightX, joy.getRightX)


def test_get_left_y():
    joy = NiDsPS5Controller(2)
    joysim = NiDsPS5ControllerSim(joy)
    _axis_test(joy, joysim, joysim.setLeftY, joy.getLeftY)


def test_get_right_y():
    joy = NiDsPS5Controller(2)
    joysim = NiDsPS5ControllerSim(joy)
    _axis_test(joy, joysim, joysim.setRightY, joy.getRightY)


def test_get_l2_axis():
    joy = NiDsPS5Controller(2)
    joysim = NiDsPS5ControllerSim(joy)
    _axis_test(joy, joysim, joysim.setL2Axis, joy.getL2Axis)


def test_get_r2_axis():
    joy = NiDsPS5Controller(2)
    joysim = NiDsPS5ControllerSim(joy)
    _axis_test(joy, joysim, joysim.setR2Axis, joy.getR2Axis)
