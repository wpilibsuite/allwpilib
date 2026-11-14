import dataclasses

import pytest
from google.protobuf import descriptor_pb2, descriptor_pool, message_factory
from wpiutil import protobuf as wpiproto
from wpiutil import wpistruct
from wpiutil.protobuf import ProtobufDescriptor

import telemetry
import telemetry.mock_backend as mock_backend


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


@dataclasses.dataclass
class Point:
    name: str
    x: int


class PointProto(ProtobufDescriptor[Point, Outer]):
    message_type = Outer

    @classmethod
    def pack(cls, msg, value):
        msg.name = value.name
        msg.inner.x = value.x

    @classmethod
    def unpack(cls, msg):
        return Point(msg.name, msg.inner.x)


@wpistruct.make_wpistruct(name="ProtoTestStruct")
@dataclasses.dataclass
class StructWithSerializer:
    x: wpistruct.double


class StructWithSerializerProto(ProtobufDescriptor[StructWithSerializer, Outer]):
    message_type = Outer

    @classmethod
    def pack(cls, msg, value):
        msg.inner.x = int(value.x)

    @classmethod
    def unpack(cls, msg):
        return StructWithSerializer(float(msg.inner.x))


@pytest.fixture
def backend():
    telemetry.TelemetryRegistry.reset()
    backend = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("", backend)
    try:
        yield backend
    finally:
        telemetry.TelemetryRegistry.set_report_warning(None)
        telemetry.TelemetryRegistry.reset()


def _outer() -> "Outer":
    value = Outer(name="hello")
    value.inner.x = 5
    return value


def _raw(path: str, value: bytes) -> mock_backend.Action:
    return mock_backend.Action(
        path, mock_backend.LogRawValue(value, "proto:test.Outer")
    )


def test_log_generated_message(backend):
    value = _outer()
    telemetry.log("outer", value)

    assert backend.get_last_action("/outer") == _raw(
        "/outer", value.SerializeToString()
    )


def test_table_log_generated_message(backend):
    value = _outer()
    telemetry.get_table("Table").log("outer", value)

    assert backend.get_last_action("/Table/outer") == _raw(
        "/Table/outer", value.SerializeToString()
    )


def test_log_registered_serializer_type(backend):
    assert wpiproto.get_serializer(Point) is PointProto

    telemetry.log("point", Point("hello", 5))

    assert backend.get_last_action("/point") == _raw(
        "/point", _outer().SerializeToString()
    )


def test_schemas_published_with_dependencies(backend):
    telemetry.log("outer", _outer())

    outer = backend.get_schema("proto:test/outer.proto")
    inner = backend.get_schema("proto:test/inner.proto")
    assert outer["type"] == inner["type"] == "proto:FileDescriptorProto"

    outer_file = descriptor_pb2.FileDescriptorProto.FromString(outer["schema_bytes"])
    assert list(outer_file.dependency) == ["test/inner.proto"]
    inner_file = descriptor_pb2.FileDescriptorProto.FromString(inner["schema_bytes"])
    assert inner_file.message_type[0].name == "Inner"


def test_serializer_is_stored_on_the_value_type():
    assert Point.WPIProto is PointProto
    assert wpiproto.get_serializer(Point) is PointProto


def test_subclasses_inherit_the_serializer():
    class LabeledPoint(Point):
        pass

    assert wpiproto.get_serializer(LabeledPoint) is PointProto


def test_registering_a_type_that_rejects_attributes_fails():
    with pytest.raises(TypeError, match="can't register"):
        wpiproto.register_serializer(int, PointProto)


def test_struct_is_preferred_over_protobuf(backend):
    assert StructWithSerializer.WPIProto is StructWithSerializerProto

    telemetry.log("both", StructWithSerializer(2.0))

    assert backend.get_last_action("/both") == mock_backend.Action(
        "/both",
        mock_backend.LogRawValue(
            wpistruct.pack(StructWithSerializer(2.0)), "struct:ProtoTestStruct"
        ),
    )
    assert backend.get_schema("proto:test/outer.proto") is None


def test_unregistered_object_still_logs_as_string(backend):
    class Unknown:
        def __str__(self) -> str:
            return "unknown"

    # wpiutil.protobuf is loaded here, but Unknown has no serializer.
    telemetry.log("unknown", Unknown())

    assert backend.get_last_action("/unknown") == mock_backend.Action(
        "/unknown", mock_backend.LogStringValue("unknown", "string")
    )


def test_protobuf_value_rejects_explicit_type_string(backend):
    with pytest.raises(TypeError):
        telemetry.log("outer", _outer(), type_string="custom")
