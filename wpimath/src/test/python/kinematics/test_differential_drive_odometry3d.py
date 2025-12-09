import pytest
import math

from wpimath.kinematics import DifferentialDriveOdometry3d
from wpimath.geometry import Rotation3d, Pose3d, Rotation2d
from wpimath.units import meters


def test_initialize():
    odometry = DifferentialDriveOdometry3d(
        Rotation3d.fromDegrees(0, 0, 90),
        meters(0),
        meters(0),
        Pose3d(meters(1), meters(2), meters(0), Rotation3d.fromDegrees(0, 0, 45))
    )

    pose = odometry.getPose()

    assert pose.x == pytest.approx(1, abs=1e-9)
    assert pose.y == pytest.approx(2, abs=1e-9)
    assert pose.z == pytest.approx(0, abs=1e-9)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(45, abs=1e-9)


def test_encoder_distances():
    odometry = DifferentialDriveOdometry3d(
        Rotation3d.fromDegrees(0, 0, 45), meters(0), meters(0)
    )

    pose = odometry.update(
        Rotation3d.fromDegrees(0, 0, 135), meters(0), meters(5 * math.pi)
    )

    assert pose.x == pytest.approx(5.0, abs=1e-9)
    assert pose.y == pytest.approx(5.0, abs=1e-9)
    assert pose.z == pytest.approx(0.0, abs=1e-9)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(90.0, abs=1e-9)