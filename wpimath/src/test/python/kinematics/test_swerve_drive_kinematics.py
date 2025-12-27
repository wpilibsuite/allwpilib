import pytest
import math

from wpimath.kinematics import (
    SwerveDrive4Kinematics,
    ChassisSpeeds,
    SwerveModuleState,
    SwerveModulePosition,
)
from wpimath.geometry import Translation2d, Rotation2d

kEpsilon = 0.1


@pytest.fixture
def kinematics_test():
    class SwerveDriveKinematicsTest:
        def __init__(self):
            self.m_fl = Translation2d(x=12, y=12)
            self.m_fr = Translation2d(x=12, y=-12)
            self.m_bl = Translation2d(x=-12, y=12)
            self.m_br = Translation2d(x=-12, y=-12)
            self.m_kinematics = SwerveDrive4Kinematics(
                self.m_fl, self.m_fr, self.m_bl, self.m_br
            )

    return SwerveDriveKinematicsTest()


def test_straight_line_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(vx=5.0, vy=0.0, omega=0.0)
    states = kinematics_test.m_kinematics.toSwerveModuleStates(speeds)

    fl, fr, bl, br = states

    assert fl.speed == pytest.approx(5.0, abs=kEpsilon)
    assert fr.speed == pytest.approx(5.0, abs=kEpsilon)
    assert bl.speed == pytest.approx(5.0, abs=kEpsilon)
    assert br.speed == pytest.approx(5.0, abs=kEpsilon)

    assert fl.angle.radians() == pytest.approx(0.0, abs=kEpsilon)
    assert fr.angle.radians() == pytest.approx(0.0, abs=kEpsilon)
    assert bl.angle.radians() == pytest.approx(0.0, abs=kEpsilon)
    assert br.angle.radians() == pytest.approx(0.0, abs=kEpsilon)


def test_straight_line_forward_kinematics(kinematics_test):
    state = SwerveModuleState(speed=5.0, angle=Rotation2d.fromDegrees(0))
    chassis_speeds = kinematics_test.m_kinematics.toChassisSpeeds(
        (state, state, state, state)
    )

    assert chassis_speeds.vx == pytest.approx(5.0, abs=kEpsilon)
    assert chassis_speeds.vy == pytest.approx(0.0, abs=kEpsilon)
    assert chassis_speeds.omega == pytest.approx(0.0, abs=kEpsilon)


def test_straight_line_forward_kinematics_with_deltas(kinematics_test):
    delta = SwerveModulePosition(distance=5.0, angle=Rotation2d.fromDegrees(0))
    twist = kinematics_test.m_kinematics.toTwist2d((delta, delta, delta, delta))

    assert twist.dx == pytest.approx(5.0, abs=kEpsilon)
    assert twist.dy == pytest.approx(0.0, abs=kEpsilon)
    assert twist.dtheta == pytest.approx(0.0, abs=kEpsilon)


def test_straight_strafe_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(vx=0, vy=5, omega=0)
    states = kinematics_test.m_kinematics.toSwerveModuleStates(speeds)

    fl, fr, bl, br = states

    assert fl.speed == pytest.approx(5.0, abs=kEpsilon)
    assert fr.speed == pytest.approx(5.0, abs=kEpsilon)
    assert bl.speed == pytest.approx(5.0, abs=kEpsilon)
    assert br.speed == pytest.approx(5.0, abs=kEpsilon)

    assert fl.angle.degrees() == pytest.approx(90.0, abs=kEpsilon)
    assert fr.angle.degrees() == pytest.approx(90.0, abs=kEpsilon)
    assert bl.angle.degrees() == pytest.approx(90.0, abs=kEpsilon)
    assert br.angle.degrees() == pytest.approx(90.0, abs=kEpsilon)


def test_straight_strafe_forward_kinematics(kinematics_test):
    state = SwerveModuleState(speed=5, angle=Rotation2d.fromDegrees(90))
    chassis_speeds = kinematics_test.m_kinematics.toChassisSpeeds(
        (state, state, state, state)
    )

    assert chassis_speeds.vx == pytest.approx(0.0, abs=kEpsilon)
    assert chassis_speeds.vy == pytest.approx(5.0, abs=kEpsilon)
    assert chassis_speeds.omega == pytest.approx(0.0, abs=kEpsilon)


