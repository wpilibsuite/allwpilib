from wpiutil.protobuf import ProtobufDescriptor

from .. import (
    ArmFeedforward,
    DifferentialDriveFeedforward,
    DifferentialDriveWheelVoltages,
    ElevatorFeedforward,
    SimpleMotorFeedforward,
)
from ._generated import controller_pb2

__all__ = [
    "ArmFeedforwardProto",
    "DifferentialDriveFeedforwardProto",
    "ElevatorFeedforwardProto",
    "SimpleMotorFeedforwardProto",
    "DifferentialDriveWheelVoltagesProto",
]


def _dt_kwargs(dt: float) -> dict[str, float]:
    # The feedforward constructors report an error for a non-positive period. A message with
    # no period (dt == 0) should just get the constructor's default.
    return {"dt": dt} if dt > 0 else {}


class ArmFeedforwardProto(
    ProtobufDescriptor[ArmFeedforward, controller_pb2.ProtobufArmFeedforward]
):
    message_type = controller_pb2.ProtobufArmFeedforward

    @classmethod
    def pack(cls, msg, value):
        msg.ks = value.get_ks()
        msg.kg = value.get_kg()
        msg.kv = value.get_kv()
        msg.ka = value.get_ka()
        # The period isn't exposed by the bindings, so leave it unset; unpack then falls
        # back to the default period.
        msg.dt = 0.0

    @classmethod
    def unpack(cls, msg):
        return ArmFeedforward(msg.ks, msg.kg, msg.kv, msg.ka, **_dt_kwargs(msg.dt))


class ElevatorFeedforwardProto(
    ProtobufDescriptor[ElevatorFeedforward, controller_pb2.ProtobufElevatorFeedforward]
):
    message_type = controller_pb2.ProtobufElevatorFeedforward

    @classmethod
    def pack(cls, msg, value):
        msg.ks = value.get_ks()
        msg.kg = value.get_kg()
        msg.kv = value.get_kv()
        msg.ka = value.get_ka()
        # See ArmFeedforwardProto.pack
        msg.dt = 0.0

    @classmethod
    def unpack(cls, msg):
        return ElevatorFeedforward(msg.ks, msg.kg, msg.kv, msg.ka, **_dt_kwargs(msg.dt))


class SimpleMotorFeedforwardProto(
    ProtobufDescriptor[
        SimpleMotorFeedforward, controller_pb2.ProtobufSimpleMotorFeedforward
    ]
):
    message_type = controller_pb2.ProtobufSimpleMotorFeedforward

    @classmethod
    def pack(cls, msg, value):
        msg.ks = value.get_ks()
        msg.kv = value.get_kv()
        msg.ka = value.get_ka()
        msg.dt = value.get_dt()

    @classmethod
    def unpack(cls, msg):
        return SimpleMotorFeedforward(msg.ks, msg.kv, msg.ka, **_dt_kwargs(msg.dt))


class DifferentialDriveFeedforwardProto(
    ProtobufDescriptor[
        DifferentialDriveFeedforward,
        controller_pb2.ProtobufDifferentialDriveFeedforward,
    ]
):
    message_type = controller_pb2.ProtobufDifferentialDriveFeedforward

    @classmethod
    def pack(cls, msg, value):
        msg.kvLinear = value.kv_linear
        msg.kaLinear = value.ka_linear
        msg.kvAngular = value.kv_angular
        msg.kaAngular = value.ka_angular

    @classmethod
    def unpack(cls, msg):
        return DifferentialDriveFeedforward(
            msg.kvLinear, msg.kaLinear, msg.kvAngular, msg.kaAngular
        )


class DifferentialDriveWheelVoltagesProto(
    ProtobufDescriptor[
        DifferentialDriveWheelVoltages,
        controller_pb2.ProtobufDifferentialDriveWheelVoltages,
    ]
):
    message_type = controller_pb2.ProtobufDifferentialDriveWheelVoltages

    @classmethod
    def pack(cls, msg, value):
        msg.left = value.left
        msg.right = value.right

    @classmethod
    def unpack(cls, msg):
        return DifferentialDriveWheelVoltages(msg.left, msg.right)
