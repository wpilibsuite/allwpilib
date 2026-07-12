from typing import ClassVar, Protocol, TypeGuard


class StructSerializable(Protocol):
    """Any type that can be serialized or deserialized as a WPILib Struct."""

    WPIStruct: ClassVar


def is_wpistruct_type(cls: type) -> TypeGuard[type[StructSerializable]]:
    """Returns True if the given type supports WPILib Struct serialization."""
    return hasattr(cls, "WPIStruct")
