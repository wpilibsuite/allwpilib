from wpiutil.protobuf import Protobuf

from .. import (
    DifferentialSample,
    DifferentialTrajectory,
    HolonomicSample,
    HolonomicTrajectory,
)
from ._generated import trajectory_pb2
from ._util import pack_nested
from .geometry import Pose2dProto
from .kinematics import ChassisAccelerationsProto, ChassisVelocitiesProto

__all__ = [
    "HolonomicSampleProto",
    "DifferentialSampleProto",
    "HolonomicTrajectoryProto",
    "DifferentialTrajectoryProto",
]


class HolonomicSampleProto(
    Protobuf[HolonomicSample, trajectory_pb2.ProtobufHolonomicSample]
):
    message_type = trajectory_pb2.ProtobufHolonomicSample

    @classmethod
    def pack(cls, msg, value):
        msg.time = value.time
        pack_nested(msg.pose, Pose2dProto, value.pose)
        pack_nested(msg.velocities, ChassisVelocitiesProto, value.velocity)
        pack_nested(msg.accelerations, ChassisAccelerationsProto, value.acceleration)

    @classmethod
    def unpack(cls, msg):
        return HolonomicSample(
            msg.time,
            Pose2dProto.unpack(msg.pose),
            ChassisVelocitiesProto.unpack(msg.velocities),
            ChassisAccelerationsProto.unpack(msg.accelerations),
        )


class DifferentialSampleProto(
    Protobuf[DifferentialSample, trajectory_pb2.ProtobufDifferentialSample]
):
    message_type = trajectory_pb2.ProtobufDifferentialSample

    @classmethod
    def pack(cls, msg, value):
        msg.time = value.time
        pack_nested(msg.pose, Pose2dProto, value.pose)
        pack_nested(msg.velocities, ChassisVelocitiesProto, value.velocity)
        pack_nested(msg.accelerations, ChassisAccelerationsProto, value.acceleration)
        msg.left_velocity = value.left_velocity
        msg.right_velocity = value.right_velocity

    @classmethod
    def unpack(cls, msg):
        return DifferentialSample(
            msg.time,
            Pose2dProto.unpack(msg.pose),
            ChassisVelocitiesProto.unpack(msg.velocities),
            ChassisAccelerationsProto.unpack(msg.accelerations),
            msg.left_velocity,
            msg.right_velocity,
        )


class HolonomicTrajectoryProto(
    Protobuf[HolonomicTrajectory, trajectory_pb2.ProtobufHolonomicTrajectory]
):
    message_type = trajectory_pb2.ProtobufHolonomicTrajectory

    @classmethod
    def pack(cls, msg, value):
        for sample in value.samples():
            HolonomicSampleProto.pack(msg.samples.add(), sample)

    @classmethod
    def unpack(cls, msg):
        # Raises ValueError if there are no samples, like the trajectory constructor.
        return HolonomicTrajectory(
            [HolonomicSampleProto.unpack(s) for s in msg.samples]
        )


class DifferentialTrajectoryProto(
    Protobuf[DifferentialTrajectory, trajectory_pb2.ProtobufDifferentialTrajectory]
):
    message_type = trajectory_pb2.ProtobufDifferentialTrajectory

    @classmethod
    def pack(cls, msg, value):
        for sample in value.samples():
            DifferentialSampleProto.pack(msg.samples.add(), sample)

    @classmethod
    def unpack(cls, msg):
        return DifferentialTrajectory(
            [DifferentialSampleProto.unpack(s) for s in msg.samples]
        )
