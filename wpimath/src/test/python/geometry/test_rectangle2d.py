import pytest
import math
import numpy as np

from wpimath.geometry import (
    Pose2d,
    Translation2d,
    Rotation2d,
    Rectangle2d
)
from wpimath.units import meters


def test_new_with_corners():
    corner_a = Translation2d(meters(1), meters(2))
    corner_b = Translation2d(meters(4), meters(6))

    rect = Rectangle2d(corner_a, corner_b)

    assert rect.xwidth == pytest.approx(3.0)
    assert rect.ywidth == pytest.approx(4.0)
    assert rect.center().x == pytest.approx(2.5)
    assert rect.center().y == pytest.approx(4.0)


def test_intersects():
    center = Pose2d(meters(4), meters(3), math.radians(90))
    rect = Rectangle2d(center, meters(2), meters(3))

    assert rect.intersects(Translation2d(meters(5.5), meters(4)))
    assert rect.intersects(Translation2d(meters(3), meters(2)))
    assert not rect.intersects(Translation2d(meters(4), meters(1.5)))
    assert not rect.intersects(Translation2d(meters(4), meters(3.5)))


def test_contains():
    center = Pose2d(meters(2), meters(3), math.radians(45))
    rect = Rectangle2d(center, meters(3), meters(1))

    assert rect.contains(Translation2d(meters(2), meters(3)))
    assert rect.contains(Translation2d(meters(3), meters(4)))
    assert not rect.contains(Translation2d(meters(3), meters(3)))


def test_distance():
    k_epsilon = 1e-9

    center = Pose2d(meters(1), meters(2), math.radians(270))
    rect = Rectangle2d(center, meters(1), meters(2))

    point1 = Translation2d(meters(2.5), meters(2))
    assert rect.distance(point1) == pytest.approx(0.5, abs=k_epsilon)

    point2 = Translation2d(meters(1), meters(2))
    assert rect.distance(point2) == pytest.approx(0, abs=k_epsilon)

    point3 = Translation2d(meters(1), meters(1))
    assert rect.distance(point3) == pytest.approx(0.5, abs=k_epsilon)

    point4 = Translation2d(meters(-1), meters(2.5))
    assert rect.distance(point4) == pytest.approx(1, abs=k_epsilon)


def test_nearest():
    k_epsilon = 1e-9

    center = Pose2d(meters(1), meters(1), math.radians(90))
    rect = Rectangle2d(center, meters(3), meters(4))

    point1 = Translation2d(meters(1), meters(3))
    nearest_point1 = rect.nearest(point1)
    assert nearest_point1.x == pytest.approx(1.0, abs=k_epsilon)
    assert nearest_point1.y == pytest.approx(2.5, abs=k_epsilon)

    point2 = Translation2d(meters(0), meters(0))
    nearest_point2 = rect.nearest(point2)
    assert nearest_point2.x == pytest.approx(0.0, abs=k_epsilon)
    assert nearest_point2.y == pytest.approx(0.0, abs=k_epsilon)


def test_equals():
    center1 = Pose2d(meters(2), meters(3), math.radians(0))
    rect1 = Rectangle2d(center1, meters(5), meters(3))

    center2 = Pose2d(meters(2), meters(3), math.radians(0))
    rect2 = Rectangle2d(center2, meters(5), meters(3))

    center3 = Pose2d(meters(2), meters(3), math.radians(0))
    rect3 = Rectangle2d(center3, meters(3), meters(3))

    assert rect1 == rect2
    assert rect2 != rect3