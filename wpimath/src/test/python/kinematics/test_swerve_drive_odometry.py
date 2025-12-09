import pytest
import math
import random

from wpimath.kinematics import (
    SwerveDrive4Kinematics,
    SwerveDrive4Odometry,
    SwerveModuleState,
    SwerveModulePosition,
    ChassisSpeeds,
)
from wpimath.trajectory import TrajectoryGenerator, TrajectoryConfig
from wpimath.geometry import Translation2d, Rotation2d, Pose2d
from wpimath.units import (
    meters,
    radians,
    seconds,
    meters_per_second,
    meters_per_second_squared,
)

kEpsilon = 0.01


@pytest.fixture
def odometry_test():
    class SwerveDriveOdometryTest:
        def __init__(self):
            self.m_fl = Translation2d(meters(12), meters(12))
            self.m_fr = Translation2d(meters(12), meters(-12))
            self.m_bl = Translation2d(meters(-12), meters(12))
            self.m_br = Translation2d(meters(-12), meters(-12))

            self.m_kinematics = SwerveDrive4Kinematics(
                self.m_fl, self.m_fr, self.m_bl, self.m_br
            )
            self.zero = SwerveModulePosition()
            self.m_odometry = SwerveDrive4Odometry(
                self.m_kinematics, Rotation2d(radians(0)), [self.zero, self.zero, self.zero, self.zero]
            )

    return SwerveDriveOdometryTest()


def test_two_iterations(odometry_test):
    position = SwerveModulePosition(meters(0.5), Rotation2d.fromDegrees(0))
    odometry_test.m_odometry.resetPosition(
        Rotation2d(radians(0)), [odometry_test.zero, odometry_test.zero, odometry_test.zero, odometry_test.zero], Pose2d()
    )

    odometry_test.m_odometry.update(Rotation2d(0), [odometry_test.zero, odometry_test.zero, odometry_test.zero, odometry_test.zero])

    pose = odometry_test.m_odometry.update(
        Rotation2d(0), [position, position, position, position]
    )

    assert pose.x == pytest.approx(0.5, abs=kEpsilon)
    assert pose.y == pytest.approx(0.0, abs=kEpsilon)
    assert pose.rotation().degrees() == pytest.approx(0.0, abs=kEpsilon)


def test_90_degree_turn(odometry_test):
    fl = SwerveModulePosition(meters(18.85), Rotation2d.fromDegrees(90))
    fr = SwerveModulePosition(meters(42.15), Rotation2d.fromDegrees(26.565))
    bl = SwerveModulePosition(meters(18.85), Rotation2d.fromDegrees(-90))
    br = SwerveModulePosition(meters(42.15), Rotation2d.fromDegrees(-26.565))

    odometry_test.m_odometry.resetPosition(
        Rotation2d(radians(0)), [odometry_test.zero, odometry_test.zero, odometry_test.zero, odometry_test.zero], Pose2d()
    )
    pose = odometry_test.m_odometry.update(Rotation2d.fromDegrees(90), [fl, fr, bl, br])

    assert pose.x == pytest.approx(12.0, abs=kEpsilon)
    assert pose.y == pytest.approx(12.0, abs=kEpsilon)
    assert pose.rotation().degrees() == pytest.approx(90.0, abs=kEpsilon)


def test_gyro_angle_reset(odometry_test):
    odometry_test.m_odometry.resetPosition(
        Rotation2d.fromDegrees(90), [odometry_test.zero, odometry_test.zero, odometry_test.zero, odometry_test.zero], Pose2d()
    )

    position = SwerveModulePosition(meters(0.5), Rotation2d.fromDegrees(0))
    pose = odometry_test.m_odometry.update(
        Rotation2d.fromDegrees(90), [position, position, position, position]
    )

    assert pose.x == pytest.approx(0.5, abs=kEpsilon)
    assert pose.y == pytest.approx(0.0, abs=kEpsilon)
    assert pose.rotation().degrees() == pytest.approx(0.0, abs=kEpsilon)


def test_accuracy_facing_trajectory():
    kinematics = SwerveDrive4Kinematics(
        Translation2d(meters(1), meters(1)),
        Translation2d(meters(1), meters(-1)),
        Translation2d(meters(-1), meters(-1)),
        Translation2d(meters(-1), meters(1)),
    )
    zero = SwerveModulePosition()
    odometry = SwerveDrive4Odometry(
        kinematics, Rotation2d(), [zero, zero, zero, zero]
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

        module_states = kinematics.toSwerveModuleStates(
            ChassisSpeeds(
                ground_truth_state.velocity,
                meters_per_second(0),
                ground_truth_state.velocity * ground_truth_state.curvature,
            )
        )

        fl.distance += module_states[0].speed * dt
        fr.distance += module_states[1].speed * dt
        bl.distance += module_states[2].speed * dt
        br.distance += module_states[3].speed * dt

        fl.angle = module_states[0].angle
        fr.angle = module_states[1].angle
        bl.angle = module_states[2].angle
        br.angle = module_states[3].angle

        xhat = odometry.update(
            ground_truth_state.pose.rotation()
            + Rotation2d(radians(random.normalvariate(0.0, 1.0) * 0.05)),
            [fl, fr, bl, br],
        )

        error = ground_truth_state.pose.translation().distance(xhat.translation())
        if error > max_error:
            max_error = error
        error_sum += error

        t += dt

    assert error_sum / (trajectory.totalTime() / dt) < 0.05
    assert max_error < 0.125


def test_accuracy_facing_x_axis():
    kinematics = SwerveDrive4Kinematics(
        Translation2d(meters(1), meters(1)),
        Translation2d(meters(1), meters(-1)),
        Translation2d(meters(-1), meters(-1)),
        Translation2d(meters(-1), meters(1)),
    )
    zero = SwerveModulePosition()
    odometry = SwerveDrive4Odometry(
        kinematics, Rotation2d(), [zero, zero, zero, zero]
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
            Rotation2d(radians(random.normalvariate(0.0, 1.0) * 0.05)), [fl, fr, bl, br]
        )
        error = ground_truth_state.pose.translation().distance(xhat.translation())
        if error > max_error:
            max_error = error
        error_sum += error

        t += dt

    assert error_sum / (trajectory.totalTime() / dt) < 0.06
    assert max_error < 0.125