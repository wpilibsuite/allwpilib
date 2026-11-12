from wpiutil.protobuf import Protobuf

from .. import (
    Pose2d,
    Pose3d,
    Quaternion,
    Rotation2d,
    Rotation3d,
    Transform2d,
    Transform3d,
    Translation2d,
    Translation3d,
    Twist2d,
    Twist3d,
)
from ._generated import geometry2d_pb2, geometry3d_pb2
from ._util import pack_nested

__all__ = [
    "Translation2dProto",
    "Rotation2dProto",
    "Pose2dProto",
    "Transform2dProto",
    "Twist2dProto",
    "Translation3dProto",
    "QuaternionProto",
    "Rotation3dProto",
    "Pose3dProto",
    "Transform3dProto",
    "Twist3dProto",
]


class Translation2dProto(Protobuf[Translation2d, geometry2d_pb2.ProtobufTranslation2d]):
    message_type = geometry2d_pb2.ProtobufTranslation2d

    @classmethod
    def pack(cls, msg, value):
        msg.x = value.x
        msg.y = value.y

    @classmethod
    def unpack(cls, msg):
        return Translation2d(msg.x, msg.y)


class Rotation2dProto(Protobuf[Rotation2d, geometry2d_pb2.ProtobufRotation2d]):
    message_type = geometry2d_pb2.ProtobufRotation2d

    @classmethod
    def pack(cls, msg, value):
        msg.value = value.radians()

    @classmethod
    def unpack(cls, msg):
        return Rotation2d(msg.value)


class Pose2dProto(Protobuf[Pose2d, geometry2d_pb2.ProtobufPose2d]):
    message_type = geometry2d_pb2.ProtobufPose2d

    @classmethod
    def pack(cls, msg, value):
        pack_nested(msg.translation, Translation2dProto, value.translation())
        pack_nested(msg.rotation, Rotation2dProto, value.rotation())

    @classmethod
    def unpack(cls, msg):
        return Pose2d(
            Translation2dProto.unpack(msg.translation),
            Rotation2dProto.unpack(msg.rotation),
        )


class Transform2dProto(Protobuf[Transform2d, geometry2d_pb2.ProtobufTransform2d]):
    message_type = geometry2d_pb2.ProtobufTransform2d

    @classmethod
    def pack(cls, msg, value):
        pack_nested(msg.translation, Translation2dProto, value.translation())
        pack_nested(msg.rotation, Rotation2dProto, value.rotation())

    @classmethod
    def unpack(cls, msg):
        return Transform2d(
            Translation2dProto.unpack(msg.translation),
            Rotation2dProto.unpack(msg.rotation),
        )


class Twist2dProto(Protobuf[Twist2d, geometry2d_pb2.ProtobufTwist2d]):
    message_type = geometry2d_pb2.ProtobufTwist2d

    @classmethod
    def pack(cls, msg, value):
        msg.dx = value.dx
        msg.dy = value.dy
        msg.dtheta = value.dtheta

    @classmethod
    def unpack(cls, msg):
        return Twist2d(msg.dx, msg.dy, msg.dtheta)


class Translation3dProto(Protobuf[Translation3d, geometry3d_pb2.ProtobufTranslation3d]):
    message_type = geometry3d_pb2.ProtobufTranslation3d

    @classmethod
    def pack(cls, msg, value):
        msg.x = value.x
        msg.y = value.y
        msg.z = value.z

    @classmethod
    def unpack(cls, msg):
        return Translation3d(msg.x, msg.y, msg.z)


class QuaternionProto(Protobuf[Quaternion, geometry3d_pb2.ProtobufQuaternion]):
    message_type = geometry3d_pb2.ProtobufQuaternion

    @classmethod
    def pack(cls, msg, value):
        msg.w = value.w
        msg.x = value.x
        msg.y = value.y
        msg.z = value.z

    @classmethod
    def unpack(cls, msg):
        return Quaternion(msg.w, msg.x, msg.y, msg.z)


class Rotation3dProto(Protobuf[Rotation3d, geometry3d_pb2.ProtobufRotation3d]):
    message_type = geometry3d_pb2.ProtobufRotation3d

    @classmethod
    def pack(cls, msg, value):
        pack_nested(msg.q, QuaternionProto, value.get_quaternion())

    @classmethod
    def unpack(cls, msg):
        return Rotation3d(QuaternionProto.unpack(msg.q))


class Pose3dProto(Protobuf[Pose3d, geometry3d_pb2.ProtobufPose3d]):
    message_type = geometry3d_pb2.ProtobufPose3d

    @classmethod
    def pack(cls, msg, value):
        pack_nested(msg.translation, Translation3dProto, value.translation())
        pack_nested(msg.rotation, Rotation3dProto, value.rotation())

    @classmethod
    def unpack(cls, msg):
        return Pose3d(
            Translation3dProto.unpack(msg.translation),
            Rotation3dProto.unpack(msg.rotation),
        )


class Transform3dProto(Protobuf[Transform3d, geometry3d_pb2.ProtobufTransform3d]):
    message_type = geometry3d_pb2.ProtobufTransform3d

    @classmethod
    def pack(cls, msg, value):
        pack_nested(msg.translation, Translation3dProto, value.translation())
        pack_nested(msg.rotation, Rotation3dProto, value.rotation())

    @classmethod
    def unpack(cls, msg):
        return Transform3d(
            Translation3dProto.unpack(msg.translation),
            Rotation3dProto.unpack(msg.rotation),
        )


class Twist3dProto(Protobuf[Twist3d, geometry3d_pb2.ProtobufTwist3d]):
    message_type = geometry3d_pb2.ProtobufTwist3d

    @classmethod
    def pack(cls, msg, value):
        msg.dx = value.dx
        msg.dy = value.dy
        msg.dz = value.dz
        msg.rx = value.rx
        msg.ry = value.ry
        msg.rz = value.rz

    @classmethod
    def unpack(cls, msg):
        return Twist3d(msg.dx, msg.dy, msg.dz, msg.rx, msg.ry, msg.rz)
