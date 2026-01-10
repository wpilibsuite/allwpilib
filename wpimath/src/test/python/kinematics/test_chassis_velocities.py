import math

from wpimath import ChassisVelocities


def test_plus() -> None:
    left = ChassisVelocities(vx=1.0, vy=0.5, omega=0.75)
    right = ChassisVelocities(vx=2.0, vy=1.5, omega=0.25)

    result = left + right

    assert math.isclose(3.0, result.vx)
    assert math.isclose(2.0, result.vy)
    assert math.isclose(1.0, result.omega)


def test_minus() -> None:
    left = ChassisVelocities(vx=1.0, vy=0.5, omega=0.75)
    right = ChassisVelocities(vx=2.0, vy=0.5, omega=0.25)

    result = left - right

    assert math.isclose(-1.0, result.vx)
    assert math.isclose(0, result.vy)
    assert math.isclose(0.5, result.omega)


def test_neg() -> None:
    velocities = ChassisVelocities(vx=1.0, vy=0.5, omega=0.75)

    result = -velocities

    assert math.isclose(-1.0, result.vx)
    assert math.isclose(-0.5, result.vy)
    assert math.isclose(-0.75, result.omega)


def test_multiplication() -> None:
    velocities = ChassisVelocities(vx=1.0, vy=0.5, omega=0.75)

    result = velocities * 2

    assert math.isclose(2.0, result.vx)
    assert math.isclose(1.0, result.vy)
    assert math.isclose(1.5, result.omega)


def test_division() -> None:
    velocities = ChassisVelocities(vx=1.0, vy=0.5, omega=0.75)

    result = velocities / 2

    assert math.isclose(0.5, result.vx)
    assert math.isclose(0.25, result.vy)
    assert math.isclose(0.375, result.omega)


def test_unpack() -> None:
    velocities = ChassisVelocities(1, 1, 0.5)

    vx, vy, omega = velocities

    assert math.isclose(1, vx)
    assert math.isclose(1, vy)
    assert math.isclose(0.5, omega)


def test_repr() -> None:
    velocities = ChassisVelocities(2.0, 1.0, 0.0)

    assert repr(velocities) == "ChassisVelocities(vx=2.000000, vy=1.000000, omega=0.000000)"
