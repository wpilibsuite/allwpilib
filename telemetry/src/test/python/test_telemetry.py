import dataclasses

import pytest

import telemetry
from wpiutil import wpistruct


@wpistruct.make_wpistruct(name="TelemetryPoint")
@dataclasses.dataclass
class TelemetryPoint:
    x: wpistruct.double
    y: wpistruct.int32


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


class RecordingTelemetryEntry(telemetry.TelemetryEntry):
    def __init__(self) -> None:
        super().__init__()
        self.actions = []

    def keep_duplicates(self) -> None:
        self.actions.append(("keep_duplicates",))

    def set_property(self, key: str, value: str) -> None:
        self.actions.append(("property", key, value))

    def log_boolean(self, value: bool, timestamp: int) -> None:
        self.actions.append(("boolean", value))

    def log_int64(self, value: int, timestamp: int) -> None:
        self.actions.append(("integer", value))

    def log_float(self, value: float, timestamp: int) -> None:
        self.actions.append(("float", value))

    def log_double(self, value: float, timestamp: int) -> None:
        self.actions.append(("double", value))

    def log_string(self, value: str, type_string: str, timestamp: int) -> None:
        self.actions.append(("string", value, type_string))

    def log_boolean_array(self, value, timestamp: int) -> None:
        self.actions.append(("boolean[]", list(value)))

    def log_int16_array(self, value, timestamp: int) -> None:
        self.actions.append(("integer[]", list(value)))

    def log_int32_array(self, value, timestamp: int) -> None:
        self.actions.append(("integer[]", list(value)))

    def log_int64_array(self, value, timestamp: int) -> None:
        self.actions.append(("integer[]", list(value)))

    def log_float_array(self, value, timestamp: int) -> None:
        self.actions.append(("float[]", list(value)))

    def log_double_array(self, value, timestamp: int) -> None:
        self.actions.append(("double[]", list(value)))

    def log_string_array(self, value, timestamp: int) -> None:
        self.actions.append(("string[]", list(value)))

    def log_raw(self, value, type_string: str, timestamp: int) -> None:
        self.actions.append(("raw", bytes(value), type_string))


class RecordingTelemetryBackend(telemetry.TelemetryBackend):
    def __init__(self) -> None:
        super().__init__()
        self.entries = {}
        self.schemas = {}
        self.removed = []

    def get_entry(self, path: str) -> RecordingTelemetryEntry:
        return self.entries.setdefault(path, RecordingTelemetryEntry())

    def remove_entry(self, path: str) -> None:
        self.removed.append(path)

    def has_schema(self, schema_name: str) -> bool:
        return schema_name in self.schemas

    def add_schema(self, schema_name: str, type_string: str, schema) -> None:
        self.schemas[schema_name] = (type_string, schema)


def test_python_backend_subclass_receives_logs():
    telemetry.TelemetryRegistry.reset()
    backend = RecordingTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("", backend)

    try:
        telemetry.keep_duplicates("value")
        telemetry.set_property("value", "unit", '"m/s"')
        telemetry.log("value", 2.5)
        telemetry.log("point", TelemetryPoint(1.0, 2))
    finally:
        telemetry.TelemetryRegistry.reset()

    assert backend.entries["/value"].actions == [
        ("keep_duplicates",),
        ("property", "unit", '"m/s"'),
        ("double", 2.5),
    ]
    assert backend.schemas["struct:TelemetryPoint"] == (
        "structschema",
        "double x; int32 y",
    )
    assert backend.entries["/point"].actions == [
        ("raw", wpistruct.pack(TelemetryPoint(1.0, 2)), "struct:TelemetryPoint")
    ]


def test_telemetry_logs_python_object(backend):
    class Value(telemetry.TelemetryLoggable):
        def log_to(self, table: telemetry.TelemetryTable) -> None:
            table.log("value", 3)

        def get_telemetry_type(self) -> str:
            return "TestValue"

    telemetry.log("PythonValue", Value())

    assert backend.get_last_action("/PythonValue/.type") == {
        "path": "/PythonValue/.type",
        "timestamp": 0,
        "kind": "string",
        "value": "TestValue",
        "type_string": "string",
    }
    assert backend.get_last_value("/PythonValue/value") == 3


