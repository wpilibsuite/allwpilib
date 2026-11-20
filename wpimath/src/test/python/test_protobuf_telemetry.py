import telemetry
import telemetry.mock_backend as mock_backend
import wpimath
from wpimath import HolonomicTrajectoryProto


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
