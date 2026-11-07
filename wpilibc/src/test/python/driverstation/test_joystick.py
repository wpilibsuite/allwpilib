import math

from wpilib import Joystick
from wpilib.simulation import JoystickSim


def test_fast_deconstruction(wpilib_state):
    # https://github.com/wpilibsuite/allwpilib/issues/1550
    Joystick(0)


def test_get_x() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.set_x(0.25)
    joysim.notify_new_data()
    assert math.isclose(joy.get_x(), 0.25)

    joysim.set_x(0)
    joysim.notify_new_data()
    assert math.isclose(joy.get_x(), 0.0)


def test_get_y() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.set_y(0.25)
    joysim.notify_new_data()
    assert math.isclose(joy.get_y(), 0.25)

    joysim.set_y(0)
    joysim.notify_new_data()
    assert math.isclose(joy.get_y(), 0.0)


def test_get_z() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.set_z(0.25)
    joysim.notify_new_data()
    assert math.isclose(joy.get_z(), 0.25)

    joysim.set_z(0)
    joysim.notify_new_data()
    assert math.isclose(joy.get_z(), 0.0)


def test_get_twist() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.set_twist(0.25)
    joysim.notify_new_data()
    assert math.isclose(joy.get_twist(), 0.25)

    joysim.set_twist(0)
    joysim.notify_new_data()
    assert math.isclose(joy.get_twist(), 0.0)


def test_get_throttle() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.set_throttle(0.25)
    joysim.notify_new_data()
    assert math.isclose(joy.get_throttle(), 0.25)

    joysim.set_throttle(0)
    joysim.notify_new_data()
    assert math.isclose(joy.get_throttle(), 0.0)


def test_get_trigger() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.set_trigger(True)
    joysim.notify_new_data()
    assert joy.get_trigger()

    joysim.set_trigger(False)
    joysim.notify_new_data()
    assert not joy.get_trigger()


def test_get_top() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.set_top(True)
    joysim.notify_new_data()
    assert joy.get_top()

    joysim.set_top(False)
    joysim.notify_new_data()
    assert not joy.get_top()


def test_get_magnitude() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    # X Only
    joysim.set_x(0.5)
    joysim.set_y(0.0)
    joysim.notify_new_data()
    assert math.isclose(joy.get_magnitude(), 0.5)

    # Y Only
    joysim.set_x(0.0)
    joysim.set_y(-0.5)
    joysim.notify_new_data()
    assert math.isclose(joy.get_magnitude(), 0.5)

    # Both
    joysim.set_x(0.5)
    joysim.set_y(-0.5)
    joysim.notify_new_data()
    assert math.isclose(joy.get_magnitude(), 0.70710678118)


def test_get_direction() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    # X Only
    joysim.set_x(0.5)
    joysim.set_y(0.0)
    joysim.notify_new_data()
    assert math.isclose(joy.get_direction_degrees(), 90)
    assert math.isclose(joy.get_direction_radians(), math.radians(90))

    # Y Only
    joysim.set_x(0.0)
    joysim.set_y(-0.5)
    joysim.notify_new_data()
    assert math.isclose(joy.get_direction_degrees(), 0)
    assert math.isclose(joy.get_direction_radians(), math.radians(0))

    # Both
    joysim.set_x(0.5)
    joysim.set_y(-0.5)
    joysim.notify_new_data()
    assert math.isclose(joy.get_direction_degrees(), 45)
    assert math.isclose(joy.get_direction_radians(), math.radians(45))
