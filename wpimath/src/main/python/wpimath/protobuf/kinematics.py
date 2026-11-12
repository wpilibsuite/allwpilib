from wpiutil.protobuf import Protobuf

from .. import (
    ChassisAccelerations,
    ChassisVelocities,
    DifferentialDriveKinematics,
    DifferentialDriveWheelAccelerations,
    DifferentialDriveWheelPositions,
    DifferentialDriveWheelVelocities,
    MecanumDriveKinematics,
    MecanumDriveWheelAccelerations,
    MecanumDriveWheelPositions,
    MecanumDriveWheelVelocities,
    SwerveDrive2Kinematics,
    SwerveDrive3Kinematics,
    SwerveDrive4Kinematics,
    SwerveDrive6Kinematics,
    SwerveModuleAcceleration,
    SwerveModulePosition,
    SwerveModuleVelocity,
)
from ._generated import kinematics_pb2
from ._util import pack_nested
from .geometry import Rotation2dProto, Translation2dProto

__all__ = [
    "ChassisVelocitiesProto",
    "ChassisAccelerationsProto",
    "DifferentialDriveKinematicsProto",
    "DifferentialDriveWheelVelocitiesProto",
    "DifferentialDriveWheelAccelerationsProto",
    "DifferentialDriveWheelPositionsProto",
    "MecanumDriveKinematicsProto",
    "MecanumDriveWheelPositionsProto",
    "MecanumDriveWheelVelocitiesProto",
    "MecanumDriveWheelAccelerationsProto",
    "SwerveDriveKinematicsProto",
    "SwerveModulePositionProto",
    "SwerveModuleVelocityProto",
    "SwerveModuleAccelerationProto",
]


class ChassisVelocitiesProto(
    Protobuf[ChassisVelocities, kinematics_pb2.ProtobufChassisVelocities]
):
    message_type = kinematics_pb2.ProtobufChassisVelocities

    @classmethod
    def pack(cls, msg, value):
        msg.vx = value.vx
        msg.vy = value.vy
        msg.omega = value.omega

    @classmethod
    def unpack(cls, msg):
        return ChassisVelocities(msg.vx, msg.vy, msg.omega)


class ChassisAccelerationsProto(
    Protobuf[ChassisAccelerations, kinematics_pb2.ProtobufChassisAccelerations]
):
    message_type = kinematics_pb2.ProtobufChassisAccelerations

    @classmethod
    def pack(cls, msg, value):
        msg.ax = value.ax
        msg.ay = value.ay
        msg.alpha = value.alpha

    @classmethod
    def unpack(cls, msg):
        return ChassisAccelerations(msg.ax, msg.ay, msg.alpha)


class DifferentialDriveKinematicsProto(
    Protobuf[
        DifferentialDriveKinematics, kinematics_pb2.ProtobufDifferentialDriveKinematics
    ]
):
    message_type = kinematics_pb2.ProtobufDifferentialDriveKinematics

    @classmethod
    def pack(cls, msg, value):
        msg.trackwidth = value.trackwidth

    @classmethod
    def unpack(cls, msg):
        return DifferentialDriveKinematics(msg.trackwidth)


class DifferentialDriveWheelVelocitiesProto(
    Protobuf[
        DifferentialDriveWheelVelocities,
        kinematics_pb2.ProtobufDifferentialDriveWheelVelocities,
    ]
):
    message_type = kinematics_pb2.ProtobufDifferentialDriveWheelVelocities

    @classmethod
    def pack(cls, msg, value):
        msg.left = value.left
        msg.right = value.right

    @classmethod
    def unpack(cls, msg):
        return DifferentialDriveWheelVelocities(msg.left, msg.right)


class DifferentialDriveWheelAccelerationsProto(
    Protobuf[
        DifferentialDriveWheelAccelerations,
        kinematics_pb2.ProtobufDifferentialDriveWheelAccelerations,
    ]
):
    message_type = kinematics_pb2.ProtobufDifferentialDriveWheelAccelerations

    @classmethod
    def pack(cls, msg, value):
        msg.left = value.left
        msg.right = value.right

    @classmethod
    def unpack(cls, msg):
        return DifferentialDriveWheelAccelerations(msg.left, msg.right)


class DifferentialDriveWheelPositionsProto(
    Protobuf[
        DifferentialDriveWheelPositions,
        kinematics_pb2.ProtobufDifferentialDriveWheelPositions,
    ]
):
    message_type = kinematics_pb2.ProtobufDifferentialDriveWheelPositions

    @classmethod
    def pack(cls, msg, value):
        msg.left = value.left
        msg.right = value.right

    @classmethod
    def unpack(cls, msg):
        # The Python binding only has a default constructor for this type.
        positions = DifferentialDriveWheelPositions()
        positions.left = msg.left
        positions.right = msg.right
        return positions


class MecanumDriveKinematicsProto(
    Protobuf[MecanumDriveKinematics, kinematics_pb2.ProtobufMecanumDriveKinematics]
):
    message_type = kinematics_pb2.ProtobufMecanumDriveKinematics

    @classmethod
    def pack(cls, msg, value):
        pack_nested(msg.front_left, Translation2dProto, value.get_front_left())
        pack_nested(msg.front_right, Translation2dProto, value.get_front_right())
        pack_nested(msg.rear_left, Translation2dProto, value.get_rear_left())
        pack_nested(msg.rear_right, Translation2dProto, value.get_rear_right())

    @classmethod
    def unpack(cls, msg):
        return MecanumDriveKinematics(
            Translation2dProto.unpack(msg.front_left),
            Translation2dProto.unpack(msg.front_right),
            Translation2dProto.unpack(msg.rear_left),
            Translation2dProto.unpack(msg.rear_right),
        )


