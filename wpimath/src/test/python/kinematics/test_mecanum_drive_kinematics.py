import pytest
import math

from wpimath.kinematics import (
    MecanumDriveKinematics,
    ChassisSpeeds,
    MecanumDriveWheelSpeeds,
    MecanumDriveWheelPositions,
)
from wpimath.geometry import Translation2d
from wpimath.units import meters, meters_per_second, radians_per_second, radians


@pytest.fixture
def kinematics_test():
    class MecanumDriveKinematicsTest:
        def __init__(self):
            self.m_fl = Translation2d(meters(12), meters(12))
            self.m_fr = Translation2d(meters(12), meters(-12))
            self.m_bl = Translation2d(meters(-12), meters(12))
            self.m_br = Translation2d(meters(-12), meters(-12))
            self.kinematics = MecanumDriveKinematics(
                self.m_fl, self.m_fr, self.m_bl, self.m_br
            )

    return MecanumDriveKinematicsTest()


def test_straight_line_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(meters_per_second(5), meters_per_second(0), radians_per_second(0))
    module_states = kinematics_test.kinematics.toWheelSpeeds(speeds)

    assert module_states.frontLeft == pytest.approx(5.0, abs=0.1)
    assert module_states.frontRight == pytest.approx(5.0, abs=0.1)
    assert module_states.rearLeft == pytest.approx(5.0, abs=0.1)
    assert module_states.rearRight == pytest.approx(5.0, abs=0.1)