def test_python_log_to_type_error_is_not_retried(backend):
    class Value(telemetry.TelemetryLoggable):
        def __init__(self) -> None:
            self.calls = 0

        def log_to(self, table: telemetry.TelemetryTable) -> None:
            self.calls += 1
            table.log("beforeError", self.calls)
            raise TypeError("user log_to failure")

    value = Value()

    with pytest.raises(TypeError, match="user log_to failure"):
        telemetry.log("PythonValue", value)

    assert value.calls == 1
    actions = [
        action
        for action in backend.get_actions()
        if action["path"] == "/PythonValue/beforeError"
    ]
    assert len(actions) == 1
    assert actions[0]["value"] == 1


def test_python_log_to_descriptor_is_read_once(backend):
    class Value:
        def __init__(self) -> None:
            self.log_to_attr_calls = 0

        @property
        def log_to(self):
            self.log_to_attr_calls += 1

            def log(table: telemetry.TelemetryTable) -> None:
                table.log("value", 3)

            return log

    value = Value()

    telemetry.log("PythonValue", value)

    assert value.log_to_attr_calls == 1
    assert backend.get_last_value("/PythonValue/value") == 3


def test_python_log_to_lookup_error_is_reported(backend):
    class Value:
        @property
        def log_to(self):
            raise RuntimeError("log_to lookup failure")

        def __str__(self) -> str:
            raise AssertionError("__str__ should not run")

    with pytest.raises(RuntimeError, match="log_to lookup failure"):
        telemetry.log("PythonValue", Value())

    assert backend.get_actions() == []


def test_python_get_telemetry_type_descriptor_is_read_once(backend):
    class Value:
        def __init__(self) -> None:
            self.type_attr_calls = 0

        @property
        def get_telemetry_type(self):
            self.type_attr_calls += 1
            return lambda: "TestValue"

        def log_to(self, table: telemetry.TelemetryTable) -> None:
            table.log("value", 3)

    value = Value()

    telemetry.log("PythonValue", value)

    assert value.type_attr_calls == 1
    assert backend.get_last_value("/PythonValue/.type") == "TestValue"
    assert backend.get_last_value("/PythonValue/value") == 3


def test_python_get_telemetry_type_lookup_error_is_reported(backend):
    class Value:
        @property
        def get_telemetry_type(self):
            raise RuntimeError("get_telemetry_type lookup failure")

        def log_to(self, table: telemetry.TelemetryTable) -> None:
            table.log("value", 3)

    with pytest.raises(RuntimeError, match="get_telemetry_type lookup failure"):
        telemetry.log("PythonValue", Value())

    assert backend.get_actions() == []


def test_log_docstrings_document_sequence_element_type():
    for doc in (telemetry.log.__doc__, telemetry.TelemetryTable.log.__doc__):
        assert "Sequences must pass an explicit element_type" in doc
        assert "WPIStruct class" in doc


def test_telemetry_logs_python_types(backend):
    telemetry.log("flag", True)
    telemetry.log("count", 3)
    telemetry.log("ratio", 0.25)
    telemetry.log("label", "ready")
    telemetry.log("raw", bytearray(b"abc"), type_string="custom")
    telemetry.log("ints", [1, 2, 3], element_type=int)
    telemetry.log("mixed", [1, 2.5], element_type=float)
    telemetry.log("strings", ["a", "b"], element_type=str)
    telemetry.log("fallback", [{"x": 1}], element_type=object)

    assert backend.get_last_action("/flag")["kind"] == "boolean"
    assert backend.get_last_value("/count") == 3
    assert backend.get_last_value("/ratio") == pytest.approx(0.25)
    assert backend.get_last_value("/label") == "ready"
    assert backend.get_last_action("/raw")["type_string"] == "custom"
    assert backend.get_last_value("/raw") == b"abc"
    assert backend.get_last_action("/ints")["kind"] == "integer[]"
    assert backend.get_last_value("/ints") == [1, 2, 3]
    assert backend.get_last_action("/mixed")["kind"] == "double[]"
    assert backend.get_last_value("/mixed") == [1.0, 2.5]
    assert backend.get_last_action("/strings")["kind"] == "string[]"
    assert backend.get_last_value("/strings") == ["a", "b"]
    assert backend.get_last_action("/fallback")["kind"] == "string[]"
    assert backend.get_last_value("/fallback") == ["{'x': 1}"]


def test_sequence_without_element_type_raises(backend):
    with pytest.raises(TypeError, match="element type must be specified"):
        telemetry.log("empty", [])

    with pytest.raises(TypeError, match="element type must be specified"):
        telemetry.log("values", [1.25])

    with pytest.raises(TypeError, match="element type must be specified"):
        telemetry.log("points", [TelemetryPoint(1.0, 2)])

    assert backend.get_actions() == []


