import pytest
import math

from wpimath import (
    MecanumDriveKinematics,
    ChassisVelocities,
    MecanumDriveWheelVelocities,
    MecanumDriveWheelPositions,
    Rotation2d,
    Translation2d,
)


@pytest.fixture
def kinematics_test():
    class MecanumDriveKinematicsTest:
        def __init__(self):
            self.m_fl = Translation2d(x=12, y=12)
            self.m_fr = Translation2d(x=12, y=-12)
            self.m_bl = Translation2d(x=-12, y=12)
            self.m_br = Translation2d(x=-12, y=-12)
            self.kinematics = MecanumDriveKinematics(
                self.m_fl, self.m_fr, self.m_bl, self.m_br
            )

    return MecanumDriveKinematicsTest()


def test_straight_line_inverse_kinematics(kinematics_test):
    velocities = ChassisVelocities(vx=5, vy=0, omega=0)
    wheel_velocities = kinematics_test.kinematics.to_wheel_velocities(velocities)

    assert wheel_velocities.front_left == pytest.approx(5.0, abs=0.1)
    assert wheel_velocities.front_right == pytest.approx(5.0, abs=0.1)
    assert wheel_velocities.rear_left == pytest.approx(5.0, abs=0.1)
    assert wheel_velocities.rear_right == pytest.approx(5.0, abs=0.1)


def test_straight_line_forward_kinematics(kinematics_test):
    wheel_velocities = MecanumDriveWheelVelocities(
        front_left=5, front_right=5, rear_left=5, rear_right=5
    )
    chassis_velocities = kinematics_test.kinematics.to_chassis_velocities(
        wheel_velocities
    )

    assert chassis_velocities.vx == pytest.approx(5.0, abs=0.1)
    assert chassis_velocities.vy == pytest.approx(0.0, abs=0.1)
    assert chassis_velocities.omega == pytest.approx(0.0, abs=0.1)


def test_straight_line_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        front_left=5, front_right=5, rear_left=5, rear_right=5
    )
    twist = kinematics_test.kinematics.to_twist2d(wheel_deltas)

    assert twist.dx == pytest.approx(5.0, abs=0.1)
    assert twist.dy == pytest.approx(0.0, abs=0.1)
    assert twist.dtheta == pytest.approx(0.0, abs=0.1)


def test_strafe_inverse_kinematics(kinematics_test):
    velocities = ChassisVelocities(vx=0, vy=4, omega=0)
    wheel_velocities = kinematics_test.kinematics.to_wheel_velocities(velocities)

    assert wheel_velocities.front_left == pytest.approx(-4.0, abs=0.1)
    assert wheel_velocities.front_right == pytest.approx(4.0, abs=0.1)
    assert wheel_velocities.rear_left == pytest.approx(4.0, abs=0.1)
    assert wheel_velocities.rear_right == pytest.approx(-4.0, abs=0.1)


def test_strafe_forward_kinematics(kinematics_test):
    wheel_velocities = MecanumDriveWheelVelocities(
        front_left=-5, front_right=5, rear_left=5, rear_right=-5
    )
    chassis_velocities = kinematics_test.kinematics.to_chassis_velocities(
        wheel_velocities
    )

    assert chassis_velocities.vx == pytest.approx(0.0, abs=0.1)
    assert chassis_velocities.vy == pytest.approx(5.0, abs=0.1)
    assert chassis_velocities.omega == pytest.approx(0.0, abs=0.1)


def test_strafe_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        front_left=-5, front_right=5, rear_left=5, rear_right=-5
    )
    twist = kinematics_test.kinematics.to_twist2d(wheel_deltas)

    assert twist.dx == pytest.approx(0.0, abs=0.1)
    assert twist.dy == pytest.approx(5.0, abs=0.1)
    assert twist.dtheta == pytest.approx(0.0, abs=0.1)


