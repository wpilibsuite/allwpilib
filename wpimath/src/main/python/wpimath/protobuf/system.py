from wpiutil.protobuf import Protobuf

from .. import (
    DCMotor,
    LinearSystem_1_1_1,
    LinearSystem_1_1_2,
    LinearSystem_1_1_3,
    LinearSystem_2_1_1,
    LinearSystem_2_1_2,
    LinearSystem_2_1_3,
    LinearSystem_2_2_1,
    LinearSystem_2_2_2,
    LinearSystem_2_2_3,
    LinearSystem_3_2_1,
    LinearSystem_3_2_2,
    LinearSystem_3_2_3,
)
from ._generated import system_pb2
from ._util import pack_nested
from .linalg import MatrixProto

__all__ = ["DCMotorProto", "LinearSystemProto"]


class DCMotorProto(Protobuf[DCMotor, system_pb2.ProtobufDCMotor]):
    message_type = system_pb2.ProtobufDCMotor

    @classmethod
    def pack(cls, msg, value):
        msg.nominal_voltage = value.nominal_voltage
        msg.stall_torque = value.stall_torque
        msg.stall_current = value.stall_current
        msg.free_current = value.free_current
        msg.free_speed = value.free_speed

    @classmethod
    def unpack(cls, msg):
        return DCMotor(
            msg.nominal_voltage,
            msg.stall_torque,
            msg.stall_current,
            msg.free_current,
            msg.free_speed,
        )


#: The LinearSystem classes the Python bindings provide, keyed by (states, inputs, outputs).
_LINEAR_SYSTEMS = {
    (1, 1, 1): LinearSystem_1_1_1,
    (1, 1, 2): LinearSystem_1_1_2,
    (1, 1, 3): LinearSystem_1_1_3,
    (2, 1, 1): LinearSystem_2_1_1,
    (2, 1, 2): LinearSystem_2_1_2,
    (2, 1, 3): LinearSystem_2_1_3,
    (2, 2, 1): LinearSystem_2_2_1,
    (2, 2, 2): LinearSystem_2_2_2,
    (2, 2, 3): LinearSystem_2_2_3,
    (3, 2, 1): LinearSystem_3_2_1,
    (3, 2, 2): LinearSystem_3_2_2,
    (3, 2, 3): LinearSystem_3_2_3,
}

_SIZES = {system: key for key, system in _LINEAR_SYSTEMS.items()}

LinearSystem = (
    LinearSystem_1_1_1
    | LinearSystem_1_1_2
    | LinearSystem_1_1_3
    | LinearSystem_2_1_1
    | LinearSystem_2_1_2
    | LinearSystem_2_1_3
    | LinearSystem_2_2_1
    | LinearSystem_2_2_2
    | LinearSystem_2_2_3
    | LinearSystem_3_2_1
    | LinearSystem_3_2_2
    | LinearSystem_3_2_3
)


class LinearSystemProto(Protobuf[LinearSystem, system_pb2.ProtobufLinearSystem]):
    """Works for every (states, inputs, outputs) combination the bindings provide."""

    message_type = system_pb2.ProtobufLinearSystem

    @classmethod
    def pack(cls, msg, value):
        import numpy as np

        states, inputs, outputs = _SIZES[type(value)]
        msg.num_states = states
        msg.num_inputs = inputs
        msg.num_outputs = outputs
        # The bindings return Eigen vectors (and 1x1 matrices) as 1-D arrays or scalars, so
        # restore the 2-D shape from the system's known size.
        pack_nested(msg.a, MatrixProto, np.reshape(value.a(), (states, states)))
        pack_nested(msg.b, MatrixProto, np.reshape(value.b(), (states, inputs)))
        pack_nested(msg.c, MatrixProto, np.reshape(value.c(), (outputs, states)))
        pack_nested(msg.d, MatrixProto, np.reshape(value.d(), (outputs, inputs)))

    @classmethod
    def unpack(cls, msg):
        key = (msg.num_states, msg.num_inputs, msg.num_outputs)
        system = _LINEAR_SYSTEMS.get(key)
        if system is None:
            raise ValueError(
                f"No LinearSystem class for {key[0]} states, {key[1]} inputs and "
                f"{key[2]} outputs; supported sizes are {sorted(_LINEAR_SYSTEMS)}"
            )
        return system(
            MatrixProto.unpack(msg.a),
            MatrixProto.unpack(msg.b),
            MatrixProto.unpack(msg.c),
            MatrixProto.unpack(msg.d),
        )
