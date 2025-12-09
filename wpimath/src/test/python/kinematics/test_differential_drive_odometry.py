import pytest
import math

from wpimath.kinematics import DifferentialDriveOdometry
from wpimath.geometry import Rotation2d


def test_encoder_distances():
    odometry = DifferentialDriveOdometry(gyroAngle=Rotation2d.fromDegrees(45), leftDistance=0, rightDistance=0)

    pose = odometry.update(gyroAngle=Rotation2d.fromDegrees(135), rightDistance=0, leftDistance=5 * math.pi)

    assert pose.x == pytest.approx(5.0, abs=1e-9)
    assert pose.y == pytest.approx(5.0, abs=1e-9)
    assert pose.rotation().degrees() == pytest.approx(90.0, abs=1e-9)