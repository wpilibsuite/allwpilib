from typing import TYPE_CHECKING, ClassVar, Final, Protocol, TypeGuard

if TYPE_CHECKING:
    from .desc import ProtobufDescriptor

#: Name of the class attribute that holds a type's :class:`ProtobufDescriptor`.
ATTRIBUTE: Final = "WPIProto"


class ProtobufSerializable(Protocol):
    """Any type that can be serialized or deserialized as a protobuf message."""

    WPIProto: ClassVar[type["ProtobufDescriptor"]]


def is_protobuf_serializable(cls: type) -> TypeGuard[type[ProtobufSerializable]]:
    """Returns True if the given type has a protobuf serializer."""
    return hasattr(cls, ATTRIBUTE)
