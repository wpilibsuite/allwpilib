from wpiutil.protobuf import Protobuf

from .. import Ellipse2d, Rectangle2d
from ._generated import shape_pb2
from ._util import pack_nested
from .geometry import Pose2dProto

__all__ = ["Rectangle2dProto", "Ellipse2dProto"]


class Rectangle2dProto(Protobuf[Rectangle2d, shape_pb2.ProtobufRectangle2d]):
    message_type = shape_pb2.ProtobufRectangle2d

    @classmethod
    def pack(cls, msg, value):
        pack_nested(msg.center, Pose2dProto, value.center())
        msg.xWidth = value.x_width
        msg.yWidth = value.y_width

    @classmethod
    def unpack(cls, msg):
        return Rectangle2d(Pose2dProto.unpack(msg.center), msg.xWidth, msg.yWidth)


class Ellipse2dProto(Protobuf[Ellipse2d, shape_pb2.ProtobufEllipse2d]):
    message_type = shape_pb2.ProtobufEllipse2d

    @classmethod
    def pack(cls, msg, value):
        pack_nested(msg.center, Pose2dProto, value.center())
        msg.xSemiAxis = value.x_semi_axis
        msg.ySemiAxis = value.y_semi_axis

    @classmethod
    def unpack(cls, msg):
        return Ellipse2d(Pose2dProto.unpack(msg.center), msg.xSemiAxis, msg.ySemiAxis)
