import pytest
import math

from wpimath.geometry import Pose2d, Translation2d, Ellipse2d, Rotation2d


def test_focal_points():
    center = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(0))
    ellipse = Ellipse2d(center, xSemiAxis=5, ySemiAxis=4)

    a, b = ellipse.focalPoints()

    assert a == Translation2d(x=-2, y=2)
    assert b == Translation2d(x=4, y=2)


def test_intersects():
    center = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(0))
    ellipse = Ellipse2d(center, xSemiAxis=2, ySemiAxis=1)

    point_a = Translation2d(x=1, y=3)
    point_b = Translation2d(x=0, y=3)

    assert ellipse.intersects(point_a)
    assert not ellipse.intersects(point_b)


def test_contains():
    center = Pose2d(x=-1, y=-2, rotation=Rotation2d.fromDegrees(45))
    ellipse = Ellipse2d(center, xSemiAxis=2, ySemiAxis=1)

    point_a = Translation2d(x=0, y=-1)
    point_b = Translation2d(x=0.5, y=-2)

    assert ellipse.contains(point_a)
    assert not ellipse.contains(point_b)


def test_distance():
    k_epsilon = 1e-9

    center = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(270))
    ellipse = Ellipse2d(center, xSemiAxis=1, ySemiAxis=2)

    point1 = Translation2d(x=2.5, y=2)
    assert ellipse.distance(point1) == pytest.approx(0, abs=k_epsilon)

    point2 = Translation2d(x=1, y=2)
    assert ellipse.distance(point2) == pytest.approx(0, abs=k_epsilon)

    point3 = Translation2d(x=1, y=1)
    assert ellipse.distance(point3) == pytest.approx(0, abs=k_epsilon)

    point4 = Translation2d(x=-1, y=2.5)
    assert ellipse.distance(point4) == pytest.approx(0.19210128384806818, abs=k_epsilon)


def test_nearest():
    k_epsilon = 1e-9

    center = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(270))
    ellipse = Ellipse2d(center, xSemiAxis=1, ySemiAxis=2)

    point1 = Translation2d(x=2.5, y=2)
    nearest_point1 = ellipse.nearest(point1)
    assert nearest_point1.x == pytest.approx(2.5, abs=k_epsilon)
    assert nearest_point1.y == pytest.approx(2.0, abs=k_epsilon)

    point2 = Translation2d(x=1, y=2)
    nearest_point2 = ellipse.nearest(point2)
    assert nearest_point2.x == pytest.approx(1.0, abs=k_epsilon)
    assert nearest_point2.y == pytest.approx(2.0, abs=k_epsilon)

    point3 = Translation2d(x=1, y=1)
    nearest_point3 = ellipse.nearest(point3)
    assert nearest_point3.x == pytest.approx(1.0, abs=k_epsilon)
    assert nearest_point3.y == pytest.approx(1.0, abs=k_epsilon)

    point4 = Translation2d(x=-1, y=2.5)
    nearest_point4 = ellipse.nearest(point4)
    assert nearest_point4.x == pytest.approx(-0.8512799937611617, abs=k_epsilon)
    assert nearest_point4.y == pytest.approx(2.378405333174535, abs=k_epsilon)


def test_equals():
    center1 = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(90))
    ellipse1 = Ellipse2d(center1, xSemiAxis=2, ySemiAxis=3)

    center2 = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(90))
    ellipse2 = Ellipse2d(center2, xSemiAxis=2, ySemiAxis=3)

    center3 = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(90))
    ellipse3 = Ellipse2d(center3, xSemiAxis=3, ySemiAxis=2)

    assert ellipse1 == ellipse2
    assert ellipse1 != ellipse3
    assert ellipse3 != ellipse2
