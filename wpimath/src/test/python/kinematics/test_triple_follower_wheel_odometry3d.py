import math

import pytest

from wpimath import (
    Pose3d,
    Rotation3d,
    TripleFollowerWheelOdometry3d,
    TripleFollowerWheelPositions,
)


X_WHEEL_1_Y_POS = 1
X_WHEEL_2_Y_POS = -1
Y_WHEEL_X_POS = 1


def _zero_rotation():
    return Rotation3d.fromDegrees(0, 0, 0)


def _zero_pose():
    return Pose3d(x=0, y=0, z=0, rotation=_zero_rotation())


def _make_odometry():
    return TripleFollowerWheelOdometry3d(
        X_WHEEL_1_Y_POS,
        X_WHEEL_2_Y_POS,
        Y_WHEEL_X_POS,
        _zero_rotation(),
        TripleFollowerWheelPositions(),
        _zero_pose(),
    )


def test_throw_on_invalid_x_wheel_setup():
    with pytest.raises(ValueError):
        TripleFollowerWheelOdometry3d(
            0,
            0,
            1,
            _zero_rotation(),
            TripleFollowerWheelPositions(),
            _zero_pose(),
        )


def test_multiple_consecutive_updates():
    odometry = _make_odometry()
    wheel_positions = TripleFollowerWheelPositions(1, 1, 1)

    odometry.resetPosition(_zero_rotation(), wheel_positions, _zero_pose())

    odometry.update(_zero_rotation(), wheel_positions)
    second_pose = odometry.update(_zero_rotation(), wheel_positions)

    assert second_pose.x == pytest.approx(0.0, abs=0.01)
    assert second_pose.y == pytest.approx(0.0, abs=0.01)
    assert (
        second_pose.rotation().toRotation2d().degrees() == pytest.approx(0.0, abs=0.01)
    )


def test_two_iterations():
    odometry = _make_odometry()
    wheel_positions = TripleFollowerWheelPositions(0.1, 0.1, 0)

    odometry.resetPosition(_zero_rotation(), TripleFollowerWheelPositions(), _zero_pose())

    odometry.update(_zero_rotation(), TripleFollowerWheelPositions())
    pose = odometry.update(_zero_rotation(), wheel_positions)

    assert pose.x == pytest.approx(0.1, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(0.0, abs=0.01)


def test_gyro_angle_reset():
    odometry = _make_odometry()
    gyro = Rotation3d.fromDegrees(0, 0, 90)
    field_angle = _zero_rotation()

    odometry.resetPosition(
        gyro, TripleFollowerWheelPositions(), Pose3d(0, 0, 0, field_angle)
    )

    positions = TripleFollowerWheelPositions(1, 1, 0)
    odometry.update(gyro, TripleFollowerWheelPositions())
    pose = odometry.update(gyro, positions)

    assert pose.x == pytest.approx(1.0, abs=0.1)
    assert pose.y == pytest.approx(0.0, abs=0.1)
    assert pose.rotation().toRotation2d().radians() == pytest.approx(0.0, abs=0.1)


def test_straight_forwards_forward_kinematics():
    odometry = _make_odometry()

    wheel_velocities = odometry.toChassisVelocities(5, 5, 0)

    assert wheel_velocities.vx == pytest.approx(5.0, abs=0.1)
    assert wheel_velocities.vy == pytest.approx(0.0, abs=0.1)
    assert wheel_velocities.omega == pytest.approx(0.0, abs=0.1)


def test_straight_left_forward_kinematics():
    odometry = _make_odometry()

    wheel_velocities = odometry.toChassisVelocities(0, 0, 5)

    assert wheel_velocities.vx == pytest.approx(0.0, abs=0.1)
    assert wheel_velocities.vy == pytest.approx(5.0, abs=0.1)
    assert wheel_velocities.omega == pytest.approx(0.0, abs=0.1)


def test_spin_in_place_forward_kinematics():
    odometry = _make_odometry()

    wheel_velocities = odometry.toChassisVelocities(5, -5, -5)

    assert wheel_velocities.vx == pytest.approx(0.0, abs=0.1)
    assert wheel_velocities.vy == pytest.approx(0.0, abs=0.1)
    assert wheel_velocities.omega == pytest.approx(-5.0, abs=0.1)


def test_mixed_motion_forward_kinematics():
    odometry = _make_odometry()

    wheel_velocities = odometry.toChassisVelocities(5, 1, -1)

    assert wheel_velocities.vx == pytest.approx(3.0, abs=0.1)
    assert wheel_velocities.vy == pytest.approx(1.0, abs=0.1)
    assert wheel_velocities.omega == pytest.approx(-2.0, abs=0.1)


def test_gyro_offset():
    odometry = _make_odometry()
    wheel_positions = TripleFollowerWheelPositions()

    odometry.resetPosition(
        Rotation3d.fromDegrees(0, 5, 0),
        wheel_positions,
        Pose3d(0, 0, 0, Rotation3d.fromDegrees(0, 0, 90)),
    )

    pose = odometry.update(Rotation3d.fromDegrees(0, 10, 0), wheel_positions)

    assert pose.x == pytest.approx(0.0, abs=1e-9)
    assert pose.y == pytest.approx(0.0, abs=1e-9)
    assert pose.z == pytest.approx(0.0, abs=1e-9)
    assert pose.rotation().x == pytest.approx(math.radians(0), abs=1e-9)
    assert pose.rotation().y == pytest.approx(math.radians(5), abs=1e-9)
    assert pose.rotation().z == pytest.approx(math.radians(90), abs=1e-9)

