import pytest
import math
import numpy as np

from wpimath import (
    ChassisVelocities,
    DifferentialDriveKinematics,
    DifferentialDriveWheelVelocities,
    Rotation2d,
)


def test_inverse_kinematics_from_zero():
    kinematics = DifferentialDriveKinematics(trackwidth=0.381 * 2)
    chassis_velocities = ChassisVelocities()
    wheel_velocities = kinematics.toWheelVelocities(chassis_velocities)

    assert wheel_velocities.left == pytest.approx(0, abs=1e-9)
    assert wheel_velocities.right == pytest.approx(0, abs=1e-9)


def test_forward_kinematics_from_zero():
    kinematics = DifferentialDriveKinematics(trackwidth=0.381 * 2)
    wheel_velocities = DifferentialDriveWheelVelocities()
    chassis_velocities = kinematics.toChassisVelocities(wheel_velocities)

    assert chassis_velocities.vx == pytest.approx(0, abs=1e-9)
    assert chassis_velocities.vy == pytest.approx(0, abs=1e-9)
    assert chassis_velocities.omega == pytest.approx(0, abs=1e-9)


def test_inverse_kinematics_for_straight_line():
    kinematics = DifferentialDriveKinematics(trackwidth=(0.381 * 2))
    chassis_velocities = ChassisVelocities(vx=3.0, vy=0, omega=0)
    wheel_velocities = kinematics.toWheelVelocities(chassis_velocities)

    assert wheel_velocities.left == pytest.approx(3, abs=1e-9)
    assert wheel_velocities.right == pytest.approx(3, abs=1e-9)


def test_forward_kinematics_for_straight_line():
    kinematics = DifferentialDriveKinematics(trackwidth=0.381 * 2)
    wheel_velocities = DifferentialDriveWheelVelocities(left=3.0, right=3.0)
    chassis_velocities = kinematics.toChassisVelocities(wheel_velocities)

    assert chassis_velocities.vx == pytest.approx(3, abs=1e-9)
    assert chassis_velocities.vy == pytest.approx(0, abs=1e-9)
    assert chassis_velocities.omega == pytest.approx(0, abs=1e-9)


def test_inverse_kinematics_for_rotate_in_place():
    kinematics = DifferentialDriveKinematics(trackwidth=0.381 * 2)
    chassis_velocities = ChassisVelocities(vx=0.0, vy=0.0, omega=math.pi)
    wheel_velocities = kinematics.toWheelVelocities(chassis_velocities)

    assert wheel_velocities.left == pytest.approx(-0.381 * math.pi, abs=1e-9)
    assert wheel_velocities.right == pytest.approx(0.381 * math.pi, abs=1e-9)


def test_forward_kinematics_for_rotate_in_place():
    kinematics = DifferentialDriveKinematics(trackwidth=0.381 * 2)
    wheel_velocities = DifferentialDriveWheelVelocities(
        left=0.381 * math.pi, right=-0.381 * math.pi
    )
    chassis_velocities = kinematics.toChassisVelocities(wheel_velocities)

    assert chassis_velocities.vx == pytest.approx(0, abs=1e-9)
    assert chassis_velocities.vy == pytest.approx(0, abs=1e-9)
    assert chassis_velocities.omega == pytest.approx(-math.pi, abs=1e-9)
