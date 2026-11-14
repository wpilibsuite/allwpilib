from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Iterable as _Iterable
from typing import ClassVar as _ClassVar, Optional as _Optional

DESCRIPTOR: _descriptor.FileDescriptor

class ProtobufCubicHermiteSpline(_message.Message):
    __slots__ = ("x_initial", "x_final", "y_initial", "y_final")
    X_INITIAL_FIELD_NUMBER: _ClassVar[int]
    X_FINAL_FIELD_NUMBER: _ClassVar[int]
    Y_INITIAL_FIELD_NUMBER: _ClassVar[int]
    Y_FINAL_FIELD_NUMBER: _ClassVar[int]
    x_initial: _containers.RepeatedScalarFieldContainer[float]
    x_final: _containers.RepeatedScalarFieldContainer[float]
    y_initial: _containers.RepeatedScalarFieldContainer[float]
    y_final: _containers.RepeatedScalarFieldContainer[float]
    def __init__(self, x_initial: _Optional[_Iterable[float]] = ..., x_final: _Optional[_Iterable[float]] = ..., y_initial: _Optional[_Iterable[float]] = ..., y_final: _Optional[_Iterable[float]] = ...) -> None: ...

class ProtobufQuinticHermiteSpline(_message.Message):
    __slots__ = ("x_initial", "x_final", "y_initial", "y_final")
    X_INITIAL_FIELD_NUMBER: _ClassVar[int]
    X_FINAL_FIELD_NUMBER: _ClassVar[int]
    Y_INITIAL_FIELD_NUMBER: _ClassVar[int]
    Y_FINAL_FIELD_NUMBER: _ClassVar[int]
    x_initial: _containers.RepeatedScalarFieldContainer[float]
    x_final: _containers.RepeatedScalarFieldContainer[float]
    y_initial: _containers.RepeatedScalarFieldContainer[float]
    y_final: _containers.RepeatedScalarFieldContainer[float]
    def __init__(self, x_initial: _Optional[_Iterable[float]] = ..., x_final: _Optional[_Iterable[float]] = ..., y_initial: _Optional[_Iterable[float]] = ..., y_final: _Optional[_Iterable[float]] = ...) -> None: ...
