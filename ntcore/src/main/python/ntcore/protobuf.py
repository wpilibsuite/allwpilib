"""
Publish and subscribe to protobuf data over NetworkTables.

A topic is bound to a :class:`wpiutil.protobuf.ProtobufDescriptor` serializer, which converts between a
Python type and its wire message::

    from wpimath import Pose2dProto

    topic = ntcore.get_protobuf_topic(inst, "/pose", Pose2dProto)
    topic.publish().set(Pose2d(1.0, 2.0, Rotation2d(0.5)))

A generated message class can be passed in place of a serializer, in which case the values
are the messages themselves::

    topic = ntcore.get_protobuf_topic(inst, "/scheduler", ProtobufScheduler)
    topic.publish().set(ProtobufScheduler(last_time_ms=1.5))

Publishing registers the schemas for the message's .proto file and everything it imports.
"""

import dataclasses
from typing import overload

from google.protobuf.message import DecodeError, Message
from wpiutil import protobuf as _proto
from wpiutil.protobuf import ProtobufDescriptor

from ._ntcore import (
    NetworkTableInstance,
    PubSubOptions,
    RawPublisher,
    RawSubscriber,
    RawTopic,
)

__all__ = [
    "ProtobufTopic",
    "ProtobufPublisher",
    "ProtobufSubscriber",
    "TimestampedProtobuf",
    "get_protobuf_topic",
    "add_protobuf_schema",
]


@dataclasses.dataclass
class TimestampedProtobuf[T]:
    """A decoded value with its local timestamp and the server timestamp, in microseconds."""

    time: int
    server_time: int
    value: T


def add_protobuf_schema(inst: NetworkTableInstance, msg_type: type[Message]) -> None:
    """
    Registers the schemas for a protobuf message class (its .proto file and all imports).
    Duplicate calls are silently ignored.
    """
    _proto.for_each_descriptor(
        msg_type,
        inst.has_schema,
        lambda name, schema: inst.add_schema(name, _proto.SCHEMA_TYPE, schema),
    )


class ProtobufSubscriber[T, M: Message]:
    """Subscribes to a protobuf topic; created by :meth:`ProtobufTopic.subscribe`."""

    def __init__(
        self, sub: RawSubscriber, serializer: type[ProtobufDescriptor[T, M]], default: T
    ):
        self._sub = sub
        self._serializer = serializer
        self._default = default

    def _decode(self, data: bytes) -> T | None:
        # Malformed bytes and values the serializer rejects (wrong sizes, empty trajectories,
        # ...) are both treated as "no valid value".
        try:
            return self._serializer.deserialize(data)
        except (DecodeError, ValueError):
            return None

    def get(self) -> T:
        """Returns the latest value, or the default if there is none or it is invalid."""
        return self.get_atomic().value

    def get_atomic(self) -> TimestampedProtobuf[T]:
        """Like :meth:`get`, but also returns the timestamps."""
        raw = self._sub.get_atomic(b"")
        # Values set with set_default() have time 0, so only an empty value at time 0 means
        # "nothing published".
        if not raw.value and raw.time == 0:
            return TimestampedProtobuf(0, 0, self._default)
        value = self._decode(raw.value)
        return TimestampedProtobuf(
            raw.time, raw.server_time, self._default if value is None else value
        )

    def read_queue(self) -> list[TimestampedProtobuf[T]]:
        """Returns all values received since the last call, dropping any that are invalid."""
        result = []
        for raw in self._sub.read_queue():
            value = self._decode(raw.value)
            if value is not None:
                result.append(TimestampedProtobuf(raw.time, raw.server_time, value))
        return result

    def close(self) -> None:
        self._sub.close()

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()


class ProtobufPublisher[T, M: Message]:
    """Publishes to a protobuf topic; created by :meth:`ProtobufTopic.publish`."""

    def __init__(self, pub: RawPublisher, serializer: type[ProtobufDescriptor[T, M]]):
        self._pub = pub
        self._serializer = serializer

    def set(self, value: T, time: int = 0) -> None:
        """
        Publishes a new value.

        :param value: value to publish
        :param time: timestamp in microseconds; 0 means the current NT time
        """
        self._pub.set(self._serializer.serialize(value), time)

    def set_default(self, value: T) -> None:
        """Sets the value to use if nothing has been published yet."""
        self._pub.set_default(self._serializer.serialize(value))

    def close(self) -> None:
        self._pub.close()

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()


class ProtobufTopic[T, M: Message]:
    """A raw topic that carries values of one protobuf serializer's type."""

    def __init__(
        self,
        inst: NetworkTableInstance,
        topic: RawTopic,
        serializer: type[ProtobufDescriptor[T, M]],
    ):
        self._inst = inst
        self._topic = topic
        self._serializer = serializer
        self._type_string = _proto.get_type_string(serializer.message_type)

    def get_type_string(self) -> str:
        return self._type_string

    def get_topic(self) -> RawTopic:
        """Returns the underlying raw topic."""
        return self._topic

    def publish(
        self,
        options: PubSubOptions | None = None,
        properties: dict | None = None,
    ) -> ProtobufPublisher[T, M]:
        """
        Creates a publisher and registers the message's schemas so subscribers can decode it.

        :param options: publish options
        :param properties: initial topic properties
        """
        add_protobuf_schema(self._inst, self._serializer.message_type)
        args = [] if options is None else [options]
        pub = self._topic.publish_ex(self._type_string, properties or {}, *args)
        return ProtobufPublisher(pub, self._serializer)

    def subscribe(
        self, default: T, options: PubSubOptions | None = None
    ) -> ProtobufSubscriber[T, M]:
        """
        Creates a subscriber.

        :param default: returned when there is no value or it is invalid
        :param options: subscribe options
        """
        args = [] if options is None else [options]
        sub = self._topic.subscribe(self._type_string, b"", *args)
        return ProtobufSubscriber(sub, self._serializer, default)


@overload
def get_protobuf_topic[T, M: Message](
    inst: NetworkTableInstance, name: str, serializer: type[ProtobufDescriptor[T, M]]
) -> ProtobufTopic[T, M]: ...


@overload
def get_protobuf_topic[M: Message](
    inst: NetworkTableInstance, name: str, serializer: type[M]
) -> ProtobufTopic[M, M]: ...


def get_protobuf_topic(
    inst: NetworkTableInstance,
    name: str,
    serializer: type[ProtobufDescriptor] | type[Message],
) -> ProtobufTopic:
    """
    Returns the protobuf topic for ``name``, creating it if needed.

    :param serializer: a :class:`~wpiutil.protobuf.ProtobufDescriptor` serializer class, or a generated
        message class to publish the messages themselves
    """
    if not issubclass(serializer, ProtobufDescriptor):
        serializer = _proto.message_serializer(serializer)
    return ProtobufTopic(inst, inst.get_raw_topic(name), serializer)
