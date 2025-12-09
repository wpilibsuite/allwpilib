import pytest

from wpimath.kinematics import SwerveModulePosition
from wpimath.geometry import Rotation2d


def test_equality():
    position1 = SwerveModulePosition(distance=2, angle=Rotation2d.fromDegrees(90))
    position2 = SwerveModulePosition(distance=2, angle=Rotation2d.fromDegrees(90))

    assert position1 == position2


def test_inequality():
    position1 = SwerveModulePosition(distance=1, angle=Rotation2d.fromDegrees(90))
    position2 = SwerveModulePosition(distance=2, angle=Rotation2d.fromDegrees(90))
    position3 = SwerveModulePosition(distance=1, angle=Rotation2d.fromDegrees(89))

    assert position1 != position2
    assert position1 != position3