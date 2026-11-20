import inspect

import numpy as np
import pytest
from google.protobuf.message import Message
from wpiutil.protobuf import ProtobufDescriptor, get_serializer

import wpimath
from wpimath import protobuf
from wpimath.protobuf import (
    ArmFeedforwardProto,
    CubicHermiteSplineProto,
    DCMotorProto,
    DifferentialDriveFeedforwardProto,
    DifferentialDriveKinematicsProto,
    DifferentialDriveWheelAccelerationsProto,
    DifferentialDriveWheelPositionsProto,
    DifferentialDriveWheelVelocitiesProto,
    DifferentialDriveWheelVoltagesProto,
    DifferentialSampleProto,
    DifferentialTrajectoryProto,
    ElevatorFeedforwardProto,
    Ellipse2dProto,
    HolonomicSampleProto,
    HolonomicTrajectoryProto,
    LinearSystemProto,
    MatrixProto,
    MecanumDriveKinematicsProto,
    MecanumDriveWheelAccelerationsProto,
    MecanumDriveWheelPositionsProto,
    MecanumDriveWheelVelocitiesProto,
    Pose2dProto,
    Pose3dProto,
    QuaternionProto,
    QuinticHermiteSplineProto,
    Rectangle2dProto,
    Rotation2dProto,
    Rotation3dProto,
    SimpleMotorFeedforwardProto,
    SwerveDriveKinematicsProto,
    SwerveModuleAccelerationProto,
    SwerveModulePositionProto,
    SwerveModuleVelocityProto,
    Transform2dProto,
    Transform3dProto,
    Translation2dProto,
    Translation3dProto,
    Twist2dProto,
    Twist3dProto,
    VectorProto,
    geometry2d_pb2,
)

_GENERATED_MODULES = [
    getattr(protobuf, name) for name in protobuf.__all__ if name.endswith("_pb2")
]
_SERIALIZERS = [
    obj
    for obj in vars(protobuf).values()
    if inspect.isclass(obj)
    and issubclass(obj, ProtobufDescriptor)
    and obj is not ProtobufDescriptor
]


def _translation2d():
    return wpimath.Translation2d(1.5, -2.0)


def _pose2d():
    return wpimath.Pose2d(1.5, -2.0, wpimath.Rotation2d(0.25))


def _pose3d():
    return wpimath.Pose3d(
        wpimath.Translation3d(1.0, 2.0, 3.0), wpimath.Rotation3d(0.1, 0.2, 0.3)
    )


def _sample_args():
    return (
        0.0,  # the first sample of a trajectory must be at time 0
        _pose2d(),
        wpimath.ChassisVelocities(1.0, 2.0, 3.0),
        wpimath.ChassisAccelerations(4.0, 5.0, 6.0),
    )


