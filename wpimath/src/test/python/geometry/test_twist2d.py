import pytest
import math

from wpimath.geometry import Twist2d, Pose2d, Rotation2d


def test_straight():
    straight = Twist2d(dx=5, dy=0, dtheta=0)
    straight_pose = straight.exp()

    assert straight_pose.x == pytest.approx(5.0)
    assert straight_pose.y == pytest.approx(0.0)
    assert straight_pose.rotation().radians() == pytest.approx(0.0)


def test_quarter_circle():
    quarter_circle = Twist2d(dx=5 / 2.0 * math.pi, dy=0, dtheta=math.pi / 2.0)
    quarter_circle_pose = quarter_circle.exp()

    assert quarter_circle_pose.x == pytest.approx(5.0)
    assert quarter_circle_pose.y == pytest.approx(5.0)
    assert quarter_circle_pose.rotation().degrees() == pytest.approx(90.0)


def test_diagonal_no_dtheta():
    diagonal = Twist2d(dx=2, dy=2, dtheta=0)
    diagonal_pose = diagonal.exp()

    assert diagonal_pose.x == pytest.approx(2.0)
    assert diagonal_pose.y == pytest.approx(2.0)
    assert diagonal_pose.rotation().degrees() == pytest.approx(0.0)


def test_equality():
    one = Twist2d(dx=5, dy=1, dtheta=3)
    two = Twist2d(dx=5, dy=1, dtheta=3)
    assert one == two


def test_inequality():
    one = Twist2d(dx=5, dy=1, dtheta=3)
    two = Twist2d(dx=5, dy=1.2, dtheta=3)
    assert one != two


def test_pose2d_log():
    end = Pose2d(x=5, y=5, angle=math.radians(90))
    start = Pose2d()

    twist = (end - start).log()

    expected = Twist2d(dx=5.0 / 2.0 * math.pi, dy=0, dtheta=math.pi / 2.0)
    assert expected == twist

    reapplied = start + twist.exp()
    assert end == reapplied
