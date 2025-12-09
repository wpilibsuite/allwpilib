import pytest
import math
import numpy as np

from wpimath.geometry import Twist2d, Pose2d
from wpimath.units import meters, radians


def test_straight():
    straight = Twist2d(meters(5), meters(0), radians(0))
    straight_pose = straight.exp()

    assert straight_pose.x == pytest.approx(5.0)
    assert straight_pose.y == pytest.approx(0.0)
    assert straight_pose.rotation().radians() == pytest.approx(0.0)


def test_quarter_circle():
    quarter_circle = Twist2d(
        meters(5) / 2.0 * math.pi, meters(0), radians(math.pi / 2.0)
    )
    quarter_circle_pose = quarter_circle.exp()

    assert quarter_circle_pose.x == pytest.approx(5.0)
    assert quarter_circle_pose.y == pytest.approx(5.0)
    assert quarter_circle_pose.rotation().degrees() == pytest.approx(90.0)


def test_diagonal_no_dtheta():
    diagonal = Twist2d(meters(2), meters(2), math.radians(0))
    diagonal_pose = diagonal.exp()

    assert diagonal_pose.x == pytest.approx(2.0)
    assert diagonal_pose.y == pytest.approx(2.0)
    assert diagonal_pose.rotation().degrees() == pytest.approx(0.0)


def test_equality():
    one = Twist2d(meters(5), meters(1), radians(3))
    two = Twist2d(meters(5), meters(1), radians(3))
    assert one == two


def test_inequality():
    one = Twist2d(meters(5), meters(1), radians(3))
    two = Twist2d(meters(5), meters(1.2), radians(3))
    assert one != two


def test_pose2d_log():
    end = Pose2d(meters(5), meters(5), math.radians(90))
    start = Pose2d()

    twist = (end - start).log()

    expected = Twist2d(
        meters(5.0 / 2.0 * math.pi), meters(0), radians(math.pi / 2.0)
    )
    assert expected == twist

    reapplied = start + twist.exp()
    assert end == reapplied


def test_constexpr():
    default_ctor = Twist2d()
    component_ctor = Twist2d(meters(1), meters(2), radians(3))
    multiplied = component_ctor * 2

    assert default_ctor.dx == pytest.approx(0.0)
    assert component_ctor.dy == pytest.approx(2.0)
    assert multiplied.dtheta == pytest.approx(6.0)