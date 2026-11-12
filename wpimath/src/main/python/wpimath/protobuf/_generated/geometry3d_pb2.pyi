from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class ProtobufTranslation3d(_message.Message):
    __slots__ = ("x", "y", "z")
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    Z_FIELD_NUMBER: _ClassVar[int]
    x: float
    y: float
    z: float
    def __init__(self, x: _Optional[float] = ..., y: _Optional[float] = ..., z: _Optional[float] = ...) -> None: ...

class ProtobufQuaternion(_message.Message):
    __slots__ = ("w", "x", "y", "z")
    W_FIELD_NUMBER: _ClassVar[int]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    Z_FIELD_NUMBER: _ClassVar[int]
    w: float
    x: float
    y: float
    z: float
    def __init__(self, w: _Optional[float] = ..., x: _Optional[float] = ..., y: _Optional[float] = ..., z: _Optional[float] = ...) -> None: ...

class ProtobufRotation3d(_message.Message):
    __slots__ = ("q",)
    Q_FIELD_NUMBER: _ClassVar[int]
    q: ProtobufQuaternion
    def __init__(self, q: _Optional[_Union[ProtobufQuaternion, _Mapping]] = ...) -> None: ...

class ProtobufPose3d(_message.Message):
    __slots__ = ("translation", "rotation")
    TRANSLATION_FIELD_NUMBER: _ClassVar[int]
    ROTATION_FIELD_NUMBER: _ClassVar[int]
    translation: ProtobufTranslation3d
    rotation: ProtobufRotation3d
    def __init__(self, translation: _Optional[_Union[ProtobufTranslation3d, _Mapping]] = ..., rotation: _Optional[_Union[ProtobufRotation3d, _Mapping]] = ...) -> None: ...

class ProtobufTransform3d(_message.Message):
    __slots__ = ("translation", "rotation")
    TRANSLATION_FIELD_NUMBER: _ClassVar[int]
    ROTATION_FIELD_NUMBER: _ClassVar[int]
    translation: ProtobufTranslation3d
    rotation: ProtobufRotation3d
    def __init__(self, translation: _Optional[_Union[ProtobufTranslation3d, _Mapping]] = ..., rotation: _Optional[_Union[ProtobufRotation3d, _Mapping]] = ...) -> None: ...

class ProtobufTwist3d(_message.Message):
    __slots__ = ("dx", "dy", "dz", "rx", "ry", "rz")
    DX_FIELD_NUMBER: _ClassVar[int]
    DY_FIELD_NUMBER: _ClassVar[int]
    DZ_FIELD_NUMBER: _ClassVar[int]
    RX_FIELD_NUMBER: _ClassVar[int]
    RY_FIELD_NUMBER: _ClassVar[int]
    RZ_FIELD_NUMBER: _ClassVar[int]
    dx: float
    dy: float
    dz: float
    rx: float
    ry: float
    rz: float
    def __init__(self, dx: _Optional[float] = ..., dy: _Optional[float] = ..., dz: _Optional[float] = ..., rx: _Optional[float] = ..., ry: _Optional[float] = ..., rz: _Optional[float] = ...) -> None: ...
