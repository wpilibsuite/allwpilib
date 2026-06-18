import pytest
import math
import random

from wpimath import (
    ChassisVelocities,
    Pose2d,
    Rotation2d,
    SwerveDrive4Kinematics,
    SwerveDrive4Odometry,
    SwerveModuleVelocity,
    SwerveModulePosition,
    TrajectoryGenerator,
    TrajectoryConfig,
    Translation2d,
)

kEpsilon = 0.01


@pytest.fixture
def odometry_test():
    class SwerveDriveOdometryTest:
        def __init__(self):
            self.m_fl = Translation2d(x=12, y=12)
            self.m_fr = Translation2d(x=12, y=-12)
            self.m_bl = Translation2d(x=-12, y=12)
            self.m_br = Translation2d(x=-12, y=-12)

            self.m_kinematics = SwerveDrive4Kinematics(
                self.m_fl, self.m_fr, self.m_bl, self.m_br
            )
            self.zero = SwerveModulePosition()
            self.m_odometry = SwerveDrive4Odometry(
                self.m_kinematics,
                Rotation2d(0),
                [self.zero, self.zero, self.zero, self.zero],
            )

    return SwerveDriveOdometryTest()


def test_two_iterations(odometry_test):
    position = SwerveModulePosition(distance=0.5, angle=Rotation2d.fromDegrees(0))
    odometry_test.m_odometry.resetPosition(
        Rotation2d(0),
        [
            odometry_test.zero,
            odometry_test.zero,
            odometry_test.zero,
            odometry_test.zero,
        ],
        Pose2d(),
    )

    odometry_test.m_odometry.update(
        Rotation2d(0),
        [
            odometry_test.zero,
            odometry_test.zero,
            odometry_test.zero,
            odometry_test.zero,
        ],
    )

    pose = odometry_test.m_odometry.update(
        Rotation2d(0), [position, position, position, position]
    )

    assert pose.x == pytest.approx(0.5, abs=kEpsilon)
    assert pose.y == pytest.approx(0.0, abs=kEpsilon)
    assert pose.rotation().degrees() == pytest.approx(0.0, abs=kEpsilon)


def test_90_degree_turn(odometry_test):
    fl = SwerveModulePosition(distance=18.85, angle=Rotation2d.fromDegrees(90))
    fr = SwerveModulePosition(distance=42.15, angle=Rotation2d.fromDegrees(26.565))
    bl = SwerveModulePosition(distance=18.85, angle=Rotation2d.fromDegrees(-90))
    br = SwerveModulePosition(distance=42.15, angle=Rotation2d.fromDegrees(-26.565))

    odometry_test.m_odometry.resetPosition(
        Rotation2d(0),
        [
            odometry_test.zero,
            odometry_test.zero,
            odometry_test.zero,
            odometry_test.zero,
        ],
        Pose2d(),
    )
    pose = odometry_test.m_odometry.update(Rotation2d.fromDegrees(90), [fl, fr, bl, br])

    assert pose.x == pytest.approx(12.0, abs=kEpsilon)
    assert pose.y == pytest.approx(12.0, abs=kEpsilon)
    assert pose.rotation().degrees() == pytest.approx(90.0, abs=kEpsilon)


def test_gyro_angle_reset(odometry_test):
    odometry_test.m_odometry.resetPosition(
        Rotation2d.fromDegrees(90),
        [
            odometry_test.zero,
            odometry_test.zero,
            odometry_test.zero,
            odometry_test.zero,
        ],
        Pose2d(),
    )

    position = SwerveModulePosition(distance=0.5, angle=Rotation2d.fromDegrees(0))
    pose = odometry_test.m_odometry.update(
        Rotation2d.fromDegrees(90), [position, position, position, position]
    )

    assert pose.x == pytest.approx(0.5, abs=kEpsilon)
    assert pose.y == pytest.approx(0.0, abs=kEpsilon)
    assert pose.rotation().degrees() == pytest.approx(0.0, abs=kEpsilon)


def test_accuracy_facing_trajectory():
    kinematics = SwerveDrive4Kinematics(
        Translation2d(x=1, y=1),
        Translation2d(x=1, y=-1),
        Translation2d(x=-1, y=-1),
        Translation2d(x=-1, y=1),
    )
    zero = SwerveModulePosition()
    odometry = SwerveDrive4Odometry(kinematics, Rotation2d(), [zero, zero, zero, zero])

    fl = SwerveModulePosition()
    fr = SwerveModulePosition()
    bl = SwerveModulePosition()
    br = SwerveModulePosition()

    trajectory = TrajectoryGenerator.generateTrajectory(
        [
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(45)),
            Pose2d(x=3, y=0, rotation=Rotation2d.fromDegrees(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(135)),
            Pose2d(x=-3, y=0, rotation=Rotation2d.fromDegrees(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(45)),
        ],
        TrajectoryConfig(maxVelocity=5.0, maxAcceleration=2.0),
    )

    random.seed(4915)

    dt = 0.02
    t = 0

    max_error = -float("inf")
    error_sum = 0

    while t < trajectory.totalTime():
        ground_truth_state = trajectory.sample(t)

        module_velocities = kinematics.toSwerveModuleVelocities(
            ChassisVelocities(
                vx=ground_truth_state.velocity,
                vy=0,
                omega=ground_truth_state.velocity * ground_truth_state.curvature,
            )
        )

        fl.distance += module_velocities[0].velocity * dt
        fr.distance += module_velocities[1].velocity * dt
        bl.distance += module_velocities[2].velocity * dt
        br.distance += module_velocities[3].velocity * dt

        fl.angle = module_velocities[0].angle
        fr.angle = module_velocities[1].angle
        bl.angle = module_velocities[2].angle
        br.angle = module_velocities[3].angle

        xhat = odometry.update(
            ground_truth_state.pose.rotation()
            + Rotation2d(random.gauss(0.0, 1.0) * 0.05),
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
        Translation2d(x=1, y=1),
        Translation2d(x=1, y=-1),
        Translation2d(x=-1, y=-1),
        Translation2d(x=-1, y=1),
    )
    zero = SwerveModulePosition()
    odometry = SwerveDrive4Odometry(kinematics, Rotation2d(), [zero, zero, zero, zero])

    fl = SwerveModulePosition()
    fr = SwerveModulePosition()
    bl = SwerveModulePosition()
    br = SwerveModulePosition()

    trajectory = TrajectoryGenerator.generateTrajectory(
        [
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(45)),
            Pose2d(x=3, y=0, rotation=Rotation2d.fromDegrees(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(135)),
            Pose2d(x=-3, y=0, rotation=Rotation2d.fromDegrees(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(45)),
        ],
        TrajectoryConfig(maxVelocity=5.0, maxAcceleration=2.0),
    )

    random.seed(4915)

    dt = 0.02
    t = 0

    max_error = -float("inf")
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
            Rotation2d(random.gauss(0.0, 1.0) * 0.05), [fl, fr, bl, br]
        )
        error = ground_truth_state.pose.translation().distance(xhat.translation())
        if error > max_error:
            max_error = error
        error_sum += error

        t += dt

    assert error_sum / (trajectory.totalTime() / dt) < 0.06
    assert max_error < 0.125