def test_straight_strafe_forward_kinematics_with_deltas(kinematics_test):
    delta = SwerveModulePosition(distance=5, angle=Rotation2d.fromDegrees(90))
    twist = kinematics_test.m_kinematics.toTwist2d((delta, delta, delta, delta))

    assert twist.dx == pytest.approx(0.0, abs=kEpsilon)
    assert twist.dy == pytest.approx(5.0, abs=kEpsilon)
    assert twist.dtheta == pytest.approx(0.0, abs=kEpsilon)


def test_turn_in_place_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(vx=0, vy=0, omega=2 * math.pi)
    states = kinematics_test.m_kinematics.toSwerveModuleStates(speeds)

    fl, fr, bl, br = states

    assert fl.speed == pytest.approx(106.63, abs=kEpsilon)
    assert fr.speed == pytest.approx(106.63, abs=kEpsilon)
    assert bl.speed == pytest.approx(106.63, abs=kEpsilon)
    assert br.speed == pytest.approx(106.63, abs=kEpsilon)

    assert fl.angle.degrees() == pytest.approx(135.0, abs=kEpsilon)
    assert fr.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)
    assert bl.angle.degrees() == pytest.approx(-135.0, abs=kEpsilon)
    assert br.angle.degrees() == pytest.approx(-45.0, abs=kEpsilon)


def test_conserve_wheel_angle(kinematics_test):
    speeds = ChassisSpeeds(vx=0, vy=0, omega=2 * math.pi)
    kinematics_test.m_kinematics.toSwerveModuleStates(speeds)
    states = kinematics_test.m_kinematics.toSwerveModuleStates(ChassisSpeeds())

    fl, fr, bl, br = states

    assert fl.speed == pytest.approx(0.0, abs=kEpsilon)
    assert fr.speed == pytest.approx(0.0, abs=kEpsilon)
    assert bl.speed == pytest.approx(0.0, abs=kEpsilon)
    assert br.speed == pytest.approx(0.0, abs=kEpsilon)

    assert fl.angle.degrees() == pytest.approx(135.0, abs=kEpsilon)
    assert fr.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)
    assert bl.angle.degrees() == pytest.approx(-135.0, abs=kEpsilon)
    assert br.angle.degrees() == pytest.approx(-45.0, abs=kEpsilon)


def test_reset_wheel_angle(kinematics_test):
    fl_angle = Rotation2d.fromDegrees(0)
    fr_angle = Rotation2d.fromDegrees(90)
    bl_angle = Rotation2d.fromDegrees(180)
    br_angle = Rotation2d.fromDegrees(270)
    kinematics_test.m_kinematics.resetHeadings((fl_angle, fr_angle, bl_angle, br_angle))
    states = kinematics_test.m_kinematics.toSwerveModuleStates(ChassisSpeeds())

    fl_mod, fr_mod, bl_mod, br_mod = states

    assert fl_mod.speed == pytest.approx(0.0, abs=kEpsilon)
    assert fr_mod.speed == pytest.approx(0.0, abs=kEpsilon)
    assert bl_mod.speed == pytest.approx(0.0, abs=kEpsilon)
    assert br_mod.speed == pytest.approx(0.0, abs=kEpsilon)

    assert fl_mod.angle.degrees() == pytest.approx(0.0, abs=kEpsilon)
    assert fr_mod.angle.degrees() == pytest.approx(90.0, abs=kEpsilon)
    assert bl_mod.angle.degrees() == pytest.approx(180.0, abs=kEpsilon)
    assert br_mod.angle.degrees() == pytest.approx(-90.0, abs=kEpsilon)


def test_turn_in_place_forward_kinematics(kinematics_test):
    fl = SwerveModuleState(speed=106.629, angle=Rotation2d.fromDegrees(135))
    fr = SwerveModuleState(speed=106.629, angle=Rotation2d.fromDegrees(45))
    bl = SwerveModuleState(speed=106.629, angle=Rotation2d.fromDegrees(-135))
    br = SwerveModuleState(speed=106.629, angle=Rotation2d.fromDegrees(-45))

    chassis_speeds = kinematics_test.m_kinematics.toChassisSpeeds((fl, fr, bl, br))

    assert chassis_speeds.vx == pytest.approx(0.0, abs=kEpsilon)
    assert chassis_speeds.vy == pytest.approx(0.0, abs=kEpsilon)
    assert chassis_speeds.omega == pytest.approx(2 * math.pi, abs=kEpsilon)


