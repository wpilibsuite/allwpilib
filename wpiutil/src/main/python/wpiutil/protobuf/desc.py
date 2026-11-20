import types
from abc import ABC, abstractmethod
from collections.abc import Iterator
from typing import ClassVar, Union, get_args, get_origin

from google.protobuf.message import Message

from .typing import ATTRIBUTE


class ProtobufDescriptor[T, M: Message](ABC):
    """
    Converts between a Python type ``T`` and its protobuf wire message ``M``.

    Implementations are stateless, so the class itself is what gets passed around; there is
    nothing to instantiate.

    A type is serializable as protobuf when it has an attribute ``WPIProto`` that contains
    one of these classes. A subclass that names its value type, as in
    ``class Pose2dProto(ProtobufDescriptor[Pose2d, ...])``, is stored there automatically
    (see :func:`register_serializer`). Unions register every member. Value types that can't
    be named by a class, such as a string annotation or a generic alias, are not registered.
    """

    #: The generated message class this serializer reads and writes.
    message_type: ClassVar[type[Message]]

    def __init_subclass__(cls, **kwargs) -> None:
        super().__init_subclass__(**kwargs)
        for base in getattr(cls, "__orig_bases__", ()):
            if get_origin(base) is ProtobufDescriptor:
                for value_type in _classes_in(get_args(base)[0]):
                    register_serializer(value_type, cls)

    @classmethod
    @abstractmethod
    def pack(cls, msg: M, value: T) -> None:
        """Fills ``msg`` (which may be a live nested field of another message) from ``value``."""

    @classmethod
    @abstractmethod
    def unpack(cls, msg: M) -> T:
        """Builds a ``T`` from ``msg``."""

    @classmethod
    def serialize(cls, value: T) -> bytes:
        """Returns the wire-format bytes for ``value``."""
        msg = cls.message_type()
        cls.pack(msg, value)  # type: ignore[arg-type]
        return msg.SerializeToString()

    @classmethod
    def deserialize(cls, data: bytes) -> T:
        """Parses wire-format bytes into a ``T``. Raises ``DecodeError`` on malformed input."""
        return cls.unpack(cls.message_type.FromString(data))  # type: ignore[arg-type]


def _classes_in(annotation: object) -> Iterator[type]:
    """Yields the classes named by a type argument: itself, or the members of a union."""
    if isinstance(annotation, type):
        yield annotation
    elif get_origin(annotation) in (Union, types.UnionType):
        for member in get_args(annotation):
            yield from _classes_in(member)


def register_serializer(value_type: type, serializer: type[ProtobufDescriptor]) -> None:
    """
    Makes ``serializer`` the one :func:`wpiutil.protobuf.get_serializer` returns for
    ``value_type`` by storing it as the ``WPIProto`` class attribute. Subclasses of
    ``value_type`` inherit it.

    :raises TypeError: if ``value_type`` doesn't allow attributes to be set, as with the
        built-in types
    """
    try:
        setattr(value_type, ATTRIBUTE, serializer)
    except TypeError as e:
        raise TypeError(
            f"can't register a protobuf serializer for {value_type!r}: {e}"
        ) from e
