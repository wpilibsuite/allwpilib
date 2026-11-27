from . import geometry2d_pb2 as _geometry2d_pb2
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Mapping as _Mapping
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class ProtobufRectangle2d(_message.Message):
    __slots__ = ("center", "xWidth", "yWidth")
    CENTER_FIELD_NUMBER: _ClassVar[int]
    XWIDTH_FIELD_NUMBER: _ClassVar[int]
    YWIDTH_FIELD_NUMBER: _ClassVar[int]
    center: _geometry2d_pb2.ProtobufPose2d
    xWidth: float
    yWidth: float
    def __init__(self, center: _Optional[_Union[_geometry2d_pb2.ProtobufPose2d, _Mapping]] = ..., xWidth: _Optional[float] = ..., yWidth: _Optional[float] = ...) -> None: ...

class ProtobufEllipse2d(_message.Message):
    __slots__ = ("center", "xSemiAxis", "ySemiAxis")
    CENTER_FIELD_NUMBER: _ClassVar[int]
    XSEMIAXIS_FIELD_NUMBER: _ClassVar[int]
    YSEMIAXIS_FIELD_NUMBER: _ClassVar[int]
    center: _geometry2d_pb2.ProtobufPose2d
    xSemiAxis: float
    ySemiAxis: float
    def __init__(self, center: _Optional[_Union[_geometry2d_pb2.ProtobufPose2d, _Mapping]] = ..., xSemiAxis: _Optional[float] = ..., ySemiAxis: _Optional[float] = ...) -> None: ...
