import importlib.util
import math

import pytest

from wpimath import Rotation2d


@pytest.mark.parametrize(
    "radians,degrees",
    [
        (math.pi / 3, 60.0),
        (math.pi / 4, 45.0),
    ],
)
def test_radians_to_degrees(radians: float, degrees: float):
    rot = Rotation2d(radians)
    assert math.isclose(degrees, rot.degrees())


@pytest.mark.parametrize(
    "degrees,radians",
    [
        (45.0, math.pi / 4),
        (30.0, math.pi / 6),
    ],
)
def test_degrees_to_radians(degrees: float, radians: float):
    rot = Rotation2d.fromDegrees(degrees)
    assert math.isclose(radians, rot.radians())


def test_rotate_by_from_zero() -> None:
    zero = Rotation2d()
    rotated = zero + Rotation2d.fromDegrees(90)

    assert math.isclose(math.pi / 2, rotated.radians())
    assert math.isclose(90.0, rotated.degrees())


def test_rotate_by_non_zero() -> None:
    rot = Rotation2d.fromDegrees(90.0)
    rot += Rotation2d.fromDegrees(30.0)

    assert math.isclose(120.0, rot.degrees())


def test_minus() -> None:
    rot1 = Rotation2d.fromDegrees(70)
    rot2 = Rotation2d.fromDegrees(30)

    assert math.isclose(40.0, (rot1 - rot2).degrees())


def test_unary_minus() -> None:
    rot = Rotation2d.fromDegrees(20)
    assert math.isclose(-20.0, (-rot).degrees())


def test_equality() -> None:
    rot1 = Rotation2d.fromDegrees(43.0)
    rot2 = Rotation2d.fromDegrees(43.0)
    assert rot1 == rot2

    rot1 = Rotation2d.fromDegrees(-180.0)
    rot2 = Rotation2d.fromDegrees(180.0)
    assert rot1 == rot2


def test_inequality() -> None:
    rot1 = Rotation2d.fromDegrees(43.0)
    rot2 = Rotation2d.fromDegrees(43.5)
    assert rot1 != rot2


@pytest.mark.skipif(
    importlib.util.find_spec("numpy") is None, reason="numpy is not available"
)
def test_to_matrix() -> None:
    before = Rotation2d.fromDegrees(20.0)
    after = Rotation2d.fromMatrix(before.toMatrix())
    assert before == after
