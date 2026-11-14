import ntcore
import pytest
import telemetry
import telemetry.mock_backend as mock_backend
import wpimath
from wpimath import (
    HolonomicTrajectoryProto,
    Pose2dProto,
    SwerveDriveKinematicsProto,
)


@pytest.fixture
def nt():
    inst = ntcore.NetworkTableInstance.create()
    inst.start_local()
    yield inst
    ntcore.NetworkTableInstance.destroy(inst)


def test_pose2d_topic(nt: ntcore.NetworkTableInstance):
    topic = ntcore.get_protobuf_topic(nt, "/pose", Pose2dProto)
    assert topic.get_type_string() == "proto:wpi.proto.ProtobufPose2d"

    pub = topic.publish()
    sub = topic.subscribe(wpimath.Pose2d())
    assert sub.get() == wpimath.Pose2d()

    pose = wpimath.Pose2d(1.5, -2.0, wpimath.Rotation2d(0.25))
    pub.set(pose)
    assert sub.get() == pose

    # Pose2d's file imports nothing, but its schema must be published for decoders.
    assert nt.has_schema("proto:wpi/math/protobuf/geometry2d.proto")


def test_trajectory_topic_registers_imported_schemas(nt: ntcore.NetworkTableInstance):
    topic = ntcore.get_protobuf_topic(nt, "/trajectory", HolonomicTrajectoryProto)
    pub = topic.publish()

    assert nt.has_schema("proto:wpi/math/protobuf/trajectory.proto")
    assert nt.has_schema("proto:wpi/math/protobuf/geometry2d.proto")
    assert nt.has_schema("proto:wpi/math/protobuf/kinematics.proto")

    sample = wpimath.HolonomicSample(
        0.0,
        wpimath.Pose2d(1.0, 2.0, wpimath.Rotation2d(0.5)),
        wpimath.ChassisVelocities(1.0, 0.0, 0.0),
        wpimath.ChassisAccelerations(0.0, 0.0, 0.0),
    )
    trajectory = wpimath.HolonomicTrajectory([sample])
    pub.set(trajectory)

    sub = topic.subscribe(trajectory)
    result = sub.get()
    assert len(result.samples()) == 1
    assert result.samples()[0].pose == sample.pose


def test_swerve_kinematics_topic(nt: ntcore.NetworkTableInstance):
    modules = [wpimath.Translation2d(float(i), 1.0) for i in range(4)]
    kinematics = wpimath.SwerveDrive4Kinematics(*modules)

    topic = ntcore.get_protobuf_topic(nt, "/kinematics", SwerveDriveKinematicsProto)
    topic.publish().set(kinematics)
    result = topic.subscribe(kinematics).get()

    assert isinstance(result, wpimath.SwerveDrive4Kinematics)
    assert list(result.get_modules()) == modules


def test_telemetry_logs_trajectory_as_protobuf():
    sample = wpimath.HolonomicSample(
        0.0,
        wpimath.Pose2d(1.0, 2.0, wpimath.Rotation2d(0.5)),
        wpimath.ChassisVelocities(1.0, 0.0, 0.0),
        wpimath.ChassisAccelerations(0.0, 0.0, 0.0),
    )
    trajectory = wpimath.HolonomicTrajectory([sample])

    telemetry.TelemetryRegistry.reset()
    backend = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("", backend)
    try:
        telemetry.log("trajectory", trajectory)

        assert backend.get_last_action("/trajectory") == mock_backend.Action(
            "/trajectory",
            mock_backend.LogRawValue(
                HolonomicTrajectoryProto.serialize(trajectory),
                "proto:wpi.proto.ProtobufHolonomicTrajectory",
            ),
        )
        assert backend.get_schema("proto:wpi/math/protobuf/trajectory.proto")
        assert backend.get_schema("proto:wpi/math/protobuf/geometry2d.proto")
    finally:
        telemetry.TelemetryRegistry.reset()
