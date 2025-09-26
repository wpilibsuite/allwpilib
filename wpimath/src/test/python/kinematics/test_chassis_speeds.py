import math

from wpimath.geometry import Rotation2d
from wpimath.kinematics import ChassisSpeeds


def test_plus() -> None:
    left = ChassisSpeeds(vx=1.0, vy=0.5, omega=0.75)
    right = ChassisSpeeds(vx=2.0, vy=1.5, omega=0.25)

    result = left + right

    assert math.isclose(3.0, result.vx)
    assert math.isclose(2.0, result.vy)
    assert math.isclose(1.0, result.omega)


def test_minus() -> None:
    left = ChassisSpeeds(vx=1.0, vy=0.5, omega=0.75)
    right = ChassisSpeeds(vx=2.0, vy=0.5, omega=0.25)

    result = left - right

    assert math.isclose(-1.0, result.vx)
    assert math.isclose(0, result.vy)
    assert math.isclose(0.5, result.omega)


def test_neg() -> None:
    speeds = ChassisSpeeds(vx=1.0, vy=0.5, omega=0.75)

    result = -speeds

    assert math.isclose(-1.0, result.vx)
    assert math.isclose(-0.5, result.vy)
    assert math.isclose(-0.75, result.omega)


def test_multiplication() -> None:
    speeds = ChassisSpeeds(vx=1.0, vy=0.5, omega=0.75)

    result = speeds * 2

    assert math.isclose(2.0, result.vx)
    assert math.isclose(1.0, result.vy)
    assert math.isclose(1.5, result.omega)


def test_division() -> None:
    speeds = ChassisSpeeds(vx=1.0, vy=0.5, omega=0.75)

    result = speeds / 2

    assert math.isclose(0.5, result.vx)
    assert math.isclose(0.25, result.vy)
    assert math.isclose(0.375, result.omega)


def test_unpack() -> None:
    speeds = ChassisSpeeds(1, 1, 0.5)

    vx, vy, omega = speeds

    assert math.isclose(1, vx)
    assert math.isclose(1, vy)
    assert math.isclose(0.5, omega)


def test_repr() -> None:
    speeds = ChassisSpeeds(2.0, 1.0, 0.0)

    assert repr(speeds) == "ChassisSpeeds(vx=2.000000, vy=1.000000, omega=0.000000)"
