import pytest
import math

from wpimath.kinematics import DifferentialDriveOdometry
from wpimath.geometry import Rotation2d
from wpimath.units import meters


def test_encoder_distances():
    odometry = DifferentialDriveOdometry(Rotation2d.fromDegrees(45), meters(0), meters(0))

    pose = odometry.update(Rotation2d.fromDegrees(135), meters(0), meters(5 * math.pi))

    assert pose.x == pytest.approx(5.0, abs=1e-9)
    assert pose.y == pytest.approx(5.0, abs=1e-9)
    assert pose.rotation().degrees() == pytest.approx(90.0, abs=1e-9)