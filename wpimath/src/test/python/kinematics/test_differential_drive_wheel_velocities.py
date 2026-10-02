import pytest

from wpimath import DifferentialDriveWheelVelocities


def test_plus():
    left = DifferentialDriveWheelVelocities(left=1.0, right=0.5)
    right = DifferentialDriveWheelVelocities(left=2.0, right=1.5)

    result = left + right

    assert result.left == 3.0
    assert result.right == 2.0


def test_minus():
    left = DifferentialDriveWheelVelocities(left=1.0, right=0.5)
    right = DifferentialDriveWheelVelocities(left=2.0, right=0.5)

    result = left - right

    assert result.left == -1.0
    assert result.right == 0.0


def test_unary_minus():
    velocities = DifferentialDriveWheelVelocities(left=1.0, right=0.5)

    result = -velocities

    assert result.left == -1.0
    assert result.right == -0.5


def test_multiplication():
    velocities = DifferentialDriveWheelVelocities(left=1.0, right=0.5)

    result = velocities * 2

    assert result.left == 2.0
    assert result.right == 1.0


def test_division():
    velocities = DifferentialDriveWheelVelocities(left=1.0, right=0.5)

    result = velocities / 2

    assert result.left == 0.5
    assert result.right == 0.25
