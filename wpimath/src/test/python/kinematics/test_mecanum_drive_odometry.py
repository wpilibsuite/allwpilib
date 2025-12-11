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


@pytest.fixture
def odometry_test():
    class MecanumDriveOdometryTest:
        def __init__(self):
            self.m_fl = Translation2d(x=12, y=12)
            self.m_fr = Translation2d(x=12, y=-12)
            self.m_bl = Translation2d(x=-12, y=12)
            self.m_br = Translation2d(x=-12, y=-12)

            self.zero = MecanumDriveWheelPositions()

            self.kinematics = MecanumDriveKinematics(
                self.m_fl, self.m_fr, self.m_bl, self.m_br
            )
            self.odometry = MecanumDriveOdometry(
                self.kinematics, Rotation2d(0), self.zero
            )

    return MecanumDriveOdometryTest()


def test_multiple_consecutive_updates(odometry_test):
    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=3.536, frontRight=3.536, rearLeft=3.536, rearRight=3.536
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
        frontLeft=0.3536, frontRight=0.3536, rearLeft=0.3536, rearRight=0.3536
    )

    odometry_test.odometry.update(Rotation2d(0), MecanumDriveWheelPositions())
    pose = odometry_test.odometry.update(Rotation2d(0), wheel_deltas)

    assert pose.x == pytest.approx(0.3536, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().radians() == pytest.approx(0.0, abs=0.01)


def test_90_degree_turn(odometry_test):
    odometry_test.odometry.resetPosition(Rotation2d(0), odometry_test.zero, Pose2d())
    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=-13.328, frontRight=39.986, rearLeft=-13.329, rearRight=39.986
    )
    odometry_test.odometry.update(Rotation2d(0), MecanumDriveWheelPositions())
    pose = odometry_test.odometry.update(Rotation2d.fromDegrees(90), wheel_deltas)

    assert pose.x == pytest.approx(8.4855, abs=0.01)
    assert pose.y == pytest.approx(8.4855, abs=0.01)
    assert pose.rotation().degrees() == pytest.approx(90.0, abs=0.01)


def test_gyro_angle_reset(odometry_test):
    odometry_test.odometry.resetPosition(
        Rotation2d.fromDegrees(90), odometry_test.zero, Pose2d()
    )

    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=0.3536, frontRight=0.3536, rearLeft=0.3536, rearRight=0.3536
    )

    odometry_test.odometry.update(
        Rotation2d.fromDegrees(90), MecanumDriveWheelPositions()
    )
    pose = odometry_test.odometry.update(Rotation2d.fromDegrees(90), wheel_deltas)

    assert pose.x == pytest.approx(0.3536, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().radians() == pytest.approx(0.0, abs=0.01)


def test_accuracy_facing_trajectory():
    kinematics = MecanumDriveKinematics(
        Translation2d(x=1, y=1),
        Translation2d(x=1, y=-1),
        Translation2d(x=-1, y=-1),
        Translation2d(x=-1, y=1),
    )

    wheel_positions = MecanumDriveWheelPositions()

    odometry = MecanumDriveOdometry(kinematics, Rotation2d(), wheel_positions)

    trajectory = TrajectoryGenerator.generateTrajectory(
        [
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(45)),
            Pose2d(x=3, y=0, rotation=Rotation2d.fromDegrees(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(135)),
            Pose2d(x=-3, y=0, rotation=Rotation2d.fromDegrees(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(45)),
        ],
        TrajectoryConfig(maxVelocity=0.5, maxAcceleration=2.0),
    )

    random.seed(4915)

    dt = 0.02
    t = 0

    max_error = -1e10
    error_sum = 0

    while t < trajectory.totalTime():
        ground_truth_state = trajectory.sample(t)

        wheel_speeds = kinematics.toWheelSpeeds(
            ChassisSpeeds(
                ground_truth_state.velocity,
                0,
                ground_truth_state.velocity * ground_truth_state.curvature,
            )
        )

        wheel_speeds.frontLeft += random.gauss(0.0, 1.0) * 0.1
        wheel_speeds.frontRight += random.gauss(0.0, 1.0) * 0.1
        wheel_speeds.rearLeft += random.gauss(0.0, 1.0) * 0.1
        wheel_speeds.rearRight += random.gauss(0.0, 1.0) * 0.1

        wheel_positions.frontLeft += wheel_speeds.frontLeft * dt
        wheel_positions.frontRight += wheel_speeds.frontRight * dt
        wheel_positions.rearLeft += wheel_speeds.rearLeft * dt
        wheel_positions.rearRight += wheel_speeds.rearRight * dt

        xhat = odometry.update(
            ground_truth_state.pose.rotation()
            + Rotation2d(random.gauss(0.0, 1.0) * 0.05),
            wheel_positions,
        )

        error = ground_truth_state.pose.translation().distance(xhat.translation())
        if error > max_error:
            max_error = error
        error_sum += error

        t += dt

    assert error_sum / (trajectory.totalTime() / dt) < 0.35
    assert max_error < 0.35


def test_accuracy_facing_x_axis():
    kinematics = MecanumDriveKinematics(
        Translation2d(x=1, y=1),
        Translation2d(x=1, y=-1),
        Translation2d(x=-1, y=-1),
        Translation2d(x=-1, y=1),
    )

    wheel_positions = MecanumDriveWheelPositions()

    odometry = MecanumDriveOdometry(kinematics, Rotation2d(), wheel_positions)

    trajectory = TrajectoryGenerator.generateTrajectory(
        [
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(45)),
            Pose2d(x=3, y=0, rotation=Rotation2d.fromDegrees(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(135)),
            Pose2d(x=-3, y=0, rotation=Rotation2d.fromDegrees(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(45)),
        ],
        TrajectoryConfig(5.0, 2.0),
    )

    random.seed(4915)

    dt = 0.02
    t = 0

    max_error = -1e10
    error_sum = 0

    while t < trajectory.totalTime():
        ground_truth_state = trajectory.sample(t)

        wheel_speeds = kinematics.toWheelSpeeds(
            ChassisSpeeds(
                ground_truth_state.velocity * ground_truth_state.pose.rotation().cos(),
                ground_truth_state.velocity * ground_truth_state.pose.rotation().sin(),
                0,
            )
        )

        wheel_speeds.frontLeft += random.gauss(0.0, 1.0) * 0.1
        wheel_speeds.frontRight += random.gauss(0.0, 1.0) * 0.1
        wheel_speeds.rearLeft += random.gauss(0.0, 1.0) * 0.1
        wheel_speeds.rearRight += random.gauss(0.0, 1.0) * 0.1

        wheel_positions.frontLeft += wheel_speeds.frontLeft * dt
        wheel_positions.frontRight += wheel_speeds.frontRight * dt
        wheel_positions.rearLeft += wheel_speeds.rearLeft * dt
        wheel_positions.rearRight += wheel_speeds.rearRight * dt

        xhat = odometry.update(
            Rotation2d(random.gauss(0.0, 1.0) * 0.05), wheel_positions
        )

        error = ground_truth_state.pose.translation().distance(xhat.translation())
        if error > max_error:
            max_error = error
        error_sum += error

        t += dt

    assert error_sum / (trajectory.totalTime() / dt) < 0.06
    assert max_error < 0.125