def test_turn_in_place_forward_kinematics_with_deltas(kinematics_test):
    fl = SwerveModulePosition(distance=106.629, angle=Rotation2d.fromDegrees(135))
    fr = SwerveModulePosition(distance=106.629, angle=Rotation2d.fromDegrees(45))
    bl = SwerveModulePosition(distance=106.629, angle=Rotation2d.fromDegrees(-135))
    br = SwerveModulePosition(distance=106.629, angle=Rotation2d.fromDegrees(-45))

    twist = kinematics_test.m_kinematics.toTwist2d((fl, fr, bl, br))

    assert twist.dx == pytest.approx(0.0, abs=kEpsilon)
    assert twist.dy == pytest.approx(0.0, abs=kEpsilon)
    assert twist.dtheta == pytest.approx(2 * math.pi, abs=kEpsilon)


def test_off_center_cor_rotation_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(0, 0, 2 * math.pi)
    states = kinematics_test.m_kinematics.toSwerveModuleStates(
        speeds, kinematics_test.m_fl
    )

    fl, fr, bl, br = states

    assert fl.speed == pytest.approx(0.0, abs=kEpsilon)
    assert fr.speed == pytest.approx(150.796, abs=kEpsilon)
    assert bl.speed == pytest.approx(150.796, abs=kEpsilon)
    assert br.speed == pytest.approx(213.258, abs=kEpsilon)

    assert fl.angle.degrees() == pytest.approx(0.0, abs=kEpsilon)
    assert fr.angle.degrees() == pytest.approx(0.0, abs=kEpsilon)
    assert bl.angle.degrees() == pytest.approx(-90.0, abs=kEpsilon)
    assert br.angle.degrees() == pytest.approx(-45.0, abs=kEpsilon)


def test_off_center_cor_rotation_forward_kinematics(kinematics_test):
    fl = SwerveModuleState(speed=0.0, angle=Rotation2d.fromDegrees(0))
    fr = SwerveModuleState(speed=150.796, angle=Rotation2d.fromDegrees(0))
    bl = SwerveModuleState(speed=150.796, angle=Rotation2d.fromDegrees(-90))
    br = SwerveModuleState(speed=213.258, angle=Rotation2d.fromDegrees(-45))

    chassis_speeds = kinematics_test.m_kinematics.toChassisSpeeds((fl, fr, bl, br))

    assert chassis_speeds.vx == pytest.approx(75.398, abs=kEpsilon)
    assert chassis_speeds.vy == pytest.approx(-75.398, abs=kEpsilon)
    assert chassis_speeds.omega == pytest.approx(2 * math.pi, abs=kEpsilon)


def test_off_center_cor_rotation_forward_kinematics_with_deltas(kinematics_test):
    fl = SwerveModulePosition(distance=0.0, angle=Rotation2d.fromDegrees(0))
    fr = SwerveModulePosition(distance=150.796, angle=Rotation2d.fromDegrees(0))
    bl = SwerveModulePosition(distance=150.796, angle=Rotation2d.fromDegrees(-90))
    br = SwerveModulePosition(distance=213.258, angle=Rotation2d.fromDegrees(-45))

    twist = kinematics_test.m_kinematics.toTwist2d((fl, fr, bl, br))

    assert twist.dx == pytest.approx(75.398, abs=kEpsilon)
    assert twist.dy == pytest.approx(-75.398, abs=kEpsilon)
    assert twist.dtheta == pytest.approx(2 * math.pi, abs=kEpsilon)


def test_off_center_cor_rotation_and_translation_inverse_kinematics(kinematics_test):
    speeds = ChassisSpeeds(0, 3.0, 1.5)
    states = kinematics_test.m_kinematics.toSwerveModuleStates(
        speeds, Translation2d(x=24, y=0)
    )

    fl, fr, bl, br = states

    assert fl.speed == pytest.approx(23.43, abs=kEpsilon)
    assert fr.speed == pytest.approx(23.43, abs=kEpsilon)
    assert bl.speed == pytest.approx(54.08, abs=kEpsilon)
    assert br.speed == pytest.approx(54.08, abs=kEpsilon)

    assert fl.angle.degrees() == pytest.approx(-140.19, abs=kEpsilon)
    assert fr.angle.degrees() == pytest.approx(-39.81, abs=kEpsilon)
    assert bl.angle.degrees() == pytest.approx(-109.44, abs=kEpsilon)
    assert br.angle.degrees() == pytest.approx(-70.56, abs=kEpsilon)


