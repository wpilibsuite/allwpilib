import pytest

from wpimath.kinematics import DifferentialDriveWheelSpeeds


def test_plus():
    left = DifferentialDriveWheelSpeeds(left=1.0, right=0.5)
    right = DifferentialDriveWheelSpeeds(left=2.0, right=1.5)

    result = left + right

    assert result.left == 3.0
    assert result.right == 2.0


def test_minus():
    left = DifferentialDriveWheelSpeeds(left=1.0, right=0.5)
    right = DifferentialDriveWheelSpeeds(left=2.0, right=0.5)

    result = left - right

    assert result.left == -1.0
    assert result.right == 0.0


def test_unary_minus():
    speeds = DifferentialDriveWheelSpeeds(left=1.0, right=0.5)

    result = -speeds

    assert result.left == -1.0
    assert result.right == -0.5


def test_multiplication():
    speeds = DifferentialDriveWheelSpeeds(left=1.0, right=0.5)

    result = speeds * 2

    assert result.left == 2.0
    assert result.right == 1.0


def test_division():
    speeds = DifferentialDriveWheelSpeeds(left=1.0, right=0.5)

    result = speeds / 2

    assert result.left == 0.5
    assert result.right == 0.25