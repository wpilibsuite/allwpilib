import pytest
import math
import random

from wpimath import (
    ChassisVelocities,
    DrivetrainSplineTrajectoryGenerator,
    MecanumDriveKinematics,
    MecanumDriveOdometry3d,
    MecanumDriveWheelPositions,
    Pose2d,
    Pose3d,
    Rotation2d,
    Rotation3d,
    TrajectoryConfig,
    Translation2d,
)


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
        gyro_angle=Rotation3d(),
        wheel_positions=odometry3d_test.zero,
        initial_pose=Pose3d(x=1, y=2, z=0, rotation=Rotation3d.from_degrees(0, 0, 45)),
    )

    pose = odometry.get_pose()

    assert pose.x == pytest.approx(1, abs=1e-9)
    assert pose.y == pytest.approx(2, abs=1e-9)
    assert pose.z == pytest.approx(0, abs=1e-9)
    assert pose.rotation().to_rotation2d().degrees() == pytest.approx(45, abs=1e-9)


def test_multiple_consecutive_updates(odometry3d_test):
    wheel_deltas = MecanumDriveWheelPositions(
        front_left=3.536, front_right=3.536, rear_left=3.536, rear_right=3.536
    )

    odometry3d_test.odometry.reset_position(Rotation3d(), wheel_deltas, Pose3d())
    odometry3d_test.odometry.update(Rotation3d(), wheel_deltas)
    second_pose = odometry3d_test.odometry.update(Rotation3d(), wheel_deltas)

    assert second_pose.x == pytest.approx(0.0, abs=0.01)
    assert second_pose.y == pytest.approx(0.0, abs=0.01)
    assert second_pose.z == pytest.approx(0.0, abs=0.01)
    assert second_pose.rotation().to_rotation2d().radians() == pytest.approx(
        0.0, abs=0.01
    )


