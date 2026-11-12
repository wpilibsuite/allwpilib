from . import wpimath_pb2 as _wpimath_pb2
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class ProtobufDCMotor(_message.Message):
    __slots__ = ("nominal_voltage", "stall_torque", "stall_current", "free_current", "free_speed")
    NOMINAL_VOLTAGE_FIELD_NUMBER: _ClassVar[int]
    STALL_TORQUE_FIELD_NUMBER: _ClassVar[int]
    STALL_CURRENT_FIELD_NUMBER: _ClassVar[int]
    FREE_CURRENT_FIELD_NUMBER: _ClassVar[int]
    FREE_SPEED_FIELD_NUMBER: _ClassVar[int]
    nominal_voltage: float
    stall_torque: float
    stall_current: float
    free_current: float
    free_speed: float
    def __init__(self, nominal_voltage: _Optional[float] = ..., stall_torque: _Optional[float] = ..., stall_current: _Optional[float] = ..., free_current: _Optional[float] = ..., free_speed: _Optional[float] = ...) -> None: ...

class ProtobufLinearSystem(_message.Message):
    __slots__ = ("num_states", "num_inputs", "num_outputs", "a", "b", "c", "d")
    NUM_STATES_FIELD_NUMBER: _ClassVar[int]
    NUM_INPUTS_FIELD_NUMBER: _ClassVar[int]
    NUM_OUTPUTS_FIELD_NUMBER: _ClassVar[int]
    A_FIELD_NUMBER: _ClassVar[int]
    B_FIELD_NUMBER: _ClassVar[int]
    C_FIELD_NUMBER: _ClassVar[int]
    D_FIELD_NUMBER: _ClassVar[int]
    num_states: int
    num_inputs: int
    num_outputs: int
    a: _wpimath_pb2.ProtobufMatrix
    b: _wpimath_pb2.ProtobufMatrix
    c: _wpimath_pb2.ProtobufMatrix
    d: _wpimath_pb2.ProtobufMatrix
    def __init__(self, num_states: _Optional[int] = ..., num_inputs: _Optional[int] = ..., num_outputs: _Optional[int] = ..., a: _Optional[_Union[_wpimath_pb2.ProtobufMatrix, _Mapping]] = ..., b: _Optional[_Union[_wpimath_pb2.ProtobufMatrix, _Mapping]] = ..., c: _Optional[_Union[_wpimath_pb2.ProtobufMatrix, _Mapping]] = ..., d: _Optional[_Union[_wpimath_pb2.ProtobufMatrix, _Mapping]] = ...) -> None: ...