def test_sequence_with_string_element_type_raises(backend):
    with pytest.raises(TypeError, match="element_type must be a Python type"):
        telemetry.log("values", [1.25], element_type="double[]")

    assert backend.get_actions() == []


def test_sequence_element_type_mismatch_raises(backend):
    with pytest.raises(TypeError, match="string telemetry arrays require str values"):
        telemetry.log("values", [object()], element_type=str)

    assert backend.get_actions() == []


def test_empty_sequence_with_element_type_logs_typed_empty(backend):
    telemetry.log("values", [], element_type=float)

    assert backend.get_last_action("/values")["kind"] == "double[]"
    assert backend.get_last_value("/values") == []


def test_table_helpers(backend):
    table = telemetry.get_table("drive")

    assert table.path == "/drive/"
    table.keep_duplicates("speed")
    table.set_property("speed", "unit", '"m/s"')
    table.log("speed", 4.5)

    assert backend.get_last_action("/drive/speed")["value"] == pytest.approx(4.5)
    actions = backend.get_actions()
    assert any(action["kind"] == "keep_duplicates" for action in actions)
    assert any(action["kind"] == "set_property" for action in actions)


def test_multi_backend_fans_out():
    telemetry.TelemetryRegistry.reset()
    first = telemetry.MockTelemetryBackend()
    second = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend(
        "", telemetry.MultiTelemetryBackend([first, second])
    )
    try:
        telemetry.log("value", 7)
        telemetry.log("label", "ready")

        assert first.get_last_value("/value") == 7
        assert second.get_last_value("/value") == 7
        assert first.get_last_value("/label") == "ready"
        assert second.get_last_value("/label") == "ready"
    finally:
        telemetry.TelemetryRegistry.reset()


def test_report_warning_allows_reentry(backend):
    warnings = []

    def report_warning(path: str, msg: str) -> None:
        warnings.append((path, msg))
        if path == "/outer":
            telemetry.TelemetryRegistry.report_warning("/nested", "nested warning")

    telemetry.TelemetryRegistry.set_report_warning(report_warning)

    telemetry.TelemetryRegistry.report_warning("/outer", "outer warning")

    assert warnings == [
        ("/outer", "outer warning"),
        ("/nested", "nested warning"),
    ]


def test_table_type_mismatch_reports_warning(backend):
    warnings = []
    telemetry.TelemetryRegistry.set_report_warning(
        lambda path, msg: warnings.append((path, msg))
    )

    table = telemetry.get_table("typed")

    assert table.set_type("TestType") is True
    assert table.set_type("TestType") is True
    assert table.set_type("OtherType") is False
    assert table.get_type() == "TestType"
    assert table.has_type() is True
    assert warnings
    assert warnings[0][0] == "/typed/"
    assert "table type mismatch" in warnings[0][1]


def test_backend_prefix_selection_and_cache_reset(backend):
    table = telemetry.get_table("drive")
    table.log("speed", 1.0)
    assert backend.get_last_value("/drive/speed") == pytest.approx(1.0)

    drive_backend = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("/drive", drive_backend)

    table.log("speed", 2.0)

    assert backend.get_last_value("/drive/speed") is None
    assert drive_backend.get_last_value("/drive/speed") == pytest.approx(2.0)


def test_get_backend_normalizes_path(backend):
    drive_backend = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("/drive", drive_backend)

    assert telemetry.TelemetryRegistry.get_backend("drive/speed") is drive_backend
    assert telemetry.TelemetryRegistry.get_backend("//drive//speed") is drive_backend

    telemetry.TelemetryRegistry.reset()
    telemetry.TelemetryRegistry.register_backend("/drive", drive_backend)

    assert telemetry.TelemetryRegistry.get_backend("drive/speed") is drive_backend


def test_struct_logging_registers_schema_and_raw_bytes(backend):
    point = TelemetryPoint(1.5, 2)

    telemetry.log("point", point)

    assert backend.get_schema("struct:TelemetryPoint") == {
        "type": "structschema",
        "schema_bytes": b"",
        "schema_string": "double x; int32 y",
    }
    assert backend.get_last_action("/point") == {
        "path": "/point",
        "timestamp": 0,
        "kind": "raw",
        "value": wpistruct.pack(point),
        "type_string": "struct:TelemetryPoint",
    }


