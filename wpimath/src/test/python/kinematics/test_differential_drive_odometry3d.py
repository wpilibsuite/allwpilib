import pytest
import math

from wpimath import DifferentialDriveOdometry3d, Rotation3d, Pose3d, Rotation2d


def test_initialize():
    odometry = DifferentialDriveOdometry3d(
        gyroAngle=Rotation3d.fromDegrees(0, 0, 90),
        leftDistance=0,
        rightDistance=0,
        initialPose=Pose3d(x=1, y=2, z=0, rotation=Rotation3d.fromDegrees(0, 0, 45)),
    )

    pose = odometry.getPose()

    assert pose.x == pytest.approx(1, abs=1e-9)
    assert pose.y == pytest.approx(2, abs=1e-9)
    assert pose.z == pytest.approx(0, abs=1e-9)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(45, abs=1e-9)


def test_encoder_distances():
    odometry = DifferentialDriveOdometry3d(
        gyroAngle=Rotation3d.fromDegrees(0, 0, 45), leftDistance=0, rightDistance=0
    )

    pose = odometry.update(Rotation3d.fromDegrees(0, 0, 135), 0, 5 * math.pi)

    assert pose.x == pytest.approx(5.0, abs=1e-9)
    assert pose.y == pytest.approx(5.0, abs=1e-9)
    assert pose.z == pytest.approx(0.0, abs=1e-9)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(90.0, abs=1e-9)