def test_rotation_inverse_kinematics(kinematics_test):
    velocities = ChassisVelocities(vx=0, vy=0, omega=2 * math.pi)
    wheel_velocities = kinematics_test.kinematics.to_wheel_velocities(velocities)

    assert wheel_velocities.front_left == pytest.approx(-150.79644737, abs=0.1)
    assert wheel_velocities.front_right == pytest.approx(150.79644737, abs=0.1)
    assert wheel_velocities.rear_left == pytest.approx(-150.79644737, abs=0.1)
    assert wheel_velocities.rear_right == pytest.approx(150.79644737, abs=0.1)


def test_rotation_forward_kinematics(kinematics_test):
    wheel_velocities = MecanumDriveWheelVelocities(
        front_left=-150.79644737,
        front_right=150.79644737,
        rear_left=-150.79644737,
        rear_right=150.79644737,
    )
    chassis_velocities = kinematics_test.kinematics.to_chassis_velocities(
        wheel_velocities
    )

    assert chassis_velocities.vx == pytest.approx(0.0, abs=0.1)
    assert chassis_velocities.vy == pytest.approx(0.0, abs=0.1)
    assert chassis_velocities.omega == pytest.approx(2 * math.pi, abs=0.1)


def test_rotation_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        front_left=-150.79644737,
        front_right=150.79644737,
        rear_left=-150.79644737,
        rear_right=150.79644737,
    )
    twist = kinematics_test.kinematics.to_twist2d(wheel_deltas)

    assert twist.dx == pytest.approx(0.0, abs=0.1)
    assert twist.dy == pytest.approx(0.0, abs=0.1)
    assert twist.dtheta == pytest.approx(2 * math.pi, abs=0.1)


def test_mixed_rotation_translation_inverse_kinematics(kinematics_test):
    velocities = ChassisVelocities(vx=2, vy=3, omega=1)
    wheel_velocities = kinematics_test.kinematics.to_wheel_velocities(velocities)

    assert wheel_velocities.front_left == pytest.approx(-25.0, abs=0.1)
    assert wheel_velocities.front_right == pytest.approx(29.0, abs=0.1)
    assert wheel_velocities.rear_left == pytest.approx(-19.0, abs=0.1)
    assert wheel_velocities.rear_right == pytest.approx(23.0, abs=0.1)


def test_mixed_rotation_translation_forward_kinematics(kinematics_test):
    wheel_velocities = MecanumDriveWheelVelocities(
        front_left=-17.677670,
        front_right=20.506097,
        rear_left=-13.435,
        rear_right=16.26,
    )

    chassis_velocities = kinematics_test.kinematics.to_chassis_velocities(
        wheel_velocities
    )

    assert chassis_velocities.vx == pytest.approx(1.41335, abs=0.1)
    assert chassis_velocities.vy == pytest.approx(2.1221, abs=0.1)
    assert chassis_velocities.omega == pytest.approx(0.707, abs=0.1)


def test_mixed_rotation_translation_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        front_left=-17.677670,
        front_right=20.506097,
        rear_left=-13.435,
        rear_right=16.26,
    )

    twist = kinematics_test.kinematics.to_twist2d(wheel_deltas)

    assert twist.dx == pytest.approx(1.41335, abs=0.1)
    assert twist.dy == pytest.approx(2.1221, abs=0.1)
    assert twist.dtheta == pytest.approx(0.707, abs=0.1)


def test_off_center_rotation_inverse_kinematics(kinematics_test):
    velocities = ChassisVelocities(vx=0, vy=0, omega=1)
    wheel_velocities = kinematics_test.kinematics.to_wheel_velocities(
        velocities, kinematics_test.m_fl
    )

    assert wheel_velocities.front_left == pytest.approx(0, abs=0.1)
    assert wheel_velocities.front_right == pytest.approx(24.0, abs=0.1)
    assert wheel_velocities.rear_left == pytest.approx(-24.0, abs=0.1)
    assert wheel_velocities.rear_right == pytest.approx(48.0, abs=0.1)


