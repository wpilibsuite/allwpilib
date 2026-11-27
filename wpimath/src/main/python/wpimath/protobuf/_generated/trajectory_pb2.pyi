from . import geometry2d_pb2 as _geometry2d_pb2
from . import kinematics_pb2 as _kinematics_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Iterable as _Iterable, Mapping as _Mapping
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class ProtobufHolonomicSample(_message.Message):
    __slots__ = ("time", "pose", "velocities", "accelerations")
    TIME_FIELD_NUMBER: _ClassVar[int]
    POSE_FIELD_NUMBER: _ClassVar[int]
    VELOCITIES_FIELD_NUMBER: _ClassVar[int]
    ACCELERATIONS_FIELD_NUMBER: _ClassVar[int]
    time: float
    pose: _geometry2d_pb2.ProtobufPose2d
    velocities: _kinematics_pb2.ProtobufChassisVelocities
    accelerations: _kinematics_pb2.ProtobufChassisAccelerations
    def __init__(self, time: _Optional[float] = ..., pose: _Optional[_Union[_geometry2d_pb2.ProtobufPose2d, _Mapping]] = ..., velocities: _Optional[_Union[_kinematics_pb2.ProtobufChassisVelocities, _Mapping]] = ..., accelerations: _Optional[_Union[_kinematics_pb2.ProtobufChassisAccelerations, _Mapping]] = ...) -> None: ...

class ProtobufDifferentialSample(_message.Message):
    __slots__ = ("time", "pose", "velocities", "accelerations", "left_velocity", "right_velocity")
    TIME_FIELD_NUMBER: _ClassVar[int]
    POSE_FIELD_NUMBER: _ClassVar[int]
    VELOCITIES_FIELD_NUMBER: _ClassVar[int]
    ACCELERATIONS_FIELD_NUMBER: _ClassVar[int]
    LEFT_VELOCITY_FIELD_NUMBER: _ClassVar[int]
    RIGHT_VELOCITY_FIELD_NUMBER: _ClassVar[int]
    time: float
    pose: _geometry2d_pb2.ProtobufPose2d
    velocities: _kinematics_pb2.ProtobufChassisVelocities
    accelerations: _kinematics_pb2.ProtobufChassisAccelerations
    left_velocity: float
    right_velocity: float
    def __init__(self, time: _Optional[float] = ..., pose: _Optional[_Union[_geometry2d_pb2.ProtobufPose2d, _Mapping]] = ..., velocities: _Optional[_Union[_kinematics_pb2.ProtobufChassisVelocities, _Mapping]] = ..., accelerations: _Optional[_Union[_kinematics_pb2.ProtobufChassisAccelerations, _Mapping]] = ..., left_velocity: _Optional[float] = ..., right_velocity: _Optional[float] = ...) -> None: ...

class ProtobufHolonomicTrajectory(_message.Message):
    __slots__ = ("samples",)
    SAMPLES_FIELD_NUMBER: _ClassVar[int]
    samples: _containers.RepeatedCompositeFieldContainer[ProtobufHolonomicSample]
    def __init__(self, samples: _Optional[_Iterable[_Union[ProtobufHolonomicSample, _Mapping]]] = ...) -> None: ...

class ProtobufDifferentialTrajectory(_message.Message):
    __slots__ = ("samples",)
    SAMPLES_FIELD_NUMBER: _ClassVar[int]
    samples: _containers.RepeatedCompositeFieldContainer[ProtobufDifferentialSample]
    def __init__(self, samples: _Optional[_Iterable[_Union[ProtobufDifferentialSample, _Mapping]]] = ...) -> None: ...
