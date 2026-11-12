import functools
from collections.abc import Callable

from google.protobuf.descriptor import FileDescriptor
from google.protobuf.message import Message

from .desc import ProtobufDescriptor
from .typing import ATTRIBUTE

#: Type string of a registered protobuf schema (a serialized FileDescriptorProto).
SCHEMA_TYPE = "proto:FileDescriptorProto"


def get_serializer(value_type: type) -> type[ProtobufDescriptor] | None:
    """
    Returns the serializer for values of ``value_type``, or None if there isn't one.

    This is the type's ``WPIProto`` attribute. A generated message class always has a
    serializer: :func:`message_serializer`.
    """
    serializer = getattr(value_type, ATTRIBUTE, None)
    if serializer is not None:
        return serializer
    if issubclass(value_type, Message):
        return message_serializer(value_type)
    return None


@functools.cache
def message_serializer[M: Message](
    msg_type: type[M],
) -> type[ProtobufDescriptor[M, M]]:
    """
    Returns a serializer whose value type is the message itself, for publishing generated
    messages that have no separate Python representation.
    """

    class MessageSerializer(ProtobufDescriptor[M, M]):
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
