import pytest

from wpimath import MecanumDriveWheelVelocities


def test_plus():
    left = MecanumDriveWheelVelocities(
        front_left=1.0,
        front_right=0.5,
        rear_left=2.0,
        rear_right=1.5,
    )
    right = MecanumDriveWheelVelocities(
        front_left=2.0,
        front_right=1.5,
        rear_left=0.5,
        rear_right=1.0,
    )

    result = left + right

    assert result.front_left == 3.0
    assert result.front_right == 2.0
    assert result.rear_left == 2.5
    assert result.rear_right == 2.5


def test_minus():
    left = MecanumDriveWheelVelocities(
        front_left=1.0,
        front_right=0.5,
        rear_left=2.0,
        rear_right=1.5,
    )
    right = MecanumDriveWheelVelocities(
        front_left=2.0,
        front_right=1.5,
        rear_left=0.5,
        rear_right=1.0,
    )

    result = left - right

    assert result.front_left == -1.0
    assert result.front_right == -1.0
    assert result.rear_left == 1.5
    assert result.rear_right == 0.5


def test_unary_minus():
    velocities = MecanumDriveWheelVelocities(
        front_left=1.0,
        front_right=0.5,
        rear_left=2.0,
        rear_right=1.5,
    )

    result = -velocities

    assert result.front_left == -1.0
    assert result.front_right == -0.5
    assert result.rear_left == -2.0
    assert result.rear_right == -1.5


def test_multiplication():
    velocities = MecanumDriveWheelVelocities(
        front_left=1.0,
        front_right=0.5,
        rear_left=2.0,
        rear_right=1.5,
    )

    result = velocities * 2

    assert result.front_left == 2.0
    assert result.front_right == 1.0
    assert result.rear_left == 4.0
    assert result.rear_right == 3.0


def test_division():
    velocities = MecanumDriveWheelVelocities(
        front_left=1.0,
        front_right=0.5,
        rear_left=2.0,
        rear_right=1.5,
    )

    result = velocities / 2

    assert result.front_left == 0.5
    assert result.front_right == 0.25
    assert result.rear_left == 1.0
    assert result.rear_right == 0.75
