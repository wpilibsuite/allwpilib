import pytest

from wpimath.kinematics import SwerveModulePosition
from wpimath.geometry import Rotation2d
from wpimath.units import meters


def test_equality():
    position1 = SwerveModulePosition(meters(2), Rotation2d.fromDegrees(90))
    position2 = SwerveModulePosition(meters(2), Rotation2d.fromDegrees(90))

    assert position1 == position2


def test_inequality():
    position1 = SwerveModulePosition(meters(1), Rotation2d.fromDegrees(90))
    position2 = SwerveModulePosition(meters(2), Rotation2d.fromDegrees(90))
    position3 = SwerveModulePosition(meters(1), Rotation2d.fromDegrees(89))

    assert position1 != position2
    assert position1 != position3