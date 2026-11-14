from . import geometry2d_pb2 as _geometry2d_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Iterable as _Iterable, Mapping as _Mapping
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class ProtobufChassisVelocities(_message.Message):
    __slots__ = ("vx", "vy", "omega")
    VX_FIELD_NUMBER: _ClassVar[int]
    VY_FIELD_NUMBER: _ClassVar[int]
    OMEGA_FIELD_NUMBER: _ClassVar[int]
    vx: float
    vy: float
    omega: float
    def __init__(self, vx: _Optional[float] = ..., vy: _Optional[float] = ..., omega: _Optional[float] = ...) -> None: ...

class ProtobufChassisAccelerations(_message.Message):
    __slots__ = ("ax", "ay", "alpha")
    AX_FIELD_NUMBER: _ClassVar[int]
    AY_FIELD_NUMBER: _ClassVar[int]
    ALPHA_FIELD_NUMBER: _ClassVar[int]
    ax: float
    ay: float
    alpha: float
    def __init__(self, ax: _Optional[float] = ..., ay: _Optional[float] = ..., alpha: _Optional[float] = ...) -> None: ...

class ProtobufDifferentialDriveKinematics(_message.Message):
    __slots__ = ("trackwidth",)
    TRACKWIDTH_FIELD_NUMBER: _ClassVar[int]
    trackwidth: float
    def __init__(self, trackwidth: _Optional[float] = ...) -> None: ...

class ProtobufDifferentialDriveWheelVelocities(_message.Message):
    __slots__ = ("left", "right")
    LEFT_FIELD_NUMBER: _ClassVar[int]
    RIGHT_FIELD_NUMBER: _ClassVar[int]
    left: float
    right: float
    def __init__(self, left: _Optional[float] = ..., right: _Optional[float] = ...) -> None: ...

class ProtobufDifferentialDriveWheelAccelerations(_message.Message):
    __slots__ = ("left", "right")
    LEFT_FIELD_NUMBER: _ClassVar[int]
    RIGHT_FIELD_NUMBER: _ClassVar[int]
    left: float
    right: float
    def __init__(self, left: _Optional[float] = ..., right: _Optional[float] = ...) -> None: ...

class ProtobufDifferentialDriveWheelPositions(_message.Message):
    __slots__ = ("left", "right")
    LEFT_FIELD_NUMBER: _ClassVar[int]
    RIGHT_FIELD_NUMBER: _ClassVar[int]
    left: float
    right: float
    def __init__(self, left: _Optional[float] = ..., right: _Optional[float] = ...) -> None: ...

class ProtobufMecanumDriveKinematics(_message.Message):
    __slots__ = ("front_left", "front_right", "rear_left", "rear_right")
    FRONT_LEFT_FIELD_NUMBER: _ClassVar[int]
    FRONT_RIGHT_FIELD_NUMBER: _ClassVar[int]
    REAR_LEFT_FIELD_NUMBER: _ClassVar[int]
    REAR_RIGHT_FIELD_NUMBER: _ClassVar[int]
    front_left: _geometry2d_pb2.ProtobufTranslation2d
    front_right: _geometry2d_pb2.ProtobufTranslation2d
    rear_left: _geometry2d_pb2.ProtobufTranslation2d
    rear_right: _geometry2d_pb2.ProtobufTranslation2d
    def __init__(self, front_left: _Optional[_Union[_geometry2d_pb2.ProtobufTranslation2d, _Mapping]] = ..., front_right: _Optional[_Union[_geometry2d_pb2.ProtobufTranslation2d, _Mapping]] = ..., rear_left: _Optional[_Union[_geometry2d_pb2.ProtobufTranslation2d, _Mapping]] = ..., rear_right: _Optional[_Union[_geometry2d_pb2.ProtobufTranslation2d, _Mapping]] = ...) -> None: ...

