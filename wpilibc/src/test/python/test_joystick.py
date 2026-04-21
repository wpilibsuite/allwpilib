import math

from wpilib import Joystick
from wpilib.simulation import JoystickSim


def test_getX() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.setX(0.25)
    joysim.notifyNewData()
    assert math.isclose(joy.getX(), 0.25)

    joysim.setX(0)
    joysim.notifyNewData()
    assert math.isclose(joy.getX(), 0.0)


def test_getY() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.setY(0.25)
    joysim.notifyNewData()
    assert math.isclose(joy.getY(), 0.25)

    joysim.setY(0)
    joysim.notifyNewData()
    assert math.isclose(joy.getY(), 0.0)


def test_getZ() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.setZ(0.25)
    joysim.notifyNewData()
    assert math.isclose(joy.getZ(), 0.25)

    joysim.setZ(0)
    joysim.notifyNewData()
    assert math.isclose(joy.getZ(), 0.0)


def test_getTwist() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.setTwist(0.25)
    joysim.notifyNewData()
    assert math.isclose(joy.getTwist(), 0.25)

    joysim.setTwist(0)
    joysim.notifyNewData()
    assert math.isclose(joy.getTwist(), 0.0)


def test_getThrottle() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.setThrottle(0.25)
    joysim.notifyNewData()
    assert math.isclose(joy.getThrottle(), 0.25)

    joysim.setThrottle(0)
    joysim.notifyNewData()
    assert math.isclose(joy.getThrottle(), 0.0)


def test_getTrigger() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.setTrigger(True)
    joysim.notifyNewData()
    assert joy.getTrigger()

    joysim.setTrigger(False)
    joysim.notifyNewData()
    assert not joy.getTrigger()


def test_getTop() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    joysim.setTop(True)
    joysim.notifyNewData()
    assert joy.getTop()

    joysim.setTop(False)
    joysim.notifyNewData()
    assert not joy.getTop()


def test_getMagnitude() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    # X Only
    joysim.setX(0.5)
    joysim.setY(0.0)
    joysim.notifyNewData()
    assert math.isclose(joy.getMagnitude(), 0.5)

    # Y Only
    joysim.setX(0.0)
    joysim.setY(-0.5)
    joysim.notifyNewData()
    assert math.isclose(joy.getMagnitude(), 0.5)

    # Both
    joysim.setX(0.5)
    joysim.setY(-0.5)
    joysim.notifyNewData()
    assert math.isclose(joy.getMagnitude(), 0.70710678118)


def test_getDirection() -> None:
    joy = Joystick(1)
    joysim = JoystickSim(joy)

    # X Only
    joysim.setX(0.5)
    joysim.setY(0.0)
    joysim.notifyNewData()
    assert math.isclose(joy.getDirectionDegrees(), 90)
    assert math.isclose(joy.getDirectionRadians(), math.radians(90))

    # Y Only
    joysim.setX(0.0)
    joysim.setY(-0.5)
    joysim.notifyNewData()
    assert math.isclose(joy.getDirectionDegrees(), 0)
    assert math.isclose(joy.getDirectionRadians(), math.radians(0))

    # Both
    joysim.setX(0.5)
    joysim.setY(-0.5)
    joysim.notifyNewData()
    assert math.isclose(joy.getDirectionDegrees(), 45)
    assert math.isclose(joy.getDirectionRadians(), math.radians(45))