def _values():
    """One representative value for every serializer."""
    t2 = _translation2d
    return {
        Translation2dProto: t2,
        Rotation2dProto: lambda: wpimath.Rotation2d(0.75),
        Pose2dProto: _pose2d,
        Transform2dProto: lambda: wpimath.Transform2d(t2(), wpimath.Rotation2d(1.0)),
        Twist2dProto: lambda: wpimath.Twist2d(1.0, 2.0, 3.0),
        Translation3dProto: lambda: wpimath.Translation3d(1.0, 2.0, 3.0),
        QuaternionProto: lambda: wpimath.Quaternion(0.5, 0.5, 0.5, 0.5),
        Rotation3dProto: lambda: wpimath.Rotation3d(0.1, 0.2, 0.3),
        Pose3dProto: _pose3d,
        Transform3dProto: lambda: wpimath.Transform3d(
            wpimath.Translation3d(1.0, 2.0, 3.0), wpimath.Rotation3d(0.1, 0.2, 0.3)
        ),
        Twist3dProto: lambda: wpimath.Twist3d(1.0, 2.0, 3.0, 0.1, 0.2, 0.3),
        protobuf.ChassisVelocitiesProto: lambda: wpimath.ChassisVelocities(
            1.0, 2.0, 3.0
        ),
        protobuf.ChassisAccelerationsProto: lambda: wpimath.ChassisAccelerations(
            1.0, 2.0, 3.0
        ),
        DifferentialDriveKinematicsProto: lambda: wpimath.DifferentialDriveKinematics(
            0.6
        ),
        DifferentialDriveWheelVelocitiesProto: lambda: wpimath.DifferentialDriveWheelVelocities(
            1.0, 2.0
        ),
        DifferentialDriveWheelAccelerationsProto: lambda: wpimath.DifferentialDriveWheelAccelerations(
            1.0, 2.0
        ),
        DifferentialDriveWheelPositionsProto: _diff_positions,
        MecanumDriveKinematicsProto: lambda: wpimath.MecanumDriveKinematics(
            wpimath.Translation2d(1.0, 1.0),
            wpimath.Translation2d(1.0, -1.0),
            wpimath.Translation2d(-1.0, 1.0),
            wpimath.Translation2d(-1.0, -1.0),
        ),
        MecanumDriveWheelPositionsProto: lambda: wpimath.MecanumDriveWheelPositions(
            1.0, 2.0, 3.0, 4.0
        ),
        MecanumDriveWheelVelocitiesProto: lambda: wpimath.MecanumDriveWheelVelocities(
            1.0, 2.0, 3.0, 4.0
        ),
        MecanumDriveWheelAccelerationsProto: lambda: wpimath.MecanumDriveWheelAccelerations(
            1.0, 2.0, 3.0, 4.0
        ),
        SwerveDriveKinematicsProto: lambda: wpimath.SwerveDrive4Kinematics(
            wpimath.Translation2d(1.0, 1.0),
            wpimath.Translation2d(1.0, -1.0),
            wpimath.Translation2d(-1.0, 1.0),
            wpimath.Translation2d(-1.0, -1.0),
        ),
        SwerveModulePositionProto: lambda: wpimath.SwerveModulePosition(
            1.0, wpimath.Rotation2d(0.5)
        ),
        SwerveModuleVelocityProto: lambda: wpimath.SwerveModuleVelocity(
            1.0, wpimath.Rotation2d(0.5)
        ),
        SwerveModuleAccelerationProto: lambda: wpimath.SwerveModuleAcceleration(
            1.0, wpimath.Rotation2d(0.5)
        ),
        ArmFeedforwardProto: lambda: wpimath.ArmFeedforward(1.0, 2.0, 3.0, 4.0),
        ElevatorFeedforwardProto: lambda: wpimath.ElevatorFeedforward(
            1.0, 2.0, 3.0, 4.0
        ),
        SimpleMotorFeedforwardProto: lambda: wpimath.SimpleMotorFeedforward(
            1.0, 2.0, 3.0, 0.05
        ),
        DifferentialDriveFeedforwardProto: lambda: wpimath.DifferentialDriveFeedforward(
            1.0, 2.0, 3.0, 4.0
        ),
        DifferentialDriveWheelVoltagesProto: lambda: wpimath.DifferentialDriveWheelVoltages(
            1.0, 2.0
        ),
        Rectangle2dProto: lambda: wpimath.Rectangle2d(_pose2d(), 2.0, 3.0),
        Ellipse2dProto: lambda: wpimath.Ellipse2d(_pose2d(), 2.0, 3.0),
        CubicHermiteSplineProto: lambda: wpimath.CubicHermiteSpline(
            (1.0, 2.0), (3.0, 4.0), (5.0, 6.0), (7.0, 8.0)
        ),
        QuinticHermiteSplineProto: lambda: wpimath.QuinticHermiteSpline(
            (1.0, 2.0, 3.0), (4.0, 5.0, 6.0), (7.0, 8.0, 9.0), (10.0, 11.0, 12.0)
        ),
        DCMotorProto: lambda: wpimath.DCMotor(12.0, 2.0, 100.0, 1.5, 500.0),
        LinearSystemProto: lambda: wpimath.LinearSystem_2_1_1(
            [[0.0, 1.0], [0.0, -1.0]], [[0.0], [1.0]], [[1.0, 0.0]], [[0.0]]
        ),
        MatrixProto: lambda: np.arange(6.0).reshape(2, 3),
        VectorProto: lambda: np.array([[1.0], [2.0], [3.0]]),
        HolonomicSampleProto: lambda: wpimath.HolonomicSample(*_sample_args()),
        DifferentialSampleProto: lambda: wpimath.DifferentialSample(
            *_sample_args(), 1.0, 2.0
        ),
        HolonomicTrajectoryProto: lambda: wpimath.HolonomicTrajectory(
            [wpimath.HolonomicSample(*_sample_args())]
        ),
        DifferentialTrajectoryProto: lambda: wpimath.DifferentialTrajectory(
            [wpimath.DifferentialSample(*_sample_args(), 1.0, 2.0)]
        ),
    }


def _diff_positions():
    positions = wpimath.DifferentialDriveWheelPositions()
    positions.left = 1.0
    positions.right = 2.0
    return positions