def test_two_iterations(odometry3d_test):
    odometry3d_test.odometry.reset_position(
        Rotation3d(), odometry3d_test.zero, Pose3d()
    )
    wheel_deltas = MecanumDriveWheelPositions(
        front_left=0.3536, front_right=0.3536, rear_left=0.3536, rear_right=0.3536
    )

    odometry3d_test.odometry.update(Rotation3d(), MecanumDriveWheelPositions())
    pose = odometry3d_test.odometry.update(Rotation3d(), wheel_deltas)

    assert pose.x == pytest.approx(0.3536, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.z == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().to_rotation2d().radians() == pytest.approx(0.0, abs=0.01)


def test_90_degree_turn(odometry3d_test):
    odometry3d_test.odometry.reset_position(
        Rotation3d(), odometry3d_test.zero, Pose3d()
    )
    wheel_deltas = MecanumDriveWheelPositions(
        front_left=-13.328, front_right=39.986, rear_left=-13.329, rear_right=39.986
    )
    odometry3d_test.odometry.update(Rotation3d(), MecanumDriveWheelPositions())
    pose = odometry3d_test.odometry.update(
        Rotation3d.from_degrees(0, 0, 90), wheel_deltas
    )

    assert pose.x == pytest.approx(8.4855, abs=0.01)
    assert pose.y == pytest.approx(8.4855, abs=0.01)
    assert pose.z == pytest.approx(0, abs=0.01)
    assert pose.rotation().to_rotation2d().degrees() == pytest.approx(90.0, abs=0.01)


def test_gyro_angle_reset(odometry3d_test):
    odometry3d_test.odometry.reset_position(
        Rotation3d.from_degrees(0, 0, 90), odometry3d_test.zero, Pose3d()
    )

    wheel_deltas = MecanumDriveWheelPositions(
        front_left=0.3536, front_right=0.3536, rear_left=0.3536, rear_right=0.3536
    )

    pose = odometry3d_test.odometry.update(
        Rotation3d.from_degrees(0, 0, 90), wheel_deltas
    )

    assert pose.x == pytest.approx(0.3536, abs=0.01)
    assert pose.y == pytest.approx(0.0, abs=0.01)
    assert pose.z == pytest.approx(0.0, abs=0.01)
    assert pose.rotation().to_rotation2d().radians() == pytest.approx(0.0, abs=0.01)


def test_accuracy_facing_trajectory():
    kinematics = MecanumDriveKinematics(
        front_left_wheel=Translation2d(x=1, y=1),
        front_right_wheel=Translation2d(x=1, y=-1),
        rear_left_wheel=Translation2d(x=-1, y=-1),
        rear_right_wheel=Translation2d(x=-1, y=1),
    )

    wheel_positions = MecanumDriveWheelPositions()

    odometry = MecanumDriveOdometry3d(kinematics, Rotation3d(), wheel_positions)

    trajectory = DrivetrainSplineTrajectoryGenerator.generate(
        [
            Pose2d(x=0, y=0, rotation=Rotation2d.from_degrees(45)),
            Pose2d(x=3, y=0, rotation=Rotation2d.from_degrees(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d.from_degrees(135)),
            Pose2d(x=-3, y=0, rotation=Rotation2d.from_degrees(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d.from_degrees(45)),
        ],
        TrajectoryConfig(max_velocity=5.0, max_acceleration=2.0),
    )

    random.seed(4915)

    dt = 0.02
    t = 0

    max_error = -1e10
    error_sum = 0

    while t < trajectory.duration():
        ground_truth_state = trajectory.sample_at(t)

        wheel_velocities = kinematics.to_wheel_velocities(
            ground_truth_state.velocity.to_robot_relative(
                ground_truth_state.pose.rotation()
            )
        )

        wheel_velocities.front_left += random.gauss(0.0, 1.0) * 0.1
        wheel_velocities.front_right += random.gauss(0.0, 1.0) * 0.1
        wheel_velocities.rear_left += random.gauss(0.0, 1.0) * 0.1
        wheel_velocities.rear_right += random.gauss(0.0, 1.0) * 0.1

        wheel_positions.front_left += wheel_velocities.front_left * dt
        wheel_positions.front_right += wheel_velocities.front_right * dt
        wheel_positions.rear_left += wheel_velocities.rear_left * dt
        wheel_positions.rear_right += wheel_velocities.rear_right * dt

        xhat = odometry.update(
            Rotation3d(
                ground_truth_state.pose.rotation()
                + Rotation2d(random.gauss(0.0, 1.0) * 0.05)
            ),
            wheel_positions,
        )

        error = ground_truth_state.pose.translation().distance(
            xhat.translation().to_translation2d()
        )
        if error > max_error:
            max_error = error
        error_sum += error

        t += dt

    assert error_sum / (trajectory.duration() / dt) < 0.07
    assert max_error < 0.125


def test_accuracy_facing_x_axis():
    kinematics = MecanumDriveKinematics(
        front_left_wheel=Translation2d(x=1, y=1),
        front_right_wheel=Translation2d(x=1, y=-1),
        rear_left_wheel=Translation2d(x=-1, y=-1),
        rear_right_wheel=Translation2d(x=-1, y=1),
    )

    wheel_positions = MecanumDriveWheelPositions()

    odometry = MecanumDriveOdometry3d(kinematics, Rotation3d(), wheel_positions)

    trajectory = DrivetrainSplineTrajectoryGenerator.generate(
        [
            Pose2d(x=0, y=0, rotation=Rotation2d(45)),
            Pose2d(x=3, y=0, rotation=Rotation2d(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d(135)),
            Pose2d(x=-3, y=0, rotation=Rotation2d(-90)),
            Pose2d(x=0, y=0, rotation=Rotation2d(45)),
        ],
        TrajectoryConfig(max_velocity=5.0, max_acceleration=2.0),
    )

    random.seed(4915)

    dt = 0.02
    t = 0

    max_error = -1e10
    error_sum = 0

    while t < trajectory.duration():
        ground_truth_state = trajectory.sample_at(t)

        wheel_velocities = kinematics.to_wheel_velocities(
            ChassisVelocities(
                ground_truth_state.forward_velocity()
                * ground_truth_state.pose.rotation().cos(),
                ground_truth_state.forward_velocity()
                * ground_truth_state.pose.rotation().sin(),
                0,
            )
        )

        wheel_velocities.front_left += random.gauss(0.0, 1.0) * 0.1
        wheel_velocities.front_right += random.gauss(0.0, 1.0) * 0.1
        wheel_velocities.rear_left += random.gauss(0.0, 1.0) * 0.1
        wheel_velocities.rear_right += random.gauss(0.0, 1.0) * 0.1

        wheel_positions.front_left += wheel_velocities.front_left * dt
        wheel_positions.front_right += wheel_velocities.front_right * dt
        wheel_positions.rear_left += wheel_velocities.rear_left * dt
        wheel_positions.rear_right += wheel_velocities.rear_right * dt

        xhat = odometry.update(
            Rotation3d(0, 0, random.gauss(0.0, 1.0) * 0.05),
            wheel_positions,
        )

        error = ground_truth_state.pose.translation().distance(
            xhat.translation().to_translation2d()
        )
        if error > max_error:
            max_error = error
        error_sum += error

        t += dt

    assert error_sum / (trajectory.duration() / dt) < 0.06
    assert max_error < 0.125