class ProtobufMecanumDriveWheelPositions(_message.Message):
    __slots__ = ("front_left", "front_right", "rear_left", "rear_right")
    FRONT_LEFT_FIELD_NUMBER: _ClassVar[int]
    FRONT_RIGHT_FIELD_NUMBER: _ClassVar[int]
    REAR_LEFT_FIELD_NUMBER: _ClassVar[int]
    REAR_RIGHT_FIELD_NUMBER: _ClassVar[int]
    front_left: float
    front_right: float
    rear_left: float
    rear_right: float
    def __init__(self, front_left: _Optional[float] = ..., front_right: _Optional[float] = ..., rear_left: _Optional[float] = ..., rear_right: _Optional[float] = ...) -> None: ...

class ProtobufMecanumDriveWheelVelocities(_message.Message):
    __slots__ = ("front_left", "front_right", "rear_left", "rear_right")
    FRONT_LEFT_FIELD_NUMBER: _ClassVar[int]
    FRONT_RIGHT_FIELD_NUMBER: _ClassVar[int]
    REAR_LEFT_FIELD_NUMBER: _ClassVar[int]
    REAR_RIGHT_FIELD_NUMBER: _ClassVar[int]
    front_left: float
    front_right: float
    rear_left: float
    rear_right: float
    def __init__(self, front_left: _Optional[float] = ..., front_right: _Optional[float] = ..., rear_left: _Optional[float] = ..., rear_right: _Optional[float] = ...) -> None: ...

class ProtobufMecanumDriveWheelAccelerations(_message.Message):
    __slots__ = ("front_left", "front_right", "rear_left", "rear_right")
    FRONT_LEFT_FIELD_NUMBER: _ClassVar[int]
    FRONT_RIGHT_FIELD_NUMBER: _ClassVar[int]
    REAR_LEFT_FIELD_NUMBER: _ClassVar[int]
    REAR_RIGHT_FIELD_NUMBER: _ClassVar[int]
    front_left: float
    front_right: float
    rear_left: float
    rear_right: float
    def __init__(self, front_left: _Optional[float] = ..., front_right: _Optional[float] = ..., rear_left: _Optional[float] = ..., rear_right: _Optional[float] = ...) -> None: ...

class ProtobufSwerveDriveKinematics(_message.Message):
    __slots__ = ("modules",)
    MODULES_FIELD_NUMBER: _ClassVar[int]
    modules: _containers.RepeatedCompositeFieldContainer[_geometry2d_pb2.ProtobufTranslation2d]
    def __init__(self, modules: _Optional[_Iterable[_Union[_geometry2d_pb2.ProtobufTranslation2d, _Mapping]]] = ...) -> None: ...

class ProtobufSwerveModulePosition(_message.Message):
    __slots__ = ("distance", "angle")
    DISTANCE_FIELD_NUMBER: _ClassVar[int]
    ANGLE_FIELD_NUMBER: _ClassVar[int]
    distance: float
    angle: _geometry2d_pb2.ProtobufRotation2d
    def __init__(self, distance: _Optional[float] = ..., angle: _Optional[_Union[_geometry2d_pb2.ProtobufRotation2d, _Mapping]] = ...) -> None: ...

class ProtobufSwerveModuleVelocity(_message.Message):
    __slots__ = ("velocity", "angle")
    VELOCITY_FIELD_NUMBER: _ClassVar[int]
    ANGLE_FIELD_NUMBER: _ClassVar[int]
    velocity: float
    angle: _geometry2d_pb2.ProtobufRotation2d
    def __init__(self, velocity: _Optional[float] = ..., angle: _Optional[_Union[_geometry2d_pb2.ProtobufRotation2d, _Mapping]] = ...) -> None: ...

class ProtobufSwerveModuleAcceleration(_message.Message):
    __slots__ = ("acceleration", "angle")
    ACCELERATION_FIELD_NUMBER: _ClassVar[int]
    ANGLE_FIELD_NUMBER: _ClassVar[int]
    acceleration: float
    angle: _geometry2d_pb2.ProtobufRotation2d
    def __init__(self, acceleration: _Optional[float] = ..., angle: _Optional[_Union[_geometry2d_pb2.ProtobufRotation2d, _Mapping]] = ...) -> None: ...
