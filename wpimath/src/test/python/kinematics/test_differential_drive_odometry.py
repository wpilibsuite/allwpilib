import pytest
import math

from wpimath import DifferentialDriveOdometry, Rotation2d


def test_encoder_distances():
    odometry = DifferentialDriveOdometry(
        gyro_angle=Rotation2d.from_degrees(45), left_distance=0, right_distance=0
    )

    pose = odometry.update(
        gyro_angle=Rotation2d.from_degrees(135),
        right_distance=0,
        left_distance=5 * math.pi,
    )

    assert pose.x == pytest.approx(5.0, abs=1e-9)
    assert pose.y == pytest.approx(5.0, abs=1e-9)
    assert pose.rotation().degrees() == pytest.approx(90.0, abs=1e-9)
