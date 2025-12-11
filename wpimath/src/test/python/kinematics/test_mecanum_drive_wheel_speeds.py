import pytest

from wpimath.kinematics import MecanumDriveWheelSpeeds


def test_plus():
    left = MecanumDriveWheelSpeeds(
        frontLeft=1.0,
        frontRight=0.5,
        rearLeft=2.0,
        rearRight=1.5,
    )
    right = MecanumDriveWheelSpeeds(
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
    left = MecanumDriveWheelSpeeds(
        frontLeft=1.0,
        frontRight=0.5,
        rearLeft=2.0,
        rearRight=1.5,
    )
    right = MecanumDriveWheelSpeeds(
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
    speeds = MecanumDriveWheelSpeeds(
        frontLeft=1.0,
        frontRight=0.5,
        rearLeft=2.0,
        rearRight=1.5,
    )

    result = -speeds

    assert result.frontLeft == -1.0
    assert result.frontRight == -0.5
    assert result.rearLeft == -2.0
    assert result.rearRight == -1.5


def test_multiplication():
    speeds = MecanumDriveWheelSpeeds(
        frontLeft=1.0,
        frontRight=0.5,
        rearLeft=2.0,
        rearRight=1.5,
    )

    result = speeds * 2

    assert result.frontLeft == 2.0
    assert result.frontRight == 1.0
    assert result.rearLeft == 4.0
    assert result.rearRight == 3.0


def test_division():
    speeds = MecanumDriveWheelSpeeds(
        frontLeft=1.0,
        frontRight=0.5,
        rearLeft=2.0,
        rearRight=1.5,
    )

    result = speeds / 2

    assert result.frontLeft == 0.5
    assert result.frontRight == 0.25
    assert result.rearLeft == 1.0
    assert result.rearRight == 0.75
