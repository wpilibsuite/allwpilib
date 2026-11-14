from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Iterable as _Iterable
from typing import ClassVar as _ClassVar, Optional as _Optional

DESCRIPTOR: _descriptor.FileDescriptor

class ProtobufMatrix(_message.Message):
    __slots__ = ("num_rows", "num_cols", "data")
    NUM_ROWS_FIELD_NUMBER: _ClassVar[int]
    NUM_COLS_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    num_rows: int
    num_cols: int
    data: _containers.RepeatedScalarFieldContainer[float]
    def __init__(self, num_rows: _Optional[int] = ..., num_cols: _Optional[int] = ..., data: _Optional[_Iterable[float]] = ...) -> None: ...

class ProtobufVector(_message.Message):
    __slots__ = ("rows",)
    ROWS_FIELD_NUMBER: _ClassVar[int]
    rows: _containers.RepeatedScalarFieldContainer[float]
    def __init__(self, rows: _Optional[_Iterable[float]] = ...) -> None: ...
