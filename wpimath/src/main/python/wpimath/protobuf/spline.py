from wpiutil.protobuf import ProtobufDescriptor

from .. import CubicHermiteSpline, QuinticHermiteSpline
from ._generated import spline_pb2

__all__ = ["CubicHermiteSplineProto", "QuinticHermiteSplineProto"]


def _control_vector(name: str, values, expected: int) -> tuple[float, ...]:
    if len(values) != expected:
        raise ValueError(
            f"{name} must have {expected} entries for this spline, got {len(values)}"
        )
    return tuple(values)


class CubicHermiteSplineProto(
    ProtobufDescriptor[CubicHermiteSpline, spline_pb2.ProtobufCubicHermiteSpline]
):
    message_type = spline_pb2.ProtobufCubicHermiteSpline

    @classmethod
    def pack(cls, msg, value):
        initial = value.get_initial_control_vector()
        final = value.get_final_control_vector()
        msg.x_initial.extend(initial.x)
        msg.x_final.extend(final.x)
        msg.y_initial.extend(initial.y)
        msg.y_final.extend(final.y)

    @classmethod
    def unpack(cls, msg):
        return CubicHermiteSpline(
            _control_vector("x_initial", msg.x_initial, 2),
            _control_vector("x_final", msg.x_final, 2),
            _control_vector("y_initial", msg.y_initial, 2),
            _control_vector("y_final", msg.y_final, 2),
        )


class QuinticHermiteSplineProto(
    ProtobufDescriptor[QuinticHermiteSpline, spline_pb2.ProtobufQuinticHermiteSpline]
):
    message_type = spline_pb2.ProtobufQuinticHermiteSpline

    @classmethod
    def pack(cls, msg, value):
        initial = value.get_initial_control_vector()
        final = value.get_final_control_vector()
        msg.x_initial.extend(initial.x)
        msg.x_final.extend(final.x)
        msg.y_initial.extend(initial.y)
        msg.y_final.extend(final.y)

    @classmethod
    def unpack(cls, msg):
        return QuinticHermiteSpline(
            _control_vector("x_initial", msg.x_initial, 3),
            _control_vector("x_final", msg.x_final, 3),
            _control_vector("y_initial", msg.y_initial, 3),
            _control_vector("y_final", msg.y_final, 3),
        )
