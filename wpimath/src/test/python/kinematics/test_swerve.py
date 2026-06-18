import pytest

from wpimath import (
    Pose2d,
    Rotation2d,
    Translation2d,
    ChassisVelocities,
    SwerveDrive4Kinematics,
    SwerveDrive4Odometry,
    SwerveModulePosition,
    SwerveModuleVelocity,
)


@pytest.fixture()
def s4():
    fl = Translation2d(12, 12)
    fr = Translation2d(12, -12)
    bl = Translation2d(-12, 12)
    br = Translation2d(-12, -12)
    return SwerveDrive4Kinematics(fl, fr, bl, br)


def test_swerve4_straightline(s4: SwerveDrive4Kinematics):
    chassisVelocities = ChassisVelocities(5, 0, 0)

    fl, fr, bl, br = s4.toSwerveModuleVelocities(chassisVelocities)
    assert fl.velocity == pytest.approx(5.0)
    assert fr.velocity == pytest.approx(5.0)
    assert bl.velocity == pytest.approx(5.0)
    assert br.velocity == pytest.approx(5.0)

    assert fl.angle.radians() == pytest.approx(0.0)
    assert fr.angle.radians() == pytest.approx(0.0)
    assert bl.angle.radians() == pytest.approx(0.0)
    assert br.angle.radians() == pytest.approx(0.0)


def test_swerve4_normalize():
    s1 = SwerveModuleVelocity(5)
    s2 = SwerveModuleVelocity(6)
    s3 = SwerveModuleVelocity(4)
    s4 = SwerveModuleVelocity(7)

    states = SwerveDrive4Kinematics.desaturateWheelVelocities((s1, s2, s3, s4), 5.5)

    kFactor = 5.5 / 7.0

    assert states[0].velocity == pytest.approx(5.0 * kFactor)
    assert states[1].velocity == pytest.approx(6.0 * kFactor)
    assert states[2].velocity == pytest.approx(4.0 * kFactor)
    assert states[3].velocity == pytest.approx(7.0 * kFactor)


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
