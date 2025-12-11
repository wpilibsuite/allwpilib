import pytest
import math
import numpy as np

from wpimath.geometry import (
    Translation3d,
    Rotation3d,
    Translation2d,
)


def test_sum():
    one = Translation3d(x=1, y=3, z=5)
    two = Translation3d(x=2, y=5, z=8)

    sum_ = one + two

    assert sum_.x == pytest.approx(3.0, abs=1e-9)
    assert sum_.y == pytest.approx(8.0, abs=1e-9)
    assert sum_.z == pytest.approx(13.0, abs=1e-9)


def test_difference():
    one = Translation3d(x=1, y=3, z=5)
    two = Translation3d(x=2, y=5, z=8)

    difference = one - two

    assert difference.x == pytest.approx(-1.0, abs=1e-9)
    assert difference.y == pytest.approx(-2.0, abs=1e-9)
    assert difference.z == pytest.approx(-3.0, abs=1e-9)


def test_rotate_by():
    x_axis = np.array([1.0, 0.0, 0.0])
    y_axis = np.array([0.0, 1.0, 0.0])
    z_axis = np.array([0.0, 0.0, 1.0])

    translation = Translation3d(x=1, y=2, z=3)

    rotated1 = translation.rotateBy(Rotation3d(x_axis, math.radians(90)))
    assert rotated1.x == pytest.approx(1.0, abs=1e-9)
    assert rotated1.y == pytest.approx(-3.0, abs=1e-9)
    assert rotated1.z == pytest.approx(2.0, abs=1e-9)

    rotated2 = translation.rotateBy(Rotation3d(y_axis, math.radians(90)))
    assert rotated2.x == pytest.approx(3.0, abs=1e-9)
    assert rotated2.y == pytest.approx(2.0, abs=1e-9)
    assert rotated2.z == pytest.approx(-1.0, abs=1e-9)

    rotated3 = translation.rotateBy(Rotation3d(z_axis, math.radians(90)))
    assert rotated3.x == pytest.approx(-2.0, abs=1e-9)
    assert rotated3.y == pytest.approx(1.0, abs=1e-9)
    assert rotated3.z == pytest.approx(3.0, abs=1e-9)


def test_rotate_around():
    x_axis = np.array([1.0, 0.0, 0.0])
    y_axis = np.array([0.0, 1.0, 0.0])
    z_axis = np.array([0.0, 0.0, 1.0])

    translation = Translation3d(x=1, y=2, z=3)
    around = Translation3d(x=3, y=2, z=1)

    rotated1 = translation.rotateAround(around, Rotation3d(x_axis, math.radians(90)))
    assert rotated1.x == pytest.approx(1.0, abs=1e-9)
    assert rotated1.y == pytest.approx(0.0, abs=1e-9)
    assert rotated1.z == pytest.approx(1.0, abs=1e-9)

    rotated2 = translation.rotateAround(around, Rotation3d(y_axis, math.radians(90)))
    assert rotated2.x == pytest.approx(5.0, abs=1e-9)
    assert rotated2.y == pytest.approx(2.0, abs=1e-9)
    assert rotated2.z == pytest.approx(3.0, abs=1e-9)

    rotated3 = translation.rotateAround(around, Rotation3d(z_axis, math.radians(90)))
    assert rotated3.x == pytest.approx(3.0, abs=1e-9)
    assert rotated3.y == pytest.approx(0.0, abs=1e-9)
    assert rotated3.z == pytest.approx(3.0, abs=1e-9)


def test_to_translation2d():
    translation = Translation3d(x=1, y=2, z=3)
    expected = Translation2d(x=1, y=2)

    assert expected == translation.toTranslation2d()


def test_multiplication():
    original = Translation3d(x=3, y=5, z=7)
    mult = original * 3

    assert mult.x == pytest.approx(9.0, abs=1e-9)
    assert mult.y == pytest.approx(15.0, abs=1e-9)
    assert mult.z == pytest.approx(21.0, abs=1e-9)


def test_division():
    original = Translation3d(x=3, y=5, z=7)
    div = original / 2

    assert div.x == pytest.approx(1.5, abs=1e-9)
    assert div.y == pytest.approx(2.5, abs=1e-9)
    assert div.z == pytest.approx(3.5, abs=1e-9)


def test_norm():
    one = Translation3d(x=3, y=5, z=7)
    assert one.norm() == pytest.approx(math.hypot(3, 5, 7), abs=1e-9)


