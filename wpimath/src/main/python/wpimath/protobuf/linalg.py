from typing import TYPE_CHECKING

from wpiutil.protobuf import ProtobufDescriptor

from ._generated import wpimath_pb2

if TYPE_CHECKING:
    import numpy as np
    import numpy.typing as npt

__all__ = ["MatrixProto", "VectorProto"]

# numpy is imported inside the methods so that importing wpimath.protobuf doesn't require it.


class MatrixProto(
    ProtobufDescriptor["npt.NDArray[np.float64]", wpimath_pb2.ProtobufMatrix]
):
    """
    A 2-D array of any shape. ``data`` is column-major, the storage order of the matrices
    returned by the C++ bindings (Eigen's default).
    """

    message_type = wpimath_pb2.ProtobufMatrix

    @classmethod
    def pack(cls, msg, value):
        import numpy as np

        array = np.asarray(value, dtype=np.float64)
        if array.ndim != 2:
            raise ValueError(f"Expected a 2-D array, got {array.ndim} dimensions")
        msg.num_rows, msg.num_cols = array.shape
        msg.data.extend(array.ravel(order="F").tolist())

    @classmethod
    def unpack(cls, msg):
        import numpy as np

        if len(msg.data) != msg.num_rows * msg.num_cols:
            raise ValueError(
                f"Matrix message has {len(msg.data)} values for a "
                f"{msg.num_rows}x{msg.num_cols} matrix"
            )
        return np.array(msg.data, dtype=np.float64).reshape(
            (msg.num_rows, msg.num_cols), order="F"
        )


class VectorProto(
    ProtobufDescriptor["npt.NDArray[np.float64]", wpimath_pb2.ProtobufVector]
):
    """A column vector. Accepts a 1-D array or an ``(n, 1)`` array; unpacks to ``(n, 1)``."""

    message_type = wpimath_pb2.ProtobufVector

    @classmethod
    def pack(cls, msg, value):
        import numpy as np

        array = np.asarray(value, dtype=np.float64)
        if array.ndim == 2 and array.shape[1] == 1:
            array = array[:, 0]
        if array.ndim != 1:
            raise ValueError(f"Expected a vector, got an array of shape {array.shape}")
        msg.rows.extend(array.tolist())

    @classmethod
    def unpack(cls, msg):
        import numpy as np

        return np.array(msg.rows, dtype=np.float64).reshape((-1, 1))
