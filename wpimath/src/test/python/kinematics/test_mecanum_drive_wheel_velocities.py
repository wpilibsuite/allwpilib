import pytest

from wpimath import MecanumDriveWheelVelocities


def test_plus():
    left = MecanumDriveWheelVelocities(
        frontLeft=1.0,
        frontRight=0.5,
        rearLeft=2.0,
        rearRight=1.5,
    )
    right = MecanumDriveWheelVelocities(
        frontLeft=2.0,
        frontRight=1.5,
        rearLeft=0.5,
        rearRight=1.0,
    )

    result = left + right

    assert result.frontLeft == 3.0
    assert result.frontRight == 2.0
    assert result.rearLeft == 2.5
    assert result.rearRight == 2.5


def test_minus():
    left = MecanumDriveWheelVelocities(
        frontLeft=1.0,
        frontRight=0.5,
        rearLeft=2.0,
        rearRight=1.5,
    )
    right = MecanumDriveWheelVelocities(
        frontLeft=2.0,
        frontRight=1.5,
        rearLeft=0.5,
        rearRight=1.0,
    )

    result = left - right

    assert result.frontLeft == -1.0
    assert result.frontRight == -1.0
    assert result.rearLeft == 1.5
    assert result.rearRight == 0.5


def test_unary_minus():
    velocities = MecanumDriveWheelVelocities(
        frontLeft=1.0,
        frontRight=0.5,
        rearLeft=2.0,
        rearRight=1.5,
    )

    result = -velocities

    assert result.frontLeft == -1.0
    assert result.frontRight == -0.5
    assert result.rearLeft == -2.0
    assert result.rearRight == -1.5


def test_multiplication():
    velocities = MecanumDriveWheelVelocities(
        frontLeft=1.0,
        frontRight=0.5,
        rearLeft=2.0,
        rearRight=1.5,
    )

    result = velocities * 2

    assert result.frontLeft == 2.0
    assert result.frontRight == 1.0
    assert result.rearLeft == 4.0
    assert result.rearRight == 3.0


def test_division():
    velocities = MecanumDriveWheelVelocities(
        frontLeft=1.0,
        frontRight=0.5,
        rearLeft=2.0,
        rearRight=1.5,
    )

    result = velocities / 2

    assert result.frontLeft == 0.5
    assert result.frontRight == 0.25
    assert result.rearLeft == 1.0
    assert result.rearRight == 0.75
