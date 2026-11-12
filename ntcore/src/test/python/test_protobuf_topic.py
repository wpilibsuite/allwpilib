from google.protobuf import descriptor_pb2, descriptor_pool, message_factory

import ntcore
from ntcore import protobuf as ntproto
from wpiutil.protobuf import Protobuf


def _make_messages():
    """Builds test/inner.proto and test/outer.proto (which imports it) in a private pool."""
    F = descriptor_pb2.FieldDescriptorProto

    inner = descriptor_pb2.FileDescriptorProto(
        name="test/inner.proto", package="test", syntax="proto3"
    )
    m = inner.message_type.add(name="Inner")
    m.field.add(name="x", number=1, type=F.TYPE_INT32, label=F.LABEL_OPTIONAL)

    outer = descriptor_pb2.FileDescriptorProto(
        name="test/outer.proto",
        package="test",
        syntax="proto3",
        dependency=["test/inner.proto"],
    )
    m = outer.message_type.add(name="Outer")
    m.field.add(name="name", number=1, type=F.TYPE_STRING, label=F.LABEL_OPTIONAL)
    m.field.add(
        name="inner",
        number=2,
        type=F.TYPE_MESSAGE,
        type_name=".test.Inner",
        label=F.LABEL_OPTIONAL,
    )

    pool = descriptor_pool.DescriptorPool()
    pool.Add(inner)
    pool.Add(outer)
    return message_factory.GetMessageClass(pool.FindMessageTypeByName("test.Outer"))


Outer = _make_messages()


def _schema(nt: ntcore.NetworkTableInstance, name: str):
    sub = nt.get_raw_topic(f"/.schema/{name}").subscribe(
        "proto:FileDescriptorProto", b""
    )
    return descriptor_pb2.FileDescriptorProto.FromString(sub.get())


def test_exported_from_ntcore():
    for name in (
        "ProtobufTopic",
        "ProtobufPublisher",
        "ProtobufSubscriber",
        "TimestampedProtobuf",
        "add_protobuf_schema",
        "get_protobuf_topic",
    ):
        assert getattr(ntcore, name) is getattr(ntproto, name)
        assert name in ntcore.__all__


def test_type_string(nt: ntcore.NetworkTableInstance):
    topic = ntproto.get_protobuf_topic(nt, "outer", Outer)
    assert topic.get_type_string() == "proto:test.Outer"


def test_roundtrip(nt: ntcore.NetworkTableInstance):
    topic = ntproto.get_protobuf_topic(nt, "outer", Outer)
    pub = topic.publish()
    sub = topic.subscribe(Outer(name="default"))

    assert sub.get().name == "default"

    value = Outer(name="hello")
    value.inner.x = 5
    pub.set(value)

    assert sub.get() == value
    atomic = sub.get_atomic()
    assert atomic.value == value
    assert atomic.time != 0

    vals = sub.read_queue()
    assert [v.value for v in vals] == [value]


def test_set_default(nt: ntcore.NetworkTableInstance):
    topic = ntproto.get_protobuf_topic(nt, "outer", Outer)
    pub = topic.publish()
    sub = topic.subscribe(Outer())

    pub.set_default(Outer(name="dflt"))
    assert sub.get().name == "dflt"


def test_schemas_registered_with_dependencies(nt: ntcore.NetworkTableInstance):
    topic = ntproto.get_protobuf_topic(nt, "outer", Outer)
    assert not nt.has_schema("proto:test/outer.proto")
    topic.publish()

    assert nt.has_schema("proto:test/outer.proto")
    assert nt.has_schema("proto:test/inner.proto")

    outer = _schema(nt, "proto:test/outer.proto")
    assert outer.name == "test/outer.proto"
    assert list(outer.dependency) == ["test/inner.proto"]
    assert _schema(nt, "proto:test/inner.proto").message_type[0].name == "Inner"


def test_unparseable_value_returns_default(nt: ntcore.NetworkTableInstance):
    topic = ntproto.get_protobuf_topic(nt, "outer", Outer)
    raw_pub = nt.get_raw_topic("outer").publish(topic.get_type_string())
    sub = topic.subscribe(Outer(name="default"))

    # field 1 (string) declares 10 bytes but only has 1
    raw_pub.set(b"\x0a\x0a\x41")

    assert sub.get().name == "default"
    assert sub.read_queue() == []


class OuterProto(Protobuf[tuple[str, int], Outer]):
    """Serializes a (name, x) tuple, and rejects a negative x on the way in."""

    message_type = Outer

    @classmethod
    def pack(cls, msg, value):
        msg.name, msg.inner.x = value

    @classmethod
    def unpack(cls, msg):
        if msg.inner.x < 0:
            raise ValueError("x must not be negative")
        return (msg.name, msg.inner.x)


def test_serializer_roundtrip(nt: ntcore.NetworkTableInstance):
    topic = ntproto.get_protobuf_topic(nt, "tuple", OuterProto)
    assert topic.get_type_string() == "proto:test.Outer"

    pub = topic.publish()
    sub = topic.subscribe(("default", 0))
    assert sub.get() == ("default", 0)

    pub.set(("hello", 5))
    assert sub.get() == ("hello", 5)
    assert nt.has_schema("proto:test/outer.proto")


def test_serializer_rejected_values_are_dropped(nt: ntcore.NetworkTableInstance):
    topic = ntproto.get_protobuf_topic(nt, "tuple", OuterProto)
    pub = topic.publish()
    sub = topic.subscribe(("default", 0))

    pub.set(("bad", -1))
    pub.set(("good", 2))

    assert sub.get() == ("good", 2)
    assert [v.value for v in sub.read_queue()] == [("good", 2)]
