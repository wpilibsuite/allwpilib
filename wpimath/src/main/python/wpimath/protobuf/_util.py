from typing import TypeVar

from google.protobuf.message import Message
from wpiutil.protobuf import ProtobufDescriptor

T = TypeVar("T")
M = TypeVar("M", bound=Message)


def pack_nested(msg: M, serializer: type[ProtobufDescriptor[T, M]], value: T) -> None:
    """
    Packs ``value`` into the nested message field ``msg``.

    proto3 omits a nested message from the wire unless it is marked present, and a submessage
    whose fields are all zero (the origin, a zero rotation, ...) would otherwise be dropped.
    Decoders that require every nested field to be present would reject such a message, so
    the field is always marked.
    """
    msg.SetInParent()
    serializer.pack(msg, value)