def test_off_center_rotation_forward_kinematics(kinematics_test):
    wheel_velocities = MecanumDriveWheelVelocities(
        front_left=0,
        front_right=16.971,
        rear_left=-16.971,
        rear_right=33.941,
    )
    chassis_velocities = kinematics_test.kinematics.to_chassis_velocities(
        wheel_velocities
    )

    assert chassis_velocities.vx == pytest.approx(8.48525, abs=0.1)
    assert chassis_velocities.vy == pytest.approx(-8.48525, abs=0.1)
    assert chassis_velocities.omega == pytest.approx(0.707, abs=0.1)


def test_off_center_rotation_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        front_left=0, front_right=16.971, rear_left=-16.971, rear_right=33.941
    )
    twist = kinematics_test.kinematics.to_twist2d(wheel_deltas)

    assert twist.dx == pytest.approx(8.48525, abs=0.1)
    assert twist.dy == pytest.approx(-8.48525, abs=0.1)
    assert twist.dtheta == pytest.approx(0.707, abs=0.1)


def test_off_center_translation_rotation_inverse_kinematics(kinematics_test):
    velocities = ChassisVelocities(vx=5, vy=2, omega=1)
    wheel_velocities = kinematics_test.kinematics.to_wheel_velocities(
        velocities, kinematics_test.m_fl
    )

    assert wheel_velocities.front_left == pytest.approx(3.0, abs=0.1)
    assert wheel_velocities.front_right == pytest.approx(31.0, abs=0.1)
    assert wheel_velocities.rear_left == pytest.approx(-17.0, abs=0.1)
    assert wheel_velocities.rear_right == pytest.approx(51.0, abs=0.1)


def test_off_center_translation_rotation_forward_kinematics(kinematics_test):
    wheel_velocities = MecanumDriveWheelVelocities(
        front_left=2.12,
        front_right=21.92,
        rear_left=-12.02,
        rear_right=36.06,
    )
    chassis_velocities = kinematics_test.kinematics.to_chassis_velocities(
        wheel_velocities
    )

    assert chassis_velocities.vx == pytest.approx(12.02, abs=0.1)
    assert chassis_velocities.vy == pytest.approx(-7.07, abs=0.1)
    assert chassis_velocities.omega == pytest.approx(0.707, abs=0.1)


def test_off_center_translation_rotation_forward_kinematics_with_deltas(
    kinematics_test,
):
    wheel_deltas = MecanumDriveWheelPositions(
        front_left=2.12, front_right=21.92, rear_left=-12.02, rear_right=36.06
    )
    twist = kinematics_test.kinematics.to_twist2d(wheel_deltas)

    assert twist.dx == pytest.approx(12.02, abs=0.1)
    assert twist.dy == pytest.approx(-7.07, abs=0.1)
    assert twist.dtheta == pytest.approx(0.707, abs=0.1)


def test_desaturate(kinematics_test):
    wheel_velocities = MecanumDriveWheelVelocities(
        front_left=5, front_right=6, rear_left=4, rear_right=7
    ).desaturate(5.5)

    k_factor = 5.5 / 7.0

    assert wheel_velocities.front_left == pytest.approx(5.0 * k_factor, abs=1e-9)
    assert wheel_velocities.front_right == pytest.approx(6.0 * k_factor, abs=1e-9)
    assert wheel_velocities.rear_left == pytest.approx(4.0 * k_factor, abs=1e-9)
    assert wheel_velocities.rear_right == pytest.approx(7.0 * k_factor, abs=1e-9)


def test_desaturate_negative_velocities(kinematics_test):
    wheel_velocities = MecanumDriveWheelVelocities(
        front_left=-5,
        front_right=6,
        rear_left=4,
        rear_right=-7,
    ).desaturate(5.5)

    k_factor = 5.5 / 7.0

    assert wheel_velocities.front_left == pytest.approx(-5.0 * k_factor, abs=1e-9)
    assert wheel_velocities.front_right == pytest.approx(6.0 * k_factor, abs=1e-9)
    assert wheel_velocities.rear_left == pytest.approx(4.0 * k_factor, abs=1e-9)
    assert wheel_velocities.rear_right == pytest.approx(-7.0 * k_factor, abs=1e-9)
