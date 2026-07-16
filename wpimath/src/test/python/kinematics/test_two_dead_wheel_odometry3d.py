import math

import pytest

from wpimath import (
    TwoDeadWheelOdometry3d,
    TwoDeadWheelPositions,
    Pose3d,
    Rotation3d,
)

X_WHEEL_Y_POS = 1
Y_WHEEL_X_POS = 1


def _zero_rotation():
    return Rotation3d.from_degrees(0, 0, 0)


def _zero_pose():
    return Pose3d(x=0, y=0, z=0, rotation=_zero_rotation())


def _make_odometry():
    return TwoDeadWheelOdometry3d(
        X_WHEEL_Y_POS, Y_WHEEL_X_POS, _zero_rotation(), TwoDeadWheelPositions()
    )


def test_multiple_consecutive_updates():
    odometry = _make_odometry()
    wheel_positions = TwoDeadWheelPositions(1, 1)

    odometry.reset_position(_zero_rotation(), wheel_positions, _zero_pose())

    odometry.update(_zero_rotation(), wheel_positions)
    second_pose = odometry.update(_zero_rotation(), wheel_positions)

    assert second_pose.x == pytest.approx(0.0, abs=0.01)
    assert second_pose.y == pytest.approx(0.0, abs=0.01)
    assert second_pose.z == pytest.approx(0.0, abs=0.01)
    assert second_pose.rotation().to_rotation2d().degrees() == pytest.approx(
        0.0, abs=0.01
    )


def test_two_iterations():
    odometry = _make_odometry()
    wheel_positions = TwoDeadWheelPositions(0.1, 0)

    odometry.reset_position(_zero_rotation(), TwoDeadWheelPositions(), _zero_pose())

    odometry.update(_zero_rotation(), TwoDeadWheelPositions())
    pose = odometry.update(_zero_rotation(), wheel_positions)

    assert pose.x == pytest.approx(0.1, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.z == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().to_rotation2d().degrees() == pytest.approx(0.0, abs=0.01)


def test_gyro_angle_reset():
    odometry = _make_odometry()
    gyro = Rotation3d.from_degrees(0, 0, 90)
    field_angle = _zero_rotation()

    odometry.reset_position(gyro, TwoDeadWheelPositions(), Pose3d(0, 0, 0, field_angle))

    positions = TwoDeadWheelPositions(1, 0)
    odometry.update(gyro, TwoDeadWheelPositions())
    pose = odometry.update(gyro, positions)

    assert pose.x == pytest.approx(1.0, abs=0.1)
    assert pose.y == pytest.approx(0.0, abs=0.1)
    assert pose.z == pytest.approx(0.0, abs=0.1)
    assert pose.rotation().to_rotation2d().radians() == pytest.approx(0.0, abs=0.1)


def test_straight_forwards_forward_kinematics():
    odometry = _make_odometry()

    wheel_velocities = odometry.to_chassis_velocities(5, 0, 0)

    assert wheel_velocities.vx == pytest.approx(5.0, abs=0.1)
    assert wheel_velocities.vy == pytest.approx(0.0, abs=0.1)
    assert wheel_velocities.omega == pytest.approx(0.0, abs=0.1)


def test_straight_left_forward_kinematics():
    odometry = _make_odometry()

    wheel_velocities = odometry.to_chassis_velocities(0, 5, 0)

    assert wheel_velocities.vx == pytest.approx(0.0, abs=0.1)
    assert wheel_velocities.vy == pytest.approx(5.0, abs=0.1)
    assert wheel_velocities.omega == pytest.approx(0.0, abs=0.1)


def test_spin_in_place_forward_kinematics():
    odometry = _make_odometry()

    wheel_velocities = odometry.to_chassis_velocities(-5, 5, 5)

    assert wheel_velocities.vx == pytest.approx(0.0, abs=0.1)
    assert wheel_velocities.vy == pytest.approx(0.0, abs=0.1)
    assert wheel_velocities.omega == pytest.approx(5.0, abs=0.1)


def test_mixed_motion_forward_kinematics():
    odometry = _make_odometry()

    wheel_velocities = odometry.to_chassis_velocities(1, -1, 5)

    assert wheel_velocities.vx == pytest.approx(6.0, abs=0.1)
    assert wheel_velocities.vy == pytest.approx(-6.0, abs=0.1)
    assert wheel_velocities.omega == pytest.approx(5.0, abs=0.1)


def test_gyro_offset():
    odometry = _make_odometry()
    wheel_positions = TwoDeadWheelPositions()

    odometry.reset_position(
        Rotation3d.from_degrees(0, 5, 0),
        wheel_positions,
        Pose3d(0, 0, 0, Rotation3d.from_degrees(0, 0, 90)),
    )

    pose = odometry.update(Rotation3d.from_degrees(0, 10, 0), wheel_positions)

    assert pose.x == pytest.approx(0.0, abs=1e-9)
    assert pose.y == pytest.approx(0.0, abs=1e-9)
    assert pose.z == pytest.approx(0.0, abs=1e-9)
    assert pose.rotation().x == pytest.approx(math.radians(0), abs=1e-9)
    assert pose.rotation().y == pytest.approx(math.radians(5), abs=1e-9)
    assert pose.rotation().z == pytest.approx(math.radians(90), abs=1e-9)
