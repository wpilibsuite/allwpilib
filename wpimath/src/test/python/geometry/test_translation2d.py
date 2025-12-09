import pytest
import math

from wpimath.geometry import Translation2d, Rotation2d


def test_sum():
    one = Translation2d(1.0, 3.0)
    two = Translation2d(2.0, 5.0)

    sum_ = one + two

    assert sum_.x == pytest.approx(3.0)
    assert sum_.y == pytest.approx(8.0)


def test_difference():
    one = Translation2d(1.0, 3.0)
    two = Translation2d(2.0, 5.0)

    difference = one - two

    assert difference.x == pytest.approx(-1.0)
    assert difference.y == pytest.approx(-2.0)


def test_rotate_by():
    another = Translation2d(3.0, 0.0)
    rotated = another.rotateBy(Rotation2d.fromDegrees(90))

    assert rotated.x == pytest.approx(0.0, abs=1e-9)
    assert rotated.y == pytest.approx(3.0, abs=1e-9)


def test_rotate_around():
    translation = Translation2d(2.0, 1.0)
    other = Translation2d(3.0, 2.0)
    rotated = translation.rotateAround(other, Rotation2d.fromDegrees(180))

    assert rotated.x == pytest.approx(4.0, abs=1e-9)
    assert rotated.y == pytest.approx(3.0, abs=1e-9)


def test_multiplication():
    original = Translation2d(3.0, 5.0)
    mult = original * 3

    assert mult.x == pytest.approx(9.0)
    assert mult.y == pytest.approx(15.0)


def test_division():
    original = Translation2d(3.0, 5.0)
    div = original / 2

    assert div.x == pytest.approx(1.5)
    assert div.y == pytest.approx(2.5)


def test_norm():
    one = Translation2d(3.0, 5.0)
    assert one.norm() == pytest.approx(math.hypot(3.0, 5.0))


def test_squared_norm():
    one = Translation2d(3.0, 5.0)
    assert one.squaredNorm() == pytest.approx(34.0)


def test_distance():
    one = Translation2d(1.0, 1.0)
    two = Translation2d(6.0, 6.0)
    assert one.distance(two) == pytest.approx(5.0 * math.sqrt(2.0))


def test_squared_distance():
    one = Translation2d(1.0, 1.0)
    two = Translation2d(6.0, 6.0)
    assert one.distance(two) ** 2 == pytest.approx(50.0)


def test_unary_minus():
    original = Translation2d(-4.5, 7.0)
    inverted = -original

    assert inverted.x == pytest.approx(4.5)
    assert inverted.y == pytest.approx(-7.0)


def test_equality():
    one = Translation2d(9.0, 5.5)
    two = Translation2d(9.0, 5.5)
    assert one == two


def test_inequality():
    one = Translation2d(9.0, 5.5)
    two = Translation2d(9.0, 5.7)
    assert one != two


def test_polar_constructor():
    one = Translation2d(math.sqrt(2), Rotation2d.fromDegrees(45))
    assert one.x == pytest.approx(1.0)
    assert one.y == pytest.approx(1.0)

    two = Translation2d(2.0, Rotation2d.fromDegrees(60))
    assert two.x == pytest.approx(1.0)
    assert two.y == pytest.approx(math.sqrt(3.0))


def test_nearest():
    origin = Translation2d(0.0, 0.0)

    translation1 = Translation2d(1.0, Rotation2d.fromDegrees(45))
    translation2 = Translation2d(2.0, Rotation2d.fromDegrees(90))
    translation3 = Translation2d(3.0, Rotation2d.fromDegrees(135))
    translation4 = Translation2d(4.0, Rotation2d.fromDegrees(180))
    translation5 = Translation2d(5.0, Rotation2d.fromDegrees(270))

    assert origin.nearest([translation5, translation3, translation4]) == translation3
    assert origin.nearest([translation1, translation2, translation3]) == translation1
    assert origin.nearest([translation4, translation2, translation3]) == translation2


def test_dot():
    one = Translation2d(2.0, 3.0)
    two = Translation2d(3.0, 4.0)
    assert one.dot(two) == pytest.approx(18.0)


def test_cross():
    one = Translation2d(2.0, 3.0)
    two = Translation2d(3.0, 4.0)
    assert one.cross(two) == pytest.approx(-1.0)