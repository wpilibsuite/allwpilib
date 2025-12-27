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


@pytest.fixture
def odometry3d_test():
    class MecanumDriveOdometry3dTest:
        def __init__(self):
            self.m_fl = Translation2d(x=12, y=12)
            self.m_fr = Translation2d(x=12, y=-12)
            self.m_bl = Translation2d(x=-12, y=12)
            self.m_br = Translation2d(x=-12, y=-12)

            self.zero = MecanumDriveWheelPositions()

            self.kinematics = MecanumDriveKinematics(
                self.m_fl, self.m_fr, self.m_bl, self.m_br
            )
            self.odometry = MecanumDriveOdometry3d(
                self.kinematics, Rotation3d(), self.zero
            )

    return MecanumDriveOdometry3dTest()


def test_initialize(odometry3d_test):
    odometry = MecanumDriveOdometry3d(
        odometry3d_test.kinematics,
        gyroAngle=Rotation3d(),
        wheelPositions=odometry3d_test.zero,
        initialPose=Pose3d(x=1, y=2, z=0, rotation=Rotation3d.fromDegrees(0, 0, 45)),
    )

    pose = odometry.getPose()

    assert pose.x == pytest.approx(1, abs=1e-9)
    assert pose.y == pytest.approx(2, abs=1e-9)
    assert pose.z == pytest.approx(0, abs=1e-9)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(45, abs=1e-9)


def test_multiple_consecutive_updates(odometry3d_test):
    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=3.536, frontRight=3.536, rearLeft=3.536, rearRight=3.536
    )

    odometry3d_test.odometry.resetPosition(Rotation3d(), wheel_deltas, Pose3d())
    odometry3d_test.odometry.update(Rotation3d(), wheel_deltas)
    second_pose = odometry3d_test.odometry.update(Rotation3d(), wheel_deltas)

    assert second_pose.x == pytest.approx(0.0, abs=0.01)
    assert second_pose.y == pytest.approx(0.0, abs=0.01)
    assert second_pose.z == pytest.approx(0.0, abs=0.01)
    assert second_pose.rotation().toRotation2d().radians() == pytest.approx(
        0.0, abs=0.01
    )


def test_two_iterations(odometry3d_test):
    odometry3d_test.odometry.resetPosition(Rotation3d(), odometry3d_test.zero, Pose3d())
    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=0.3536, frontRight=0.3536, rearLeft=0.3536, rearRight=0.3536
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
        frontLeft=-13.328, frontRight=39.986, rearLeft=-13.329, rearRight=39.986
    )
    odometry3d_test.odometry.update(Rotation3d(), MecanumDriveWheelPositions())
    pose = odometry3d_test.odometry.update(
        Rotation3d.fromDegrees(0, 0, 90), wheel_deltas
    )

    assert pose.x == pytest.approx(8.4855, abs=0.01)
    assert pose.y == pytest.approx(8.4855, abs=0.01)
    assert pose.z == pytest.approx(0, abs=0.01)
    assert pose.rotation().toRotation2d().degrees() == pytest.approx(90.0, abs=0.01)


def test_gyro_angle_reset(odometry3d_test):
    odometry3d_test.odometry.resetPosition(
        Rotation3d.fromDegrees(0, 0, 90), odometry3d_test.zero, Pose3d()
    )

    wheel_deltas = MecanumDriveWheelPositions(
        frontLeft=0.3536, frontRight=0.3536, rearLeft=0.3536, rearRight=0.3536
    )

    pose = odometry3d_test.odometry.update(
        Rotation3d.fromDegrees(0, 0, 90), wheel_deltas
    )

    assert pose.x == pytest.approx(0.3536, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.z == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().toRotation2d().radians() == pytest.approx(0.0, abs=0.01)


def test_accuracy_facing_trajectory():
    kinematics = MecanumDriveKinematics(
        frontLeftWheel=Translation2d(x=1, y=1),
        frontRightWheel=Translation2d(x=1, y=-1),
        rearLeftWheel=Translation2d(x=-1, y=-1),
        rearRightWheel=Translation2d(x=-1, y=1),
    )

    wheel_positions = MecanumDriveWheelPositions()

    odometry = MecanumDriveOdometry3d(kinematics, Rotation3d(), wheel_positions)

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
            Rotation3d(
                ground_truth_state.pose.rotation()
                + Rotation2d(random.gauss(0.0, 1.0) * 0.05)
            ),
            wheel_positions,
        )

        error = ground_truth_state.pose.translation().distance(
            xhat.translation().toTranslation2d()
        )
        if error > max_error:
            max_error = error
        error_sum += error

        t += dt

    assert error_sum / (trajectory.totalTime() / dt) < 0.07
    assert max_error < 0.125


def test_accuracy_facing_x_axis():
    kinematics = MecanumDriveKinematics(
        frontLeftWheel=Translation2d(x=1, y=1),
        frontRightWheel=Translation2d(x=1, y=-1),
        rearLeftWheel=Translation2d(x=-1, y=-1),
        rearRightWheel=Translation2d(x=-1, y=1),
    )

    wheel_positions = MecanumDriveWheelPositions()

    odometry = MecanumDriveOdometry3d(kinematics, Rotation3d(), wheel_positions)

    trajectory = TrajectoryGenerator.generateTrajectory(
        [
            Pose2d(x=0, y=0, rotation=Rotation2d(45)),
            Pose2d(x=3, y=0, rotation=Rotation2d(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d(135)),
            Pose2d(x=-3, y=0, rotation=Rotation2d(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d(45)),
        ],
        TrajectoryConfig(maxVelocity=5.0, maxAcceleration=2.0),
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
            Rotation3d(0, 0, random.gauss(0.0, 1.0) * 0.05),
            wheel_positions,
        )

        error = ground_truth_state.pose.translation().distance(
            xhat.translation().toTranslation2d()
        )
        if error > max_error:
            max_error = error
        error_sum += error

        t += dt

    assert error_sum / (trajectory.totalTime() / dt) < 0.06
    assert max_error < 0.125
