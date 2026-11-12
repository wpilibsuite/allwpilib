import dataclasses
from pathlib import Path

import wpilog
from google.protobuf import descriptor_pb2, descriptor_pool, message_factory
from wpilog import protobuf as logproto
from wpiutil.protobuf import ProtobufDescriptor


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


class OuterProto(ProtobufDescriptor[tuple[str, int], Outer]):
    """Serializes a (name, x) tuple."""

    message_type = Outer

    @classmethod
    def pack(cls, msg, value):
        msg.name, msg.inner.x = value

    @classmethod
    def unpack(cls, msg):
        return (msg.name, msg.inner.x)


# Timestamps are in nanoseconds but the file only keeps whole microseconds.
US = 1000


@dataclasses.dataclass
class Start:
    name: str
    type: str
    metadata: str


def _read(path: Path):
    """
    Returns ({entry id: start record}, [(entry id, timestamp in ns, raw bytes)]).

    Everything is copied out of the reader: a StartRecordData only views the reader's buffer,
    so it is invalid once the reader is gone.
    """
    reader = wpilog.DataLogReader(str(path))
    assert reader.is_valid()
    starts = {}
    records = []
    for record in reader:
        if record.is_start():
            start = record.get_start_data()
            starts[start.entry] = Start(start.name, start.type, start.metadata)
        elif not record.is_control():
            records.append(
                (record.get_entry(), record.get_timestamp(), bytes(record.get_raw()))
            )
    return starts, records


def _entry_id(starts, name: str) -> int:
    (entry,) = [e for e, start in starts.items() if start.name == name]
    return entry


def test_exported_from_wpilog():
    for name in ("ProtobufLogEntry", "add_protobuf_schema"):
        assert getattr(wpilog, name) is getattr(logproto, name)
        assert name in wpilog.__all__


def test_append_message(tmp_path: Path):
    path = tmp_path / "message.wpilog"
    value = Outer(name="hello")
    value.inner.x = 5
    with wpilog.DataLogWriter(str(path)) as log:
        entry = logproto.ProtobufLogEntry(log, "outer", Outer, timestamp=5 * US)
        entry.append(value, 7 * US)

    starts, records = _read(path)
    outer = _entry_id(starts, "outer")
    assert starts[outer].type == "proto:test.Outer"
    assert [(t, Outer.FromString(raw)) for e, t, raw in records if e == outer] == [
        (7 * US, value)
    ]


def test_append_with_serializer(tmp_path: Path):
    path = tmp_path / "serializer.wpilog"
    with wpilog.DataLogWriter(str(path)) as log:
        entry = logproto.ProtobufLogEntry(log, "outer", OuterProto, timestamp=5 * US)
        entry.append(("hello", 5), 7 * US)

    starts, records = _read(path)
    outer = _entry_id(starts, "outer")
    (raw,) = [raw for e, _, raw in records if e == outer]
    assert OuterProto.deserialize(raw) == ("hello", 5)


def test_schemas_written_with_dependencies(tmp_path: Path):
    path = tmp_path / "schemas.wpilog"
    with wpilog.DataLogWriter(str(path)) as log:
        assert not log.has_schema("proto:test/outer.proto")
        logproto.ProtobufLogEntry(log, "outer", Outer, timestamp=5)
        assert log.has_schema("proto:test/outer.proto")
        assert log.has_schema("proto:test/inner.proto")

    starts, records = _read(path)
    schemas = {
        name: descriptor_pb2.FileDescriptorProto.FromString(raw)
        for name in ("proto:test/outer.proto", "proto:test/inner.proto")
        for e, _, raw in records
        if e == _entry_id(starts, f"/.schema/{name}")
    }
    assert list(schemas["proto:test/outer.proto"].dependency) == ["test/inner.proto"]
    assert schemas["proto:test/inner.proto"].message_type[0].name == "Inner"
    assert all(
        starts[_entry_id(starts, f"/.schema/{name}")].type
        == "proto:FileDescriptorProto"
        for name in schemas
    )


def test_second_entry_does_not_rewrite_schemas(tmp_path: Path):
    path = tmp_path / "shared-schemas.wpilog"
    with wpilog.DataLogWriter(str(path)) as log:
        logproto.ProtobufLogEntry(log, "a", Outer, timestamp=5)
        logproto.ProtobufLogEntry(log, "b", Outer, timestamp=5)

    starts, _ = _read(path)
    assert [s.name for s in starts.values()].count(
        "/.schema/proto:test/outer.proto"
    ) == 1


def test_update_skips_unchanged_values(tmp_path: Path):
    path = tmp_path / "update.wpilog"
    with wpilog.DataLogWriter(str(path)) as log:
        entry = logproto.ProtobufLogEntry(log, "outer", OuterProto, timestamp=5 * US)
        assert not entry.has_last_value()
        assert entry.get_last_value() is None

        entry.update(("a", 1), 7 * US)
        entry.update(("a", 1), 8 * US)
        entry.update(("b", 2), 9 * US)
        assert entry.has_last_value()
        assert entry.get_last_value() == ("b", 2)

    starts, records = _read(path)
    outer = _entry_id(starts, "outer")
    assert [
        (t, OuterProto.deserialize(raw)) for e, t, raw in records if e == outer
    ] == [(7 * US, ("a", 1)), (9 * US, ("b", 2))]


def test_metadata_and_finish(tmp_path: Path):
    path = tmp_path / "metadata.wpilog"
    with wpilog.DataLogWriter(str(path)) as log:
        entry = logproto.ProtobufLogEntry(
            log, "outer", Outer, metadata="initial", timestamp=5 * US
        )
        entry.set_metadata("changed", 6 * US)
        entry.finish(7 * US)

    starts, _ = _read(path)
    assert starts[_entry_id(starts, "outer")].metadata == "initial"