def test_struct_array_logging_registers_schema_and_raw_bytes(backend):
    points = [TelemetryPoint(1.0, 2), TelemetryPoint(3.0, 4)]

    telemetry.log("points", points, element_type=TelemetryPoint)

    assert backend.get_schema("struct:TelemetryPoint")["schema_string"] == (
        "double x; int32 y"
    )
    assert backend.get_last_action("/points") == {
        "path": "/points",
        "timestamp": 0,
        "kind": "raw",
        "value": wpistruct.pack_array(points),
        "type_string": "struct:TelemetryPoint[]",
    }


def test_empty_struct_array_with_element_type_logs_schema_and_empty_raw_bytes(
    backend,
):
    telemetry.log("points", [], element_type=TelemetryPoint)

    assert backend.get_schema("struct:TelemetryPoint")["schema_string"] == (
        "double x; int32 y"
    )
    assert backend.get_last_action("/points") == {
        "path": "/points",
        "timestamp": 0,
        "kind": "raw",
        "value": b"",
        "type_string": "struct:TelemetryPoint[]",
    }


def test_struct_array_element_type_mismatch_raises(backend):
    with pytest.raises(TypeError, match="specified WPIStruct type"):
        telemetry.log("points", [object()], element_type=TelemetryPoint)

    assert backend.get_actions() == []


def test_struct_schema_uses_matching_backend(backend):
    drive_backend = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("/drive", drive_backend)

    telemetry.get_table("drive").log("point", TelemetryPoint(1.0, 2))

    assert backend.get_schema("struct:TelemetryPoint") is None
    assert drive_backend.get_schema("struct:TelemetryPoint") is not None
    assert backend.get_last_action("/drive/point") is None
    assert drive_backend.get_last_value("/drive/point") == wpistruct.pack(
        TelemetryPoint(1.0, 2)
    )


def test_multi_backend_fans_out_struct_schema_and_loggable_metadata():
    telemetry.TelemetryRegistry.reset()
    first = telemetry.MockTelemetryBackend()
    second = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend(
        "", telemetry.MultiTelemetryBackend([first, second])
    )

    class Value(telemetry.TelemetryLoggable):
        def log_to(self, table: telemetry.TelemetryTable) -> None:
            table.log("point", TelemetryPoint(1.0, 2))

        def get_telemetry_type(self) -> str:
            return "TypedValue"

    try:
        telemetry.log("value", Value())

        assert first.get_last_value("/value/.type") == "TypedValue"
        assert second.get_last_value("/value/.type") == "TypedValue"
        assert first.get_schema("struct:TelemetryPoint") is not None
        assert second.get_schema("struct:TelemetryPoint") is not None
        assert first.get_last_value("/value/point") == wpistruct.pack(
            TelemetryPoint(1.0, 2)
        )
        assert second.get_last_value("/value/point") == wpistruct.pack(
            TelemetryPoint(1.0, 2)
        )
    finally:
        telemetry.TelemetryRegistry.reset()


def test_discard_parent_expands_python_object_for_non_discard_descendant():
    telemetry.TelemetryRegistry.reset()
    telemetry.TelemetryRegistry.register_backend(
        "", telemetry.DiscardTelemetryBackend()
    )
    speed_backend = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("/robot/speed", speed_backend)

    class RobotSpeed(telemetry.TelemetryLoggable):
        def log_to(self, table: telemetry.TelemetryTable) -> None:
            table.log("speed", 4.5)

    try:
        telemetry.log("robot", RobotSpeed())

        assert speed_backend.get_last_value("/robot/speed") == pytest.approx(4.5)
    finally:
        telemetry.TelemetryRegistry.reset()


def test_discard_backend_skips_telemetry_work_and_cache_resets(backend):
    class ThrowingLoggable(telemetry.TelemetryLoggable):
        def log_to(self, table: telemetry.TelemetryTable) -> None:
            raise AssertionError("log_to should not run")

    class ThrowingStr:
        def __str__(self) -> str:
            raise AssertionError("__str__ should not run")

    telemetry.TelemetryRegistry.register_backend(
        "/discard", telemetry.DiscardTelemetryBackend()
    )
    table = telemetry.get_table("discard")

    table.log("value", ThrowingLoggable())
    table.log("text", ThrowingStr())
    table.log("points", [TelemetryPoint(1.0, 2)], element_type=TelemetryPoint)

    assert backend.get_actions() == []

    active = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("/discard", active)

    table.log("value", 5)

    assert active.get_last_value("/discard/value") == 5
