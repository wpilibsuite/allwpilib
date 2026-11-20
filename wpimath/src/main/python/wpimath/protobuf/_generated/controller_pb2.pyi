from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Optional as _Optional

DESCRIPTOR: _descriptor.FileDescriptor

class ProtobufArmFeedforward(_message.Message):
    __slots__ = ("ks", "kg", "kv", "ka", "dt")
    KS_FIELD_NUMBER: _ClassVar[int]
    KG_FIELD_NUMBER: _ClassVar[int]
    KV_FIELD_NUMBER: _ClassVar[int]
    KA_FIELD_NUMBER: _ClassVar[int]
    DT_FIELD_NUMBER: _ClassVar[int]
    ks: float
    kg: float
    kv: float
    ka: float
    dt: float
    def __init__(self, ks: _Optional[float] = ..., kg: _Optional[float] = ..., kv: _Optional[float] = ..., ka: _Optional[float] = ..., dt: _Optional[float] = ...) -> None: ...

class ProtobufDifferentialDriveFeedforward(_message.Message):
    __slots__ = ("kvLinear", "kaLinear", "kvAngular", "kaAngular")
    KVLINEAR_FIELD_NUMBER: _ClassVar[int]
    KALINEAR_FIELD_NUMBER: _ClassVar[int]
    KVANGULAR_FIELD_NUMBER: _ClassVar[int]
    KAANGULAR_FIELD_NUMBER: _ClassVar[int]
    kvLinear: float
    kaLinear: float
    kvAngular: float
    kaAngular: float
    def __init__(self, kvLinear: _Optional[float] = ..., kaLinear: _Optional[float] = ..., kvAngular: _Optional[float] = ..., kaAngular: _Optional[float] = ...) -> None: ...

class ProtobufElevatorFeedforward(_message.Message):
    __slots__ = ("ks", "kg", "kv", "ka", "dt")
    KS_FIELD_NUMBER: _ClassVar[int]
    KG_FIELD_NUMBER: _ClassVar[int]
    KV_FIELD_NUMBER: _ClassVar[int]
    KA_FIELD_NUMBER: _ClassVar[int]
    DT_FIELD_NUMBER: _ClassVar[int]
    ks: float
    kg: float
    kv: float
    ka: float
    dt: float
    def __init__(self, ks: _Optional[float] = ..., kg: _Optional[float] = ..., kv: _Optional[float] = ..., ka: _Optional[float] = ..., dt: _Optional[float] = ...) -> None: ...

class ProtobufSimpleMotorFeedforward(_message.Message):
    __slots__ = ("ks", "kv", "ka", "dt")
    KS_FIELD_NUMBER: _ClassVar[int]
    KV_FIELD_NUMBER: _ClassVar[int]
    KA_FIELD_NUMBER: _ClassVar[int]
    DT_FIELD_NUMBER: _ClassVar[int]
    ks: float
    kv: float
    ka: float
    dt: float
    def __init__(self, ks: _Optional[float] = ..., kv: _Optional[float] = ..., ka: _Optional[float] = ..., dt: _Optional[float] = ...) -> None: ...

class ProtobufDifferentialDriveWheelVoltages(_message.Message):
    __slots__ = ("left", "right")
    LEFT_FIELD_NUMBER: _ClassVar[int]
    RIGHT_FIELD_NUMBER: _ClassVar[int]
    left: float
    right: float
    def __init__(self, left: _Optional[float] = ..., right: _Optional[float] = ...) -> None: ...
