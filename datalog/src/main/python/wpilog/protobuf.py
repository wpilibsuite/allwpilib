"""
Log protobuf data to a DataLog.

An entry is bound to a :class:`wpiutil.protobuf.ProtobufDescriptor` serializer, which converts between a
Python type and its wire message::

    from wpimath import Pose2dProto

    entry = wpilog.ProtobufLogEntry(log, "/pose", Pose2dProto)
    entry.append(Pose2d(1.0, 2.0, Rotation2d(0.5)))

A generated message class can be passed in place of a serializer, in which case the values
are the messages themselves.

Creating an entry writes the schemas for the message's .proto file and everything it imports
to the log.
"""

from typing import overload

from google.protobuf.message import Message
from wpiutil import protobuf as _proto
from wpiutil.protobuf import ProtobufDescriptor

from ._wpilog import DataLog, RawLogEntry

__all__ = ["ProtobufLogEntry", "add_protobuf_schema"]


def add_protobuf_schema(
    log: DataLog, msg_type: type[Message], timestamp: int = 0
) -> None:
    """
    Writes the schemas for a protobuf message class (its .proto file and all imports) to the
    log. Schemas that are already in the log are skipped.

    :param timestamp: timestamp in nanoseconds; 0 means now
    """
    _proto.for_each_descriptor(
        msg_type,
        log.has_schema,
        lambda name, schema: log.add_schema(
            name, _proto.SCHEMA_TYPE, schema, timestamp
        ),
    )


class ProtobufLogEntry[T, M: Message]:
    """A log entry that records values of one protobuf serializer's type."""

    @overload
    def __init__(
        self,
        log: DataLog,
        name: str,
        serializer: type[ProtobufDescriptor[T, M]],
        *,
        metadata: str = "",
        timestamp: int = 0,
    ) -> None: ...

    @overload
    def __init__(
        self: "ProtobufLogEntry[M, M]",
        log: DataLog,
        name: str,
        serializer: type[M],
        *,
        metadata: str = "",
        timestamp: int = 0,
    ) -> None: ...

    def __init__(
        self,
        log: DataLog,
        name: str,
        serializer: type[ProtobufDescriptor] | type[Message],
        *,
        metadata: str = "",
        timestamp: int = 0,
    ) -> None:
        """
        :param log: log to write to
        :param name: name of the entry
        :param serializer: a :class:`~wpiutil.protobuf.ProtobufDescriptor` serializer class, or a
            generated message class to record the messages themselves
        :param metadata: initial metadata for the entry
        :param timestamp: timestamp in nanoseconds; 0 means now
        """
        if not issubclass(serializer, ProtobufDescriptor):
            serializer = _proto.message_serializer(serializer)
        self._serializer = serializer
        add_protobuf_schema(log, serializer.message_type, timestamp)
        self._entry = RawLogEntry(
            log,
            name,
            metadata,
            _proto.get_type_string(serializer.message_type),
            timestamp,
        )

    def append(self, value: T, timestamp: int = 0) -> None:
        """
        Appends a record to the log.

        :param timestamp: timestamp in nanoseconds; 0 means now
        """
        self._entry.append(self._serializer.serialize(value), timestamp)

    def update(self, value: T, timestamp: int = 0) -> None:
        """
        Appends a record to the log only if the serialized value differs from the last value
        passed to this method on this entry.

        :param timestamp: timestamp in nanoseconds; 0 means now
        """
        self._entry.update(self._serializer.serialize(value), timestamp)

    def has_last_value(self) -> bool:
        """Returns whether :meth:`update` has recorded a value."""
        return self._entry.has_last_value()

    def get_last_value(self) -> T | None:
        """Returns the last value recorded by :meth:`update`, or None if there isn't one."""
        raw = self._entry.get_last_value()
        return None if raw is None else self._serializer.deserialize(bytes(raw))

    def set_metadata(self, metadata: str, timestamp: int = 0) -> None:
        """Updates the entry's metadata."""
        self._entry.set_metadata(metadata, timestamp)

    def finish(self, timestamp: int = 0) -> None:
        """Ends the entry; no more records can be appended."""
        self._entry.finish(timestamp)
