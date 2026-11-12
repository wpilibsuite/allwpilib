"""
This package contains the helpers for sending protobuf messages over WPILib's data
protocols (NetworkTables, DataLog and telemetry), and a mechanism to implement your own
custom serializers using Python (see :class:`wpiutil.protobuf.ProtobufDescriptor`).

Each Python type's serializer is stored as a ``WPIProto`` class attribute, like
``WPIStruct`` for structs, so that generic code can find it with
:func:`wpiutil.protobuf.get_serializer`.
"""

from .desc import ProtobufDescriptor, register_serializer
from .typing import ProtobufSerializable, is_protobuf_serializable

from .serializer import (
    SCHEMA_TYPE,
    for_each_descriptor,
    get_schema_name,
    get_serializer,
    get_type_string,
    message_serializer,
)

__all__ = [
    "ProtobufDescriptor",
    "register_serializer",
    "ProtobufSerializable",
    "is_protobuf_serializable",
    "SCHEMA_TYPE",
    "for_each_descriptor",
    "get_schema_name",
    "get_serializer",
    "get_type_string",
    "message_serializer",
]
