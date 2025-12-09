import pytest
import math
import random

from wpimath.kinematics import (
    MecanumDriveKinematics,
    MecanumDriveOdometry,
    MecanumDriveWheelPositions,
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
    radians_per_second
)


@pytest.fixture
def odometry_test():
    class MecanumDriveOdometryTest:
        def __init__(self):
            self.m_fl = Translation2d(meters(12), meters(12))
            self.m_fr = Translation2d(meters(12), meters(-12))
            self.m_bl = Translation2d(meters(-12), meters(12))
            self.m_br = Translation2d(meters(-12), meters(-12))

            self.zero = MecanumDriveWheelPositions()

            self.kinematics = MecanumDriveKinematics(
                self.m_fl, self.m_fr, self.m_bl, self.m_br
            )
            self.odometry = MecanumDriveOdometry(self.kinematics, Rotation2d(0), self.zero)

    return MecanumDriveOdometryTest()


def test_multiple_consecutive_updates(odometry_test):
    wheel_deltas = MecanumDriveWheelPositions(
        meters(3.536), meters(3.536), meters(3.536), meters(3.536)
    )

    odometry_test.odometry.resetPosition(Rotation2d(0), wheel_deltas, Pose2d())
    odometry_test.odometry.update(Rotation2d(0), wheel_deltas)
    second_pose = odometry_test.odometry.update(Rotation2d.fromDegrees(0), wheel_deltas)

    assert second_pose.x == pytest.approx(0.0, abs=0.01)
    assert second_pose.y == pytest.approx(0.0, abs=0.01)
    assert second_pose.rotation().radians() == pytest.approx(0.0, abs=0.01)


def test_two_iterations(odometry_test):
    odometry_test.odometry.resetPosition(Rotation2d(0), odometry_test.zero, Pose2d())
    wheel_deltas = MecanumDriveWheelPositions(
        meters(0.3536), meters(0.3536), meters(0.3536), meters(0.3536)
    )

    odometry_test.odometry.update(Rotation2d(0), MecanumDriveWheelPositions())
    pose = odometry_test.odometry.update(Rotation2d(0), wheel_deltas)

    assert pose.x == pytest.approx(0.3536, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().radians() == pytest.approx(0.0, abs=0.01)


def test_90_degree_turn(odometry_test):
    odometry_test.odometry.resetPosition(Rotation2d(0), odometry_test.zero, Pose2d())
    wheel_deltas = MecanumDriveWheelPositions(
        meters(-13.328), meters(39.986), meters(-13.329), meters(39.986)
    )
    odometry_test.odometry.update(Rotation2d(0), MecanumDriveWheelPositions())
    pose = odometry_test.odometry.update(Rotation2d.fromDegrees(90), wheel_deltas)

    assert pose.x == pytest.approx(8.4855, abs=0.01)
    assert pose.y == pytest.approx(8.4855, abs=0.01)
    assert pose.rotation().degrees() == pytest.approx(90.0, abs=0.01)


def test_gyro_angle_reset(odometry_test):
    odometry_test.odometry.resetPosition(Rotation2d.fromDegrees(90), odometry_test.zero, Pose2d())

    wheel_deltas = MecanumDriveWheelPositions(
        meters(0.3536), meters(0.3536), meters(0.3536), meters(0.3536)
    )

    odometry_test.odometry.update(Rotation2d.fromDegrees(90), MecanumDriveWheelPositions())
    pose = odometry_test.odometry.update(Rotation2d.fromDegrees(90), wheel_deltas)

    assert pose.x == pytest.approx(0.3536, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().radians() == pytest.approx(0.0, abs=0.01)


def test_accuracy_facing_trajectory():
    kinematics = MecanumDriveKinematics(
        Translation2d(meters(1), meters(1)),
        Translation2d(meters(1), meters(-1)),
        Translation2d(meters(-1), meters(-1)),
        Translation2d(meters(-1), meters(1)),
    )

    wheel_positions = MecanumDriveWheelPositions()

    odometry = MecanumDriveOdometry(kinematics, Rotation2d(), wheel_positions)

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
            ground_truth_state.pose.rotation()
            + Rotation2d(radians(random.normalvariate(0.0, 1.0) * 0.05)),
            wheel_positions,
        )

        error = ground_truth_state.pose.translation().distance(xhat.translation())
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

    odometry = MecanumDriveOdometry(kinematics, Rotation2d(), wheel_positions)

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
            Rotation2d(radians(random.normalvariate(0.0, 1.0) * 0.05)), wheel_positions
        )

        error = ground_truth_state.pose.translation().distance(xhat.translation())
        if error > max_error:
            max_error = error
        error_sum += error

        t += dt

    assert error_sum / (trajectory.totalTime() / dt) < 0.06
    assert max_error < 0.125