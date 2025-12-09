import pytest
import math
import random

from wpimath.kinematics import (
    MecanumDriveKinematics,
    MecanumDriveOdometry3d,
    MecanumDriveWheelPositions,
    ChassisSpeeds,
)
from wpimath.trajectory import TrajectoryGenerator, TrajectoryConfig
from wpimath.geometry import Translation2d, Rotation2d, Pose3d, Rotation3d, Pose2d
from wpimath.units import (
    meters,
    radians,
    seconds,
    meters_per_second,
    meters_per_second_squared,
    radians_per_second
)


@pytest.fixture
def odometry3d_test():
    class MecanumDriveOdometry3dTest:
        def __init__(self):
            self.m_fl = Translation2d(meters(12), meters(12))
            self.m_fr = Translation2d(meters(12), meters(-12))
            self.m_bl = Translation2d(meters(-12), meters(12))
            self.m_br = Translation2d(meters(-12), meters(-12))

            self.zero = MecanumDriveWheelPositions()

            self.kinematics = MecanumDriveKinematics(
                self.m_fl, self.m_fr, self.m_bl, self.m_br
            )
            self.odometry = MecanumDriveOdometry3d(self.kinematics, Rotation3d(), self.zero)

    return MecanumDriveOdometry3dTest()


def test_initialize(odometry3d_test):
    odometry = MecanumDriveOdometry3d(
        odometry3d_test.kinematics,
        Rotation3d(),
        odometry3d_test.zero,
        Pose3d(meters(1), meters(2), meters(0), Rotation3d.fromDegrees(0, 0, 45)),
    )

    pose = odometry.getPose()

    assert pose.x == pytest.approx(1, abs=1e-9)
    assert pose.y == pytest.approx(2, abs=1e-9)
    assert pose.z == pytest.approx(0, abs=1e-9)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(45, abs=1e-9)


def test_multiple_consecutive_updates(odometry3d_test):
    wheel_deltas = MecanumDriveWheelPositions(
        meters(3.536), meters(3.536), meters(3.536), meters(3.536)
    )

    odometry3d_test.odometry.resetPosition(Rotation3d(), wheel_deltas, Pose3d())
    odometry3d_test.odometry.update(Rotation3d(), wheel_deltas)
    second_pose = odometry3d_test.odometry.update(Rotation3d(), wheel_deltas)

    assert second_pose.x == pytest.approx(0.0, abs=0.01)
    assert second_pose.y == pytest.approx(0.0, abs=0.01)
    assert second_pose.z == pytest.approx(0.0, abs=0.01)
    assert second_pose.rotation().toRotation2d().radians() == pytest.approx(0.0, abs=0.01)


