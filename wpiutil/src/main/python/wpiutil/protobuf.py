"""
Helpers for publishing protobuf messages from Python.

It computes the NetworkTables/DataLog type string for a message class, walks the message's
file descriptors so their schemas can be registered, and keeps a registry that maps Python
types to their serializers so that generic code (such as telemetry) can find one.
"""

import functools
import types
from abc import ABC, abstractmethod
from collections.abc import Callable, Iterator
from typing import ClassVar, Union, get_args, get_origin

from google.protobuf.descriptor import FileDescriptor
from google.protobuf.message import Message

__all__ = [
    "SCHEMA_TYPE",
    "Protobuf",
    "message_serializer",
    "get_serializer",
    "register_serializer",
    "get_type_string",
    "get_schema_name",
    "for_each_descriptor",
]

#: Type string of a registered protobuf schema (a serialized FileDescriptorProto).
SCHEMA_TYPE = "proto:FileDescriptorProto"


class Protobuf[T, M: Message](ABC):
    """
    Converts between a Python type ``T`` and its protobuf wire message ``M``.

    Implementations are stateless, so the class itself is what gets passed around; there is
    nothing to instantiate.

    A subclass that names its value type, as in ``class Pose2dProto(Protobuf[Pose2d, ...])``,
    is registered for that type (see :func:`get_serializer`). Unions register every member.
    Value types that can't be named by a class, such as a string annotation or a generic
    alias, are not registered.
    """

    #: The generated message class this serializer reads and writes.
    message_type: ClassVar[type[Message]]

    def __init_subclass__(cls, **kwargs) -> None:
        super().__init_subclass__(**kwargs)
        for base in getattr(cls, "__orig_bases__", ()):
            if get_origin(base) is Protobuf:
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


#: Serializers by the Python type they serialize.
_serializers: dict[type, type[Protobuf]] = {}


def _classes_in(annotation: object) -> Iterator[type]:
    """Yields the classes named by a type argument: itself, or the members of a union."""
    if isinstance(annotation, type):
        yield annotation
    elif get_origin(annotation) in (Union, types.UnionType):
        for member in get_args(annotation):
            yield from _classes_in(member)


def register_serializer(value_type: type, serializer: type[Protobuf]) -> None:
    """Makes ``serializer`` the one :func:`get_serializer` returns for ``value_type``."""
    _serializers[value_type] = serializer


def get_serializer(value_type: type) -> type[Protobuf] | None:
    """
    Returns the serializer for values of ``value_type``, or None if there isn't one.

    Registered serializers are found through the type's base classes. A generated message
    class always has one: :func:`message_serializer`.
    """
    for cls in value_type.__mro__:
        serializer = _serializers.get(cls)
        if serializer is not None:
            return serializer
    if issubclass(value_type, Message):
        return message_serializer(value_type)
    return None


@functools.cache
def message_serializer[M: Message](msg_type: type[M]) -> type[Protobuf[M, M]]:
    """
    Returns a serializer whose value type is the message itself, for publishing generated
    messages that have no separate Python representation.
    """

    class MessageSerializer(Protobuf[M, M]):
        message_type = msg_type

        @classmethod
        def pack(cls, msg: M, value: M) -> None:
            msg.CopyFrom(value)

        @classmethod
        def unpack(cls, msg: M) -> M:
            return msg

    MessageSerializer.__name__ = f"{msg_type.__name__}Serializer"
    MessageSerializer.__qualname__ = MessageSerializer.__name__
    return MessageSerializer


def get_type_string(msg_type: type[Message]) -> str:
    """Returns the data type string for a protobuf message class, e.g. ``proto:wpi.proto.ProtobufPose2d``."""
    return f"proto:{msg_type.DESCRIPTOR.full_name}"


def get_schema_name(file_descriptor: FileDescriptor) -> str:
    """Returns the schema name for a .proto file, e.g. ``proto:wpi/math/protobuf/geometry2d.proto``."""
    return f"proto:{file_descriptor.name}"


def for_each_descriptor(
    msg_type: type[Message],
    exists: Callable[[str], bool],
    fn: Callable[[str, bytes], None],
) -> None:
    """
    Calls ``fn(schema_name, serialized_file_descriptor)`` for the message's .proto file and
    every file it imports, dependencies first.

    :param msg_type: protobuf message class
    :param exists: returns True if a schema with that name is already registered; that file
        (and so its dependencies) is skipped
    :param fn: called once per schema that still needs registering
    """
    _walk(msg_type.DESCRIPTOR.file, exists, fn, set())


def _walk(
    file_descriptor: FileDescriptor,
    exists: Callable[[str], bool],
    fn: Callable[[str, bytes], None],
    seen: set[str],
) -> None:
    name = get_schema_name(file_descriptor)
    if name in seen or exists(name):
        return
    seen.add(name)
    for dependency in file_descriptor.dependencies:
        _walk(dependency, exists, fn, seen)
    fn(name, file_descriptor.serialized_pb)