def test_squared_norm():
    one = Translation3d(x=3, y=5, z=7)
    assert one.squaredNorm() == pytest.approx(83.0, abs=1e-9)


def test_distance():
    one = Translation3d(x=1, y=1, z=1)
    two = Translation3d(x=6, y=6, z=6)
    assert one.distance(two) == pytest.approx(5 * math.sqrt(3), abs=1e-9)


def test_squared_distance():
    one = Translation3d(x=1, y=1, z=1)
    two = Translation3d(x=6, y=6, z=6)
    assert one.squaredDistance(two) == pytest.approx(75.0, abs=1e-9)


def test_unary_minus():
    original = Translation3d(x=-4.5, y=7, z=9)
    inverted = -original

    assert inverted.x == pytest.approx(4.5, abs=1e-9)
    assert inverted.y == pytest.approx(-7.0, abs=1e-9)
    assert inverted.z == pytest.approx(-9.0, abs=1e-9)


def test_equality():
    one = Translation3d(x=9, y=5.5, z=3.5)
    two = Translation3d(x=9, y=5.5, z=3.5)
    assert one == two


def test_inequality():
    one = Translation3d(x=9, y=5.5, z=3.5)
    two = Translation3d(x=9, y=5.7, z=3.5)
    assert one != two


def test_polar_constructor():
    z_axis = np.array([0.0, 0.0, 1.0])

    one = Translation3d(math.sqrt(2) * 1, Rotation3d(z_axis, math.radians(45)))
    assert one.x == pytest.approx(1.0, abs=1e-9)
    assert one.y == pytest.approx(1.0, abs=1e-9)
    assert one.z == pytest.approx(0.0, abs=1e-9)

    two = Translation3d(2, Rotation3d(z_axis, math.radians(60)))
    assert two.x == pytest.approx(1.0, abs=1e-9)
    assert two.y == pytest.approx(math.sqrt(3.0), abs=1e-9)
    assert two.z == pytest.approx(0.0, abs=1e-9)


def test_to_vector():
    vec = np.array([1.0, 2.0, 3.0])
    translation = Translation3d(vec)

    assert vec[0] == pytest.approx(translation.x)
    assert vec[1] == pytest.approx(translation.y)
    assert vec[2] == pytest.approx(translation.z)

    assert np.allclose(vec, translation.toVector())


def test_constexpr():
    default_ctor = Translation3d()
    component_ctor = Translation3d(x=1, y=2, z=3)
    added = default_ctor + component_ctor
    subtracted = default_ctor - component_ctor
    negated = -component_ctor
    multiplied = component_ctor * 2
    divided = component_ctor / 2
    projected = component_ctor.toTranslation2d()

    assert default_ctor.x == pytest.approx(0.0)
    assert component_ctor.y == pytest.approx(2.0)
    assert added.z == pytest.approx(3.0)
    assert subtracted.x == pytest.approx(-1.0)
    assert negated.y == pytest.approx(-2.0)
    assert multiplied.z == pytest.approx(6.0)
    assert divided.y == pytest.approx(1.0)
    assert projected.x == pytest.approx(1.0)
    assert projected.y == pytest.approx(2.0)


def test_nearest():
    origin = Translation3d(x=0, y=0, z=0)

    # Distance sort
    # translations are in order of closest to farthest away from the origin at
    # various positions in 3D space.
    translation1 = Translation3d(x=1, y=0, z=0)
    translation2 = Translation3d(x=0, y=2, z=0)
    translation3 = Translation3d(x=0, y=0, z=3)
    translation4 = Translation3d(x=2, y=2, z=2)
    translation5 = Translation3d(x=3, y=3, z=3)

    nearest1 = origin.nearest([translation5, translation3, translation4])
    assert nearest1.x == pytest.approx(translation3.x)
    assert nearest1.y == pytest.approx(translation3.y)
    assert nearest1.z == pytest.approx(translation3.z)

    nearest2 = origin.nearest([translation1, translation2, translation3])
    assert nearest2.x == pytest.approx(translation1.x)
    assert nearest2.y == pytest.approx(translation1.y)
    assert nearest2.z == pytest.approx(translation1.z)

    nearest3 = origin.nearest([translation4, translation2, translation3])
    assert nearest3.x == pytest.approx(translation2.x)
    assert nearest3.y == pytest.approx(translation2.y)
    assert nearest3.z == pytest.approx(translation2.z)


def test_dot():
    one = Translation3d(x=1, y=2, z=3)
    two = Translation3d(x=4, y=5, z=6)
    assert one.dot(two) == pytest.approx(32.0, abs=1e-9)