def test_two_iterations(odometry3d_test):
    odometry3d_test.odometry.resetPosition(Rotation3d(), odometry3d_test.zero, Pose3d())
    wheel_deltas = MecanumDriveWheelPositions(
        meters(0.3536), meters(0.3536), meters(0.3536), meters(0.3536)
    )

    odometry3d_test.odometry.update(Rotation3d(), MecanumDriveWheelPositions())
    pose = odometry3d_test.odometry.update(Rotation3d(), wheel_deltas)

    assert pose.x == pytest.approx(0.3536, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.z == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().toRotation2d().radians() == pytest.approx(0.0, abs=0.01)


def test_90_degree_turn(odometry3d_test):
    odometry3d_test.odometry.resetPosition(Rotation3d(), odometry3d_test.zero, Pose3d())
    wheel_deltas = MecanumDriveWheelPositions(
        meters(-13.328), meters(39.986), meters(-13.329), meters(39.986)
    )
    odometry3d_test.odometry.update(Rotation3d(), MecanumDriveWheelPositions())
    pose = odometry3d_test.odometry.update(Rotation3d.fromDegrees(0, 0, 90), wheel_deltas)

    assert pose.x == pytest.approx(8.4855, abs=0.01)
    assert pose.y == pytest.approx(8.4855, abs=0.01)
    assert pose.z == pytest.approx(0, abs=0.01)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(90.0, abs=0.01)


def test_gyro_angle_reset(odometry3d_test):
    odometry3d_test.odometry.resetPosition(Rotation3d.fromDegrees(0, 0, 90), odometry3d_test.zero, Pose3d())

    wheel_deltas = MecanumDriveWheelPositions(
        meters(0.3536), meters(0.3536), meters(0.3536), meters(0.3536)
    )

    pose = odometry3d_test.odometry.update(Rotation3d.fromDegrees(0, 0, 90), wheel_deltas)

    assert pose.x == pytest.approx(0.3536, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.z == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().toRotation2d().radians() == pytest.approx(0.0, abs=0.01)


def test_accuracy_facing_trajectory():
    kinematics = MecanumDriveKinematics(
        Translation2d(meters(1), meters(1)),
        Translation2d(meters(1), meters(-1)),
        Translation2d(meters(-1), meters(-1)),
        Translation2d(meters(-1), meters(1)),
    )

    wheel_positions = MecanumDriveWheelPositions()

    odometry = MecanumDriveOdometry3d(kinematics, Rotation3d(), wheel_positions)

    trajectory = TrajectoryGenerator.generateTrajectory(
        [
            Pose2d(meters(0), meters(0), math.radians(45)),
            Pose2d(meters(3), meters(0), math.radians(-90)),
            Pose2d(meters(0), meters(0), math.radians(135)),
            Pose2d(meters(-3), meters(0), math.radians(-90)),
            Pose2d(meters(0), meters(0), math.radians(45)),
        ],
        TrajectoryConfig(meters_per_second(5.0), meters_per_second_squared(2.0)),
    )

    dt = seconds(0.02)
    t = seconds(0)

    max_error = -1e10
    error_sum = 0

    while t < trajectory.totalTime():
        ground_truth_state = trajectory.sample(t)

        wheel_speeds = kinematics.toWheelSpeeds(
            ChassisSpeeds(
                ground_truth_state.velocity,
                meters_per_second(0),
                ground_truth_state.velocity * ground_truth_state.curvature,
            )
        )

        wheel_speeds.frontLeft += random.normalvariate(0.0, 1.0) * meters_per_second(0.1)
        wheel_speeds.frontRight += random.normalvariate(0.0, 1.0) * meters_per_second(0.1)
        wheel_speeds.rearLeft += random.normalvariate(0.0, 1.0) * meters_per_second(0.1)
        wheel_speeds.rearRight += random.normalvariate(0.0, 1.0) * meters_per_second(0.1)

        wheel_positions.frontLeft += wheel_speeds.frontLeft * dt
        wheel_positions.frontRight += wheel_speeds.frontRight * dt
        wheel_positions.rearLeft += wheel_speeds.rearLeft * dt
        wheel_positions.rearRight += wheel_speeds.rearRight * dt

        xhat = odometry.update(
            Rotation3d(
                ground_truth_state.pose.rotation()
                + Rotation2d(radians(random.normalvariate(0.0, 1.0) * 0.05))
            ),
            wheel_positions,
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


def test_accuracy_facing_x_axis():
    kinematics = MecanumDriveKinematics(
        Translation2d(meters(1), meters(1)),
        Translation2d(meters(1), meters(-1)),
        Translation2d(meters(-1), meters(-1)),
        Translation2d(meters(-1), meters(1)),
    )

    wheel_positions = MecanumDriveWheelPositions()

    odometry = MecanumDriveOdometry3d(kinematics, Rotation3d(), wheel_positions)

    trajectory = TrajectoryGenerator.generateTrajectory(
        [
            Pose2d(meters(0), meters(0), math.radians(45)),
            Pose2d(meters(3), meters(0), math.radians(-90)),
            Pose2d(meters(0), meters(0), math.radians(135)),
            Pose2d(meters(-3), meters(0), math.radians(-90)),
            Pose2d(meters(0), meters(0), math.radians(45)),
        ],
        TrajectoryConfig(meters_per_second(5.0), meters_per_second_squared(2.0)),
    )

    dt = seconds(0.02)
    t = seconds(0)

    max_error = -1e10
    error_sum = 0

    while t < trajectory.totalTime():
        ground_truth_state = trajectory.sample(t)

        wheel_speeds = kinematics.toWheelSpeeds(
            ChassisSpeeds(
                ground_truth_state.velocity * ground_truth_state.pose.rotation().cos(),
                ground_truth_state.velocity * ground_truth_state.pose.rotation().sin(),
                radians_per_second(0),
            )
        )

        wheel_speeds.frontLeft += random.normalvariate(0.0, 1.0) * meters_per_second(0.1)
        wheel_speeds.frontRight += random.normalvariate(0.0, 1.0) * meters_per_second(0.1)
        wheel_speeds.rearLeft += random.normalvariate(0.0, 1.0) * meters_per_second(0.1)
        wheel_speeds.rearRight += random.normalvariate(0.0, 1.0) * meters_per_second(0.1)

        wheel_positions.frontLeft += wheel_speeds.frontLeft * dt
        wheel_positions.frontRight += wheel_speeds.frontRight * dt
        wheel_positions.rearLeft += wheel_speeds.rearLeft * dt
        wheel_positions.rearRight += wheel_speeds.rearRight * dt

        xhat = odometry.update(
            Rotation3d(radians(0), radians(0), radians(random.normalvariate(0.0, 1.0) * 0.05)),
            wheel_positions,
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