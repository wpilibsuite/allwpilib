import pytest

from wpimath import Rotation2d, SwerveModulePosition


def test_equality():
    position1 = SwerveModulePosition(distance=2, angle=Rotation2d.from_degrees(90))
    position2 = SwerveModulePosition(distance=2, angle=Rotation2d.from_degrees(90))

    assert position1 == position2


def test_inequality():
    position1 = SwerveModulePosition(distance=1, angle=Rotation2d.from_degrees(90))
    position2 = SwerveModulePosition(distance=2, angle=Rotation2d.from_degrees(90))
    position3 = SwerveModulePosition(distance=1, angle=Rotation2d.from_degrees(89))

    assert position1 != position2
    assert position1 != position3