def test_straight_line_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        meters_per_second(5), meters_per_second(5), meters_per_second(5), meters_per_second(5)
    )
    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(5.0, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(0.0, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(0.0, abs=0.1)


def test_straight_line_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        meters(5), meters(5), meters(5), meters(5)
    )
    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(5.0, abs=0.1)
    assert twist.dy == pytest.approx(0.0, abs=0.1)
    assert twist.dtheta == pytest.approx(0.0, abs=0.1)


def test_strafe_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(meters_per_second(0), meters_per_second(4), radians_per_second(0))
    module_states = kinematics_test.kinematics.toWheelSpeeds(speeds)

    assert module_states.frontLeft == pytest.approx(-4.0, abs=0.1)
    assert module_states.frontRight == pytest.approx(4.0, abs=0.1)
    assert module_states.rearLeft == pytest.approx(4.0, abs=0.1)
    assert module_states.rearRight == pytest.approx(-4.0, abs=0.1)


def test_strafe_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        meters_per_second(-5), meters_per_second(5), meters_per_second(5), meters_per_second(-5)
    )
    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(0.0, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(5.0, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(0.0, abs=0.1)


def test_strafe_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        meters(-5), meters(5), meters(5), meters(-5)
    )
    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(0.0, abs=0.1)
    assert twist.dy == pytest.approx(5.0, abs=0.1)
    assert twist.dtheta == pytest.approx(0.0, abs=0.1)


def test_rotation_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(
        meters_per_second(0), meters_per_second(0), radians_per_second(2 * math.pi)
    )
    module_states = kinematics_test.kinematics.toWheelSpeeds(speeds)

    assert module_states.frontLeft == pytest.approx(-150.79644737, abs=0.1)
    assert module_states.frontRight == pytest.approx(150.79644737, abs=0.1)
    assert module_states.rearLeft == pytest.approx(-150.79644737, abs=0.1)
    assert module_states.rearRight == pytest.approx(150.79644737, abs=0.1)


def test_rotation_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        meters_per_second(-150.79644737),
        meters_per_second(150.79644737),
        meters_per_second(-150.79644737),
        meters_per_second(150.79644737),
    )
    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(0.0, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(0.0, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(2 * math.pi, abs=0.1)


def test_rotation_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        meters(-150.79644737), meters(150.79644737), meters(-150.79644737), meters(150.79644737)
    )
    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(0.0, abs=0.1)
    assert twist.dy == pytest.approx(0.0, abs=0.1)
    assert twist.dtheta == pytest.approx(2 * math.pi, abs=0.1)


def test_mixed_rotation_translation_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(meters_per_second(2), meters_per_second(3), radians_per_second(1))
    module_states = kinematics_test.kinematics.toWheelSpeeds(speeds)

    assert module_states.frontLeft == pytest.approx(-25.0, abs=0.1)
    assert module_states.frontRight == pytest.approx(29.0, abs=0.1)
    assert module_states.rearLeft == pytest.approx(-19.0, abs=0.1)
    assert module_states.rearRight == pytest.approx(23.0, abs=0.1)


def test_mixed_rotation_translation_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        meters_per_second(-17.677670),
        meters_per_second(20.506097),
        meters_per_second(-13.435),
        meters_per_second(16.26),
    )

    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(1.41335, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(2.1221, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(0.707, abs=0.1)


def test_mixed_rotation_translation_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        meters(-17.677670), meters(20.506097), meters(-13.435), meters(16.26)
    )

    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(1.41335, abs=0.1)
    assert twist.dy == pytest.approx(2.1221, abs=0.1)
    assert twist.dtheta == pytest.approx(0.707, abs=0.1)


def test_off_center_rotation_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(meters_per_second(0), meters_per_second(0), radians_per_second(1))
    module_states = kinematics_test.kinematics.toWheelSpeeds(speeds, kinematics_test.m_fl)

    assert module_states.frontLeft == pytest.approx(0, abs=0.1)
    assert module_states.frontRight == pytest.approx(24.0, abs=0.1)
    assert module_states.rearLeft == pytest.approx(-24.0, abs=0.1)
    assert module_states.rearRight == pytest.approx(48.0, abs=0.1)


def test_off_center_rotation_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        meters_per_second(0),
        meters_per_second(16.971),
        meters_per_second(-16.971),
        meters_per_second(33.941),
    )
    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(8.48525, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(-8.48525, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(0.707, abs=0.1)


def test_off_center_rotation_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        meters(0), meters(16.971), meters(-16.971), meters(33.941)
    )
    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(8.48525, abs=0.1)
    assert twist.dy == pytest.approx(-8.48525, abs=0.1)
    assert twist.dtheta == pytest.approx(0.707, abs=0.1)


def test_off_center_translation_rotation_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(meters_per_second(5), meters_per_second(2), radians_per_second(1))
    module_states = kinematics_test.kinematics.toWheelSpeeds(speeds, kinematics_test.m_fl)

    assert module_states.frontLeft == pytest.approx(3.0, abs=0.1)
    assert module_states.frontRight == pytest.approx(31.0, abs=0.1)
    assert module_states.rearLeft == pytest.approx(-17.0, abs=0.1)
    assert module_states.rearRight == pytest.approx(51.0, abs=0.1)


def test_off_center_translation_rotation_forward_kinematics(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        meters_per_second(2.12),
        meters_per_second(21.92),
        meters_per_second(-12.02),
        meters_per_second(36.06),
    )
    chassis_speeds = kinematics_test.kinematics.toChassisSpeeds(wheel_speeds)

    assert chassis_speeds.vx == pytest.approx(12.02, abs=0.1)
    assert chassis_speeds.vy == pytest.approx(-7.07, abs=0.1)
    assert chassis_speeds.omega == pytest.approx(0.707, abs=0.1)


def test_off_center_translation_rotation_forward_kinematics_with_deltas(kinematics_test):
    wheel_deltas = MecanumDriveWheelPositions(
        meters(2.12), meters(21.92), meters(-12.02), meters(36.06)
    )
    twist = kinematics_test.kinematics.toTwist2d(wheel_deltas)

    assert twist.dx == pytest.approx(12.02, abs=0.1)
    assert twist.dy == pytest.approx(-7.07, abs=0.1)
    assert twist.dtheta == pytest.approx(0.707, abs=0.1)


def test_desaturate(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        meters_per_second(5), meters_per_second(6), meters_per_second(4), meters_per_second(7)
    ).desaturate(meters_per_second(5.5))

    k_factor = 5.5 / 7.0

    assert wheel_speeds.frontLeft == pytest.approx(5.0 * k_factor, abs=1e-9)
    assert wheel_speeds.frontRight == pytest.approx(6.0 * k_factor, abs=1e-9)
    assert wheel_speeds.rearLeft == pytest.approx(4.0 * k_factor, abs=1e-9)
    assert wheel_speeds.rearRight == pytest.approx(7.0 * k_factor, abs=1e-9)


def test_desaturate_negative_speeds(kinematics_test):
    wheel_speeds = MecanumDriveWheelSpeeds(
        meters_per_second(-5),
        meters_per_second(6),
        meters_per_second(4),
        meters_per_second(-7),
    ).desaturate(meters_per_second(5.5))

    k_factor = 5.5 / 7.0

    assert wheel_speeds.frontLeft == pytest.approx(-5.0 * k_factor, abs=1e-9)
    assert wheel_speeds.frontRight == pytest.approx(6.0 * k_factor, abs=1e-9)
    assert wheel_speeds.rearLeft == pytest.approx(4.0 * k_factor, abs=1e-9)
    assert wheel_speeds.rearRight == pytest.approx(-7.0 * k_factor, abs=1e-9)