import pytest

from wpimath.geometry import Pose2d, Rotation2d, Translation2d
from wpimath.kinematics import (
    ChassisSpeeds,
    SwerveDrive4Kinematics,
    SwerveDrive4Odometry,
    SwerveModulePosition,
    SwerveModuleState,
)


@pytest.fixture()
def s4():
    fl = Translation2d(12, 12)
    fr = Translation2d(12, -12)
    bl = Translation2d(-12, 12)
    br = Translation2d(-12, -12)
    return SwerveDrive4Kinematics(fl, fr, bl, br)


def test_swerve4_straightline(s4: SwerveDrive4Kinematics):
    chassisSpeeds = ChassisSpeeds(5, 0, 0)

    fl, fr, bl, br = s4.toSwerveModuleStates(chassisSpeeds)
    assert fl.speed == pytest.approx(5.0)
    assert fr.speed == pytest.approx(5.0)
    assert bl.speed == pytest.approx(5.0)
    assert br.speed == pytest.approx(5.0)

    assert fl.angle.radians() == pytest.approx(0.0)
    assert fr.angle.radians() == pytest.approx(0.0)
    assert bl.angle.radians() == pytest.approx(0.0)
    assert br.angle.radians() == pytest.approx(0.0)


def test_swerve4_normalize():
    s1 = SwerveModuleState(5)
    s2 = SwerveModuleState(6)
    s3 = SwerveModuleState(4)
    s4 = SwerveModuleState(7)

    states = SwerveDrive4Kinematics.desaturateWheelSpeeds((s1, s2, s3, s4), 5.5)

    kFactor = 5.5 / 7.0

    assert states[0].speed == pytest.approx(5.0 * kFactor)
    assert states[1].speed == pytest.approx(6.0 * kFactor)
    assert states[2].speed == pytest.approx(4.0 * kFactor)
    assert states[3].speed == pytest.approx(7.0 * kFactor)


def test_swerve4_odometry(s4: SwerveDrive4Kinematics):
    zero = SwerveModulePosition()
    odometry = SwerveDrive4Odometry(s4, Rotation2d(0), (zero, zero, zero, zero))
    odometry.resetPosition(Rotation2d(0), (zero, zero, zero, zero), Pose2d())

    position = SwerveModulePosition(0.5)

    odometry.update(
        Rotation2d(0),
        (
            zero,
            zero,
            zero,
            zero,
        ),
    )

    pose = odometry.update(Rotation2d(0), (position, position, position, position))

    print(pose)
    assert pose.x == pytest.approx(0.5)
    assert pose.y == pytest.approx(0.0)
    assert pose.rotation().degrees() == pytest.approx(0)