def test_off_center_cor_rotation_and_translation_forward_kinematics(kinematics_test):
    fl = SwerveModuleState(speed=23.43, angle=Rotation2d.fromDegrees(-140.19))
    fr = SwerveModuleState(speed=23.43, angle=Rotation2d.fromDegrees(-39.81))
    bl = SwerveModuleState(speed=54.08, angle=Rotation2d.fromDegrees(-109.44))
    br = SwerveModuleState(speed=54.08, angle=Rotation2d.fromDegrees(-70.56))

    chassis_speeds = kinematics_test.m_kinematics.toChassisSpeeds((fl, fr, bl, br))

    assert chassis_speeds.vx == pytest.approx(0.0, abs=kEpsilon)
    assert chassis_speeds.vy == pytest.approx(-33.0, abs=kEpsilon)
    assert chassis_speeds.omega == pytest.approx(1.5, abs=kEpsilon)


def test_off_center_cor_rotation_and_translation_forward_kinematics_with_deltas(
    kinematics_test,
):
    fl = SwerveModulePosition(distance=23.43, angle=Rotation2d.fromDegrees(-140.19))
    fr = SwerveModulePosition(distance=23.43, angle=Rotation2d.fromDegrees(-39.81))
    bl = SwerveModulePosition(distance=54.08, angle=Rotation2d.fromDegrees(-109.44))
    br = SwerveModulePosition(distance=54.08, angle=Rotation2d.fromDegrees(-70.56))

    twist = kinematics_test.m_kinematics.toTwist2d((fl, fr, bl, br))

    assert twist.dx == pytest.approx(0.0, abs=kEpsilon)
    assert twist.dy == pytest.approx(-33.0, abs=kEpsilon)
    assert twist.dtheta == pytest.approx(1.5, abs=kEpsilon)


def test_desaturate(kinematics_test):
    state1 = SwerveModuleState(speed=5.0, angle=Rotation2d.fromDegrees(0))
    state2 = SwerveModuleState(speed=6.0, angle=Rotation2d.fromDegrees(0))
    state3 = SwerveModuleState(speed=4.0, angle=Rotation2d.fromDegrees(0))
    state4 = SwerveModuleState(speed=7.0, angle=Rotation2d.fromDegrees(0))

    arr = [state1, state2, state3, state4]
    arr = kinematics_test.m_kinematics.desaturateWheelSpeeds(arr, 5.5)

    k_factor = 5.5 / 7.0

    assert arr[0].speed == pytest.approx(5.0 * k_factor, abs=kEpsilon)
    assert arr[1].speed == pytest.approx(6.0 * k_factor, abs=kEpsilon)
    assert arr[2].speed == pytest.approx(4.0 * k_factor, abs=kEpsilon)
    assert arr[3].speed == pytest.approx(7.0 * k_factor, abs=kEpsilon)


def test_desaturate_smooth(kinematics_test):
    state1 = SwerveModuleState(speed=5.0, angle=Rotation2d(0))
    state2 = SwerveModuleState(speed=6.0, angle=Rotation2d(0))
    state3 = SwerveModuleState(speed=4.0, angle=Rotation2d(0))
    state4 = SwerveModuleState(speed=7.0, angle=Rotation2d(0))

    arr = [state1, state2, state3, state4]
    chassis_speeds = kinematics_test.m_kinematics.toChassisSpeeds(
        (arr[0], arr[1], arr[2], arr[3])
    )
    arr = kinematics_test.m_kinematics.desaturateWheelSpeeds(
        arr, chassis_speeds, 5.5, 5.5, 3.5
    )

    k_factor = 5.5 / 7.0

    assert arr[0].speed == pytest.approx(5.0 * k_factor, abs=kEpsilon)
    assert arr[1].speed == pytest.approx(6.0 * k_factor, abs=kEpsilon)
    assert arr[2].speed == pytest.approx(4.0 * k_factor, abs=kEpsilon)
    assert arr[3].speed == pytest.approx(7.0 * k_factor, abs=kEpsilon)


def test_desaturate_negative_speed(kinematics_test):
    state1 = SwerveModuleState(speed=1.0, angle=Rotation2d(0))
    state2 = SwerveModuleState(speed=1.0, angle=Rotation2d(0))
    state3 = SwerveModuleState(speed=-2.0, angle=Rotation2d(0))
    state4 = SwerveModuleState(speed=-2.0, angle=Rotation2d(0))

    arr = [state1, state2, state3, state4]
    arr = kinematics_test.m_kinematics.desaturateWheelSpeeds(arr, 1.0)

    assert arr[0].speed == pytest.approx(0.5, abs=kEpsilon)
    assert arr[1].speed == pytest.approx(0.5, abs=kEpsilon)
    assert arr[2].speed == pytest.approx(-1.0, abs=kEpsilon)
    assert arr[3].speed == pytest.approx(-1.0, abs=kEpsilon)
