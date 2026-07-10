import pytest

from wpimath import (
    TwoDeadWheelOdometry,
    TwoDeadWheelPositions,
    Pose2d,
    Rotation2d,
)

X_WHEEL_Y_POS = 1
Y_WHEEL_X_POS = 1


def _zero_rotation():
    return Rotation2d.fromDegrees(0)


def _zero_pose():
    return Pose2d(x=0, y=0, rotation=_zero_rotation())


def _make_odometry():
    return TwoDeadWheelOdometry(
        X_WHEEL_Y_POS, Y_WHEEL_X_POS, _zero_rotation(), TwoDeadWheelPositions()
    )


def test_multiple_consecutive_updates():
    odometry = _make_odometry()
    wheel_positions = TwoDeadWheelPositions(1, 1)

    odometry.resetPosition(_zero_rotation(), wheel_positions, _zero_pose())

    odometry.update(_zero_rotation(), wheel_positions)
    second_pose = odometry.update(_zero_rotation(), wheel_positions)

    assert second_pose.x == pytest.approx(0.0, abs=0.01)
    assert second_pose.y == pytest.approx(0.0, abs=0.01)
    assert second_pose.rotation().degrees() == pytest.approx(0.0, abs=0.01)


def test_two_iterations():
    odometry = _make_odometry()
    wheel_positions = TwoDeadWheelPositions(0.1, 0)

    odometry.resetPosition(_zero_rotation(), TwoDeadWheelPositions(), _zero_pose())

    odometry.update(_zero_rotation(), TwoDeadWheelPositions())
    pose = odometry.update(_zero_rotation(), wheel_positions)

    assert pose.x == pytest.approx(0.1, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().degrees() == pytest.approx(0.0, abs=0.01)


def test_gyro_angle_reset():
    odometry = _make_odometry()
    gyro = Rotation2d.fromDegrees(90)
    field_angle = _zero_rotation()

    odometry.resetPosition(gyro, TwoDeadWheelPositions(), Pose2d(0, 0, field_angle))

    positions = TwoDeadWheelPositions(1, 0)
    odometry.update(gyro, TwoDeadWheelPositions())
    pose = odometry.update(gyro, positions)

    assert pose.x == pytest.approx(1.0, abs=0.1)
    assert pose.y == pytest.approx(0.0, abs=0.1)
    assert pose.rotation().radians() == pytest.approx(0.0, abs=0.1)


def test_straight_forwards_forward_kinematics():
    odometry = _make_odometry()

    wheel_velocities = odometry.toChassisVelocities(0, 5, 0)

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

    wheel_velocities = odometry.toChassisVelocities(5, -5, 5)

    assert wheel_velocities.vx == pytest.approx(0.0, abs=0.1)
    assert wheel_velocities.vy == pytest.approx(0.0, abs=0.1)
    assert wheel_velocities.omega == pytest.approx(5.0, abs=0.1)


def test_mixed_motion_forward_kinematics():
    odometry = _make_odometry()

    wheel_velocities = odometry.toChassisVelocities(5, 1, -1)

    assert wheel_velocities.vx == pytest.approx(6.0, abs=0.1)
    assert wheel_velocities.vy == pytest.approx(-6.0, abs=0.1)
    assert wheel_velocities.omega == pytest.approx(5.0, abs=0.1)
