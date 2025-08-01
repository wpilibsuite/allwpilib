from typing import ClassVar, Protocol

try:
    from typing import TypeGuard
except ImportError:
    try:
        from typing_extensions import TypeGuard
    except ImportError:
        # Runtime fallback for Python 3.9 without typing_extensions
        class TypeGuard:
            def __class_getitem__(cls, key):
                return bool


class StructSerializable(Protocol):
    """Any type that can be serialized or deserialized as a WPILib Struct."""

    WPIStruct: ClassVar


def is_wpistruct_type(cls: type) -> TypeGuard[type[StructSerializable]]:
    """Returns True if the given type supports WPILib Struct serialization."""
    return hasattr(cls, "WPIStruct")