def test_every_message_has_a_serializer():
    messages = {
        obj
        for module in _GENERATED_MODULES
        for obj in vars(module).values()
        if inspect.isclass(obj) and issubclass(obj, Message)
    }
    assert messages
    covered = {serializer.message_type for serializer in _SERIALIZERS}
    assert messages == covered


def test_every_serializer_has_a_test_value():
    assert set(_SERIALIZERS) == set(_values())


@pytest.mark.parametrize(
    "serializer", _SERIALIZERS, ids=lambda serializer: serializer.__name__
)
def test_roundtrip(serializer):
    value = _values()[serializer]()
    data = serializer.serialize(value)
    result = serializer.deserialize(data)

    assert type(result) is type(value)
    # Not every type defines ==, so compare the wire bytes; this also checks that the values
    # survive a second trip.
    assert serializer.serialize(result) == data


def test_zero_valued_nested_messages_stay_on_the_wire():
    # The origin has all-zero nested messages. C++ and Java decoders need both fields present.
    data = Pose2dProto.serialize(wpimath.Pose2d())
    assert data.hex() == "0a001200"
    assert geometry2d_pb2.ProtobufPose2d.FromString(data).HasField("translation")


def test_pose2d_values_and_wire_bytes():
    pose = _pose2d()
    data = Pose2dProto.serialize(pose)
    # Field 1 (translation) then field 2 (rotation); all doubles are little-endian.
    assert data.hex() == (
        "0a12" "09000000000000f83f" "11" "00000000000000c0" "1209" "09000000000000d03f"
    )
    assert Pose2dProto.deserialize(data) == pose


def test_dt_zero_falls_back_to_default_period():
    msg = protobuf.controller_pb2.ProtobufArmFeedforward(ks=1, kg=2, kv=3, ka=4)
    ArmFeedforwardProto.unpack(msg)  # must not raise


def test_swerve_kinematics_picks_class_by_module_count():
    for count, cls in (
        (2, wpimath.SwerveDrive2Kinematics),
        (3, wpimath.SwerveDrive3Kinematics),
    ):
        kinematics = cls(*(wpimath.Translation2d(float(i), 0.0) for i in range(count)))
        result = SwerveDriveKinematicsProto.deserialize(
            SwerveDriveKinematicsProto.serialize(kinematics)
        )
        assert type(result) is cls

    with pytest.raises(ValueError):
        msg = protobuf.kinematics_pb2.ProtobufSwerveDriveKinematics()
        for _ in range(5):
            msg.modules.add()
        SwerveDriveKinematicsProto.unpack(msg)


def test_linear_system_sizes_and_values():
    system = _values()[LinearSystemProto]()
    result = LinearSystemProto.deserialize(LinearSystemProto.serialize(system))
    assert isinstance(result, wpimath.LinearSystem_2_1_1)
    assert np.array_equal(result.a(), system.a())
    assert np.array_equal(result.b(), system.b())


def test_matrix_is_column_major_like_cpp():
    msg = protobuf.wpimath_pb2.ProtobufMatrix()
    MatrixProto.pack(msg, np.array([[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]]))
    assert (msg.num_rows, msg.num_cols) == (2, 3)
    assert list(msg.data) == [1.0, 4.0, 2.0, 5.0, 3.0, 6.0]

    # and it reads back the same way
    assert np.array_equal(
        MatrixProto.unpack(msg), np.array([[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]])
    )


def test_serializers_are_exported_from_wpimath():
    for serializer in _SERIALIZERS:
        name = serializer.__name__
        assert getattr(wpimath, name) is serializer
        assert name in wpimath.__all__


def test_serializers_are_registered_for_their_value_types():
    # the serializer is a class attribute of the (pybind) value type, like WPIStruct
    assert wpimath.Pose2d.WPIProto is Pose2dProto
    assert get_serializer(wpimath.Pose2d) is Pose2dProto
    assert get_serializer(wpimath.HolonomicTrajectory) is HolonomicTrajectoryProto
    # unions register every member
    assert get_serializer(wpimath.SwerveDrive4Kinematics) is SwerveDriveKinematicsProto
    assert get_serializer(wpimath.LinearSystem_3_2_1) is LinearSystemProto


def test_bad_shapes_are_rejected():
    with pytest.raises(ValueError):
        msg = protobuf.wpimath_pb2.ProtobufMatrix(num_rows=2, num_cols=2, data=[1.0])
        MatrixProto.unpack(msg)
    with pytest.raises(ValueError):
        CubicHermiteSplineProto.unpack(protobuf.spline_pb2.ProtobufCubicHermiteSpline())
