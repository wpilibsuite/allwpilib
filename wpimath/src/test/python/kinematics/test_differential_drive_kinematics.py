import pytest
import math
import numpy as np

from wpimath.kinematics import (
    ChassisSpeeds,
    DifferentialDriveKinematics,
    DifferentialDriveWheelSpeeds,
)
from wpimath.units import (
    meters,
    meters_per_second,
    radians,
    radians_per_second,
)


def test_inverse_kinematics_from_zero():
    kinematics = DifferentialDriveKinematics(meters(0.381 * 2))
    chassis_speeds = ChassisSpeeds()
    wheel_speeds = kinematics.toWheelSpeeds(chassis_speeds)

    assert wheel_speeds.left == pytest.approx(0, abs=1e-9)
    assert wheel_speeds.right == pytest.approx(0, abs=1e-9)


def test_forward_kinematics_from_zero():
    kinematics = DifferentialDriveKinematics(meters(0.381 * 2))
    wheel_speeds = DifferentialDriveWheelSpeeds()
    chassis_speeds = kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(0, abs=1e-9)
    assert chassis_speeds.vy == pytest.approx(0, abs=1e-9)
    assert chassis_speeds.omega == pytest.approx(0, abs=1e-9)


def test_inverse_kinematics_for_straight_line():
    kinematics = DifferentialDriveKinematics(meters(0.381 * 2))
    chassis_speeds = ChassisSpeeds(meters_per_second(3.0), meters_per_second(0), radians_per_second(0))
    wheel_speeds = kinematics.toWheelSpeeds(chassis_speeds)

    assert wheel_speeds.left == pytest.approx(3, abs=1e-9)
    assert wheel_speeds.right == pytest.approx(3, abs=1e-9)


def test_forward_kinematics_for_straight_line():
    kinematics = DifferentialDriveKinematics(meters(0.381 * 2))
    wheel_speeds = DifferentialDriveWheelSpeeds(meters_per_second(3.0), meters_per_second(3.0))
    chassis_speeds = kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(3, abs=1e-9)
    assert chassis_speeds.vy == pytest.approx(0, abs=1e-9)
    assert chassis_speeds.omega == pytest.approx(0, abs=1e-9)


def test_inverse_kinematics_for_rotate_in_place():
    kinematics = DifferentialDriveKinematics(meters(0.381 * 2))
    chassis_speeds = ChassisSpeeds(
        meters_per_second(0.0), meters_per_second(0.0), radians_per_second(math.pi)
    )
    wheel_speeds = kinematics.toWheelSpeeds(chassis_speeds)

    assert wheel_speeds.left == pytest.approx(-0.381 * math.pi, abs=1e-9)
    assert wheel_speeds.right == pytest.approx(0.381 * math.pi, abs=1e-9)


def test_forward_kinematics_for_rotate_in_place():
    kinematics = DifferentialDriveKinematics(meters(0.381 * 2))
    wheel_speeds = DifferentialDriveWheelSpeeds(
        meters_per_second(0.381 * math.pi), meters_per_second(-0.381 * math.pi)
    )
    chassis_speeds = kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(0, abs=1e-9)
    assert chassis_speeds.vy == pytest.approx(0, abs=1e-9)
    assert chassis_speeds.omega == pytest.approx(-math.pi, abs=1e-9)