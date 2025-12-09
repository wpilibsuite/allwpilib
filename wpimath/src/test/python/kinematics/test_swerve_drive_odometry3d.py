import pytest
import math
import random

from wpimath.kinematics import (
    SwerveDrive4Kinematics,
    SwerveDrive4Odometry3d,
    SwerveModulePosition,
    ChassisSpeeds
)
from wpimath.trajectory import TrajectoryGenerator, TrajectoryConfig
from wpimath.geometry import Translation2d, Rotation2d, Rotation3d, Pose3d, Pose2d
from wpimath.units import (
    meters,
    radians,
    seconds,
    meters_per_second,
    meters_per_second_squared,
)

kEpsilon = 0.01


@pytest.fixture
def odometry3d_test():
    class SwerveDriveOdometry3dTest:
        def __init__(self):
            self.m_fl = Translation2d(meters(12), meters(12))
            self.m_fr = Translation2d(meters(12), meters(-12))
            self.m_bl = Translation2d(meters(-12), meters(12))
            self.m_br = Translation2d(meters(-12), meters(-12))
            self.m_kinematics = SwerveDrive4Kinematics(
                self.m_fl, self.m_fr, self.m_bl, self.m_br
            )
            self.zero = SwerveModulePosition()
            self.m_odometry = SwerveDrive4Odometry3d(
                self.m_kinematics, Rotation3d(), [self.zero, self.zero, self.zero, self.zero]
            )

    return SwerveDriveOdometry3dTest()


def test_initialize(odometry3d_test):
    odometry = SwerveDrive4Odometry3d(
        odometry3d_test.m_kinematics,
        Rotation3d(),
        [odometry3d_test.zero, odometry3d_test.zero, odometry3d_test.zero, odometry3d_test.zero],
        Pose3d(meters(1), meters(2), meters(0), Rotation3d.fromDegrees(0, 0, 45)),
    )

    pose = odometry.getPose()

    assert pose.x == pytest.approx(1, abs=kEpsilon)
    assert pose.y == pytest.approx(2, abs=kEpsilon)
    assert pose.z == pytest.approx(0, abs=kEpsilon)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(45, abs=kEpsilon)


def test_two_iterations(odometry3d_test):
    position = SwerveModulePosition(meters(0.5), Rotation2d(0))
    
    odometry3d_test.m_odometry.resetPosition(
        Rotation3d(), [odometry3d_test.zero, odometry3d_test.zero, odometry3d_test.zero, odometry3d_test.zero], Pose3d()
    )

    odometry3d_test.m_odometry.update(
        Rotation3d(), [odometry3d_test.zero, odometry3d_test.zero, odometry3d_test.zero, odometry3d_test.zero]
    )

    pose = odometry3d_test.m_odometry.update(
        Rotation3d(), [position, position, position, position]
    )

    assert pose.x == pytest.approx(0.5, abs=kEpsilon)
    assert pose.y == pytest.approx(0.0, abs=kEpsilon)
    assert pose.z == pytest.approx(0.0, abs=kEpsilon)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(0.0, abs=kEpsilon)


def test_90_degree_turn(odometry3d_test):
    fl = SwerveModulePosition(meters(18.85), Rotation2d.fromDegrees(90))
    fr = SwerveModulePosition(meters(42.15), Rotation2d.fromDegrees(26.565))
    bl = SwerveModulePosition(meters(18.85), Rotation2d.fromDegrees(-90))
    br = SwerveModulePosition(meters(42.15), Rotation2d.fromDegrees(-26.565))

    odometry3d_test.m_odometry.resetPosition(
        Rotation3d(), [odometry3d_test.zero, odometry3d_test.zero, odometry3d_test.zero, odometry3d_test.zero], Pose3d()
    )
    pose = odometry3d_test.m_odometry.update(
        Rotation3d.fromDegrees(0, 0, 90), [fl, fr, bl, br]
    )

    assert pose.x == pytest.approx(12.0, abs=kEpsilon)
    assert pose.y == pytest.approx(12.0, abs=kEpsilon)
    assert pose.z == pytest.approx(0.0, abs=kEpsilon)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(90.0, abs=kEpsilon)


def test_gyro_angle_reset(odometry3d_test):
    odometry3d_test.m_odometry.resetPosition(
        Rotation3d.fromDegrees(0, 0, 90),
        [odometry3d_test.zero, odometry3d_test.zero, odometry3d_test.zero, odometry3d_test.zero],
        Pose3d(),
    )

    position = SwerveModulePosition(meters(0.5), Rotation2d.fromDegrees(0))

    pose = odometry3d_test.m_odometry.update(
        Rotation3d.fromDegrees(0, 0, 90),
        [position, position, position, position],
    )

    assert pose.x == pytest.approx(0.5, abs=kEpsilon)
    assert pose.y == pytest.approx(0.0, abs=kEpsilon)
    assert pose.z == pytest.approx(0.0, abs=kEpsilon)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(0.0, abs=kEpsilon)


def test_accuracy_facing_x_axis():
    kinematics = SwerveDrive4Kinematics(
        Translation2d(meters(1), meters(1)),
        Translation2d(meters(1), meters(-1)),
        Translation2d(meters(-1), meters(-1)),
        Translation2d(meters(-1), meters(1)),
    )
    
    zero = SwerveModulePosition()
    odometry = SwerveDrive4Odometry3d(
        kinematics, Rotation3d(), [zero, zero, zero, zero]
    )

    fl = SwerveModulePosition()
    fr = SwerveModulePosition()
    bl = SwerveModulePosition()
    br = SwerveModulePosition()

    trajectory = TrajectoryGenerator.generateTrajectory(
        [
            Pose2d(meters(0), meters(0), Rotation2d.fromDegrees(45)),
            Pose2d(meters(3), meters(0), Rotation2d.fromDegrees(-90)),
            Pose2d(meters(0), meters(0), Rotation2d.fromDegrees(135)),
            Pose2d(meters(-3), meters(0), Rotation2d.fromDegrees(-90)),
            Pose2d(meters(0), meters(0), Rotation2d.fromDegrees(45)),
        ],
        TrajectoryConfig(meters_per_second(5.0), meters_per_second_squared(2.0)),
    )

    dt = seconds(0.02)
    t = seconds(0)

    max_error = -float('inf')
    error_sum = 0

    while t < trajectory.totalTime():
        ground_truth_state = trajectory.sample(t)

        fl.distance += (
            ground_truth_state.velocity * dt
            + 0.5 * ground_truth_state.acceleration * dt * dt
        )
        fr.distance += (
            ground_truth_state.velocity * dt
            + 0.5 * ground_truth_state.acceleration * dt * dt
        )
        bl.distance += (
            ground_truth_state.velocity * dt
            + 0.5 * ground_truth_state.acceleration * dt * dt
        )
        br.distance += (
            ground_truth_state.velocity * dt
            + 0.5 * ground_truth_state.acceleration * dt * dt
        )

        fl.angle = ground_truth_state.pose.rotation()
        fr.angle = ground_truth_state.pose.rotation()
        bl.angle = ground_truth_state.pose.rotation()
        br.angle = ground_truth_state.pose.rotation()

        xhat = odometry.update(
            Rotation3d(radians(0), radians(0), radians(random.normalvariate(0.0, 1.0) * 0.05)),
            [fl, fr, bl, br],
        )
        error = (
            ground_truth_state.pose.translation()
            .distance(xhat.translation().toTranslation2d())
            
        )

        if error > max_error:
            max_error = error
        error_sum += error

        t += dt

    assert error_sum / (trajectory.totalTime() / dt) < 0.06
    assert max_error < 0.125