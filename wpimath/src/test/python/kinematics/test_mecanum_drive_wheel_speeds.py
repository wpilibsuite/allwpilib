import pytest

from wpimath.kinematics import MecanumDriveWheelSpeeds
from wpimath.units import meters_per_second


def test_plus():
    left = MecanumDriveWheelSpeeds(
        meters_per_second(1.0),
        meters_per_second(0.5),
        meters_per_second(2.0),
        meters_per_second(1.5),
    )
    right = MecanumDriveWheelSpeeds(
        meters_per_second(2.0),
        meters_per_second(1.5),
        meters_per_second(0.5),
        meters_per_second(1.0),
    )

    result = left + right

    assert result.frontLeft == 3.0
    assert result.frontRight == 2.0
    assert result.rearLeft == 2.5
    assert result.rearRight == 2.5


def test_minus():
    left = MecanumDriveWheelSpeeds(
        meters_per_second(1.0),
        meters_per_second(0.5),
        meters_per_second(2.0),
        meters_per_second(1.5),
    )
    right = MecanumDriveWheelSpeeds(
        meters_per_second(2.0),
        meters_per_second(1.5),
        meters_per_second(0.5),
        meters_per_second(1.0),
    )

    result = left - right

    assert result.frontLeft == -1.0
    assert result.frontRight == -1.0
    assert result.rearLeft == 1.5
    assert result.rearRight == 0.5


def test_unary_minus():
    speeds = MecanumDriveWheelSpeeds(
        meters_per_second(1.0),
        meters_per_second(0.5),
        meters_per_second(2.0),
        meters_per_second(1.5),
    )

    result = -speeds

    assert result.frontLeft == -1.0
    assert result.frontRight == -0.5
    assert result.rearLeft == -2.0
    assert result.rearRight == -1.5


def test_multiplication():
    speeds = MecanumDriveWheelSpeeds(
        meters_per_second(1.0),
        meters_per_second(0.5),
        meters_per_second(2.0),
        meters_per_second(1.5),
    )

    result = speeds * 2

    assert result.frontLeft == 2.0
    assert result.frontRight == 1.0
    assert result.rearLeft == 4.0
    assert result.rearRight == 3.0


def test_division():
    speeds = MecanumDriveWheelSpeeds(
        meters_per_second(1.0),
        meters_per_second(0.5),
        meters_per_second(2.0),
        meters_per_second(1.5),
    )

    result = speeds / 2

    assert result.frontLeft == 0.5
    assert result.frontRight == 0.25
    assert result.rearLeft == 1.0
    assert result.rearRight == 0.75