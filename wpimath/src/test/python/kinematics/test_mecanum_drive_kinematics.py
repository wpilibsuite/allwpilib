import pytest
import math

from wpimath.kinematics import (
    MecanumDriveKinematics,
    ChassisSpeeds,
    MecanumDriveWheelSpeeds,
    MecanumDriveWheelPositions,
)
from wpimath.geometry import Rotation2d, Translation2d


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
    speeds = ChassisSpeeds(vx=5, vy=0, omega=0)
    module_states = kinematics_test.kinematics.toWheelSpeeds(speeds)

    assert module_states.frontLeft == pytest.approx(5.0, abs=0.1)
    assert module_states.frontRight == pytest.approx(5.0, abs=0.1)
    assert module_states.rearLeft == pytest.approx(5.0, abs=0.1)
    assert module_states.rearRight == pytest.approx(5.0, abs=0.1)


def test_straight_line_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        frontLeft=5, frontRight=5, rearLeft=5, rearRight=5
    )
    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(5.0, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(0.0, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(0.0, abs=0.1)


def test_straight_line_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=5, frontRight=5, rearLeft=5, rearRight=5
    )
    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(5.0, abs=0.1)
    assert twist.dy == pytest.approx(0.0, abs=0.1)
    assert twist.dtheta == pytest.approx(0.0, abs=0.1)


def test_strafe_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(vx=0, vy=4, omega=0)
    module_states = kinematics_test.kinematics.toWheelSpeeds(speeds)

    assert module_states.frontLeft == pytest.approx(-4.0, abs=0.1)
    assert module_states.frontRight == pytest.approx(4.0, abs=0.1)
    assert module_states.rearLeft == pytest.approx(4.0, abs=0.1)
    assert module_states.rearRight == pytest.approx(-4.0, abs=0.1)


def test_strafe_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        frontLeft=-5, frontRight=5, rearLeft=5, rearRight=-5
    )
    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(0.0, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(5.0, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(0.0, abs=0.1)


def test_strafe_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=-5, frontRight=5, rearLeft=5, rearRight=-5
    )
    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(0.0, abs=0.1)
    assert twist.dy == pytest.approx(5.0, abs=0.1)
    assert twist.dtheta == pytest.approx(0.0, abs=0.1)


def test_rotation_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(vx=0, vy=0, omega=2 * math.pi)
    module_states = kinematics_test.kinematics.toWheelSpeeds(speeds)

    assert module_states.frontLeft == pytest.approx(-150.79644737, abs=0.1)
    assert module_states.frontRight == pytest.approx(150.79644737, abs=0.1)
    assert module_states.rearLeft == pytest.approx(-150.79644737, abs=0.1)
    assert module_states.rearRight == pytest.approx(150.79644737, abs=0.1)


def test_rotation_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        frontLeft=-150.79644737,
        frontRight=150.79644737,
        rearLeft=-150.79644737,
        rearRight=150.79644737,
    )
    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(0.0, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(0.0, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(2 * math.pi, abs=0.1)


def test_rotation_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=-150.79644737,
        frontRight=150.79644737,
        rearLeft=-150.79644737,
        rearRight=150.79644737,
    )
    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(0.0, abs=0.1)
    assert twist.dy == pytest.approx(0.0, abs=0.1)
    assert twist.dtheta == pytest.approx(2 * math.pi, abs=0.1)


def test_mixed_rotation_translation_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(vx=2, vy=3, omega=1)
    module_states = kinematics_test.kinematics.toWheelSpeeds(speeds)

    assert module_states.frontLeft == pytest.approx(-25.0, abs=0.1)
    assert module_states.frontRight == pytest.approx(29.0, abs=0.1)
    assert module_states.rearLeft == pytest.approx(-19.0, abs=0.1)
    assert module_states.rearRight == pytest.approx(23.0, abs=0.1)


def test_mixed_rotation_translation_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        frontLeft=-17.677670,
        frontRight=20.506097,
        rearLeft=-13.435,
        rearRight=16.26,
    )

    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(1.41335, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(2.1221, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(0.707, abs=0.1)


def test_mixed_rotation_translation_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=-17.677670, frontRight=20.506097, rearLeft=-13.435, rearRight=16.26
    )

    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(1.41335, abs=0.1)
    assert twist.dy == pytest.approx(2.1221, abs=0.1)
    assert twist.dtheta == pytest.approx(0.707, abs=0.1)


def test_off_center_rotation_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(vx=0, vy=0, omega=1)
    module_states = kinematics_test.kinematics.toWheelSpeeds(
        speeds, kinematics_test.m_fl
    )

    assert module_states.frontLeft == pytest.approx(0, abs=0.1)
    assert module_states.frontRight == pytest.approx(24.0, abs=0.1)
    assert module_states.rearLeft == pytest.approx(-24.0, abs=0.1)
    assert module_states.rearRight == pytest.approx(48.0, abs=0.1)


def test_off_center_rotation_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        frontLeft=0,
        frontRight=16.971,
        rearLeft=-16.971,
        rearRight=33.941,
    )
    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(8.48525, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(-8.48525, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(0.707, abs=0.1)


def test_off_center_rotation_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=0, frontRight=16.971, rearLeft=-16.971, rearRight=33.941
    )
    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(8.48525, abs=0.1)
    assert twist.dy == pytest.approx(-8.48525, abs=0.1)
    assert twist.dtheta == pytest.approx(0.707, abs=0.1)


def test_off_center_translation_rotation_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(vx=5, vy=2, omega=1)
    module_states = kinematics_test.kinematics.toWheelSpeeds(
        speeds, kinematics_test.m_fl
    )

    assert module_states.frontLeft == pytest.approx(3.0, abs=0.1)
    assert module_states.frontRight == pytest.approx(31.0, abs=0.1)
    assert module_states.rearLeft == pytest.approx(-17.0, abs=0.1)
    assert module_states.rearRight == pytest.approx(51.0, abs=0.1)


def test_off_center_translation_rotation_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        frontLeft=2.12,
        frontRight=21.92,
        rearLeft=-12.02,
        rearRight=36.06,
    )
    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(12.02, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(-7.07, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(0.707, abs=0.1)


def test_off_center_translation_rotation_forward_kinematics_with_deltas(
    kinematics_test,
):
    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=2.12, frontRight=21.92, rearLeft=-12.02, rearRight=36.06
    )
    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(12.02, abs=0.1)
    assert twist.dy == pytest.approx(-7.07, abs=0.1)
    assert twist.dtheta == pytest.approx(0.707, abs=0.1)


def test_desaturate(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        frontLeft=5, frontRight=6, rearLeft=4, rearRight=7
    ).desaturate(5.5)

    k_factor = 5.5 / 7.0

    assert wheel_speeds.frontLeft == pytest.approx(5.0 * k_factor, abs=1e-9)
    assert wheel_speeds.frontRight == pytest.approx(6.0 * k_factor, abs=1e-9)
    assert wheel_speeds.rearLeft == pytest.approx(4.0 * k_factor, abs=1e-9)
    assert wheel_speeds.rearRight == pytest.approx(7.0 * k_factor, abs=1e-9)


def test_desaturate_negative_speeds(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        frontLeft=-5,
        frontRight=6,
        rearLeft=4,
        rearRight=-7,
    ).desaturate(5.5)

    k_factor = 5.5 / 7.0

    assert wheel_speeds.frontLeft == pytest.approx(-5.0 * k_factor, abs=1e-9)
    assert wheel_speeds.frontRight == pytest.approx(6.0 * k_factor, abs=1e-9)
    assert wheel_speeds.rearLeft == pytest.approx(4.0 * k_factor, abs=1e-9)
    assert wheel_speeds.rearRight == pytest.approx(-7.0 * k_factor, abs=1e-9)