class MecanumDriveWheelPositionsProto(
    Protobuf[
        MecanumDriveWheelPositions, kinematics_pb2.ProtobufMecanumDriveWheelPositions
    ]
):
    message_type = kinematics_pb2.ProtobufMecanumDriveWheelPositions

    @classmethod
    def pack(cls, msg, value):
        msg.front_left = value.front_left
        msg.front_right = value.front_right
        msg.rear_left = value.rear_left
        msg.rear_right = value.rear_right

    @classmethod
    def unpack(cls, msg):
        return MecanumDriveWheelPositions(
            msg.front_left, msg.front_right, msg.rear_left, msg.rear_right
        )


class MecanumDriveWheelVelocitiesProto(
    Protobuf[
        MecanumDriveWheelVelocities, kinematics_pb2.ProtobufMecanumDriveWheelVelocities
    ]
):
    message_type = kinematics_pb2.ProtobufMecanumDriveWheelVelocities

    @classmethod
    def pack(cls, msg, value):
        msg.front_left = value.front_left
        msg.front_right = value.front_right
        msg.rear_left = value.rear_left
        msg.rear_right = value.rear_right

    @classmethod
    def unpack(cls, msg):
        return MecanumDriveWheelVelocities(
            msg.front_left, msg.front_right, msg.rear_left, msg.rear_right
        )


class MecanumDriveWheelAccelerationsProto(
    Protobuf[
        MecanumDriveWheelAccelerations,
        kinematics_pb2.ProtobufMecanumDriveWheelAccelerations,
    ]
):
    message_type = kinematics_pb2.ProtobufMecanumDriveWheelAccelerations

    @classmethod
    def pack(cls, msg, value):
        msg.front_left = value.front_left
        msg.front_right = value.front_right
        msg.rear_left = value.rear_left
        msg.rear_right = value.rear_right

    @classmethod
    def unpack(cls, msg):
        return MecanumDriveWheelAccelerations(
            msg.front_left, msg.front_right, msg.rear_left, msg.rear_right
        )


#: The swerve kinematics classes the Python bindings provide, keyed by module count.
_SWERVE_KINEMATICS = {
    2: SwerveDrive2Kinematics,
    3: SwerveDrive3Kinematics,
    4: SwerveDrive4Kinematics,
    6: SwerveDrive6Kinematics,
}

SwerveDriveKinematics = (
    SwerveDrive2Kinematics
    | SwerveDrive3Kinematics
    | SwerveDrive4Kinematics
    | SwerveDrive6Kinematics
)


class SwerveDriveKinematicsProto(
    Protobuf[SwerveDriveKinematics, kinematics_pb2.ProtobufSwerveDriveKinematics]
):
    """Works for every module count the bindings provide (2, 3, 4 and 6)."""

    message_type = kinematics_pb2.ProtobufSwerveDriveKinematics

    @classmethod
    def pack(cls, msg, value):
        for module in value.get_modules():
            Translation2dProto.pack(msg.modules.add(), module)

    @classmethod
    def unpack(cls, msg):
        count = len(msg.modules)
        kinematics = _SWERVE_KINEMATICS.get(count)
        if kinematics is None:
            raise ValueError(
                f"No swerve kinematics class for {count} modules; "
                f"supported counts are {sorted(_SWERVE_KINEMATICS)}"
            )
        return kinematics(*(Translation2dProto.unpack(m) for m in msg.modules))


class SwerveModulePositionProto(
    Protobuf[SwerveModulePosition, kinematics_pb2.ProtobufSwerveModulePosition]
):
    message_type = kinematics_pb2.ProtobufSwerveModulePosition

    @classmethod
    def pack(cls, msg, value):
        msg.distance = value.distance
        pack_nested(msg.angle, Rotation2dProto, value.angle)

    @classmethod
    def unpack(cls, msg):
        return SwerveModulePosition(msg.distance, Rotation2dProto.unpack(msg.angle))


class SwerveModuleVelocityProto(
    Protobuf[SwerveModuleVelocity, kinematics_pb2.ProtobufSwerveModuleVelocity]
):
    message_type = kinematics_pb2.ProtobufSwerveModuleVelocity

    @classmethod
    def pack(cls, msg, value):
        msg.velocity = value.velocity
        pack_nested(msg.angle, Rotation2dProto, value.angle)

    @classmethod
    def unpack(cls, msg):
        return SwerveModuleVelocity(msg.velocity, Rotation2dProto.unpack(msg.angle))


class SwerveModuleAccelerationProto(
    Protobuf[SwerveModuleAcceleration, kinematics_pb2.ProtobufSwerveModuleAcceleration]
):
    message_type = kinematics_pb2.ProtobufSwerveModuleAcceleration

    @classmethod
    def pack(cls, msg, value):
        msg.acceleration = value.acceleration
        pack_nested(msg.angle, Rotation2dProto, value.angle)

    @classmethod
    def unpack(cls, msg):
        return SwerveModuleAcceleration(
            msg.acceleration, Rotation2dProto.unpack(msg.angle)
        )
