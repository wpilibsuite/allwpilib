import array
import dataclasses

import pytest

import telemetry
import telemetry.mock_backend as mock_backend
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


def test_mock_backend_optional_results_are_none():
    backend = telemetry.MockTelemetryBackend()
    assert backend.get_last_action("/missing") is None
    assert backend.get_last_value("/missing") is None
    assert backend.get_schema("missing") is None
    assert backend.get_actions() == []


def test_mock_backend_returns_action_value_dataclasses(backend):
    telemetry.keep_duplicates("value")
    telemetry.set_property("value", "unit", '"m/s"')
    telemetry.log("label", "ready")
    telemetry.log("flags", [True, False], element_type=bool)
    telemetry.log("raw", b"abc", type_string="custom")
    telemetry.log("count", 3)
    telemetry.log("ints", [1, 2], element_type=int)

    assert backend.get_actions() == [
        mock_backend.Action("/value", mock_backend.KeepDuplicatesValue(True)),
        mock_backend.Action("/value", mock_backend.SetPropertyValue("unit", '"m/s"')),
        mock_backend.Action("/label", mock_backend.LogStringValue("ready", "string")),
        mock_backend.Action("/flags", mock_backend.LogBooleanArrayValue([True, False])),
        mock_backend.Action("/raw", mock_backend.LogRawValue(b"abc", "custom")),
        mock_backend.Action("/count", 3),
        mock_backend.Action("/ints", [1, 2]),
    ]


def test_mock_backend_action_dataclasses_are_copies(backend):
    telemetry.log("flags", [True, False], element_type=bool)

    action = backend.get_actions()[0]
    action.path = "/changed"
    action.value.value[0] = False

    assert backend.get_actions() == [
        mock_backend.Action("/flags", mock_backend.LogBooleanArrayValue([True, False]))
    ]


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

    assert backend.get_last_action("/PythonValue/.type") == mock_backend.Action(
        "/PythonValue/.type",
        mock_backend.LogStringValue("TestValue", "string"),
    )
    assert backend.get_last_value("/PythonValue/value") == 3


def test_native_telemetry_loggable_has_no_constructor():
    from telemetry import _telemetry

    with pytest.raises(TypeError, match="No constructor defined"):
        _telemetry.TelemetryLoggable()


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
        if action.path == "/PythonValue/beforeError"
    ]
    assert actions == [mock_backend.Action("/PythonValue/beforeError", 1)]


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


def test_telemetry_type_defaults_are_keyword_only(backend):
    with pytest.raises(TypeError):
        telemetry.log("value", [1], int)

    telemetry.log("value", [1], element_type=int)
    assert backend.get_last_value("/value") == [1]


def test_log_docstrings_document_sequence_element_type():
    for doc in (telemetry.log.__doc__, telemetry.TelemetryTable.log.__doc__):
        assert "Sequences must pass an explicit element_type" in doc
        assert "WPIStruct class" in doc
        assert "element_type: type[object] | None = None" in doc


@pytest.mark.parametrize("use_table", [False, True])
def test_scalar_with_none_element_type_logs(backend, use_table):
    log = telemetry.get_table().log if use_table else telemetry.log
    log("value", 3, element_type=None)

    assert backend.get_last_value("/value") == 3


@pytest.mark.parametrize("use_table", [False, True])
def test_sequence_with_none_element_type_raises(backend, use_table):
    log = telemetry.get_table().log if use_table else telemetry.log
    with pytest.raises(TypeError, match="element type must be specified"):
        log("values", [1], element_type=None)

    assert backend.get_actions() == []


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

    assert backend.get_last_action("/flag") == mock_backend.Action("/flag", True)
    assert backend.get_last_value("/count") == 3
    assert backend.get_last_value("/ratio") == pytest.approx(0.25)
    assert backend.get_last_value("/label") == "ready"
    assert backend.get_last_action("/raw") == mock_backend.Action(
        "/raw", mock_backend.LogRawValue(b"abc", "custom")
    )
    assert backend.get_last_value("/raw") == b"abc"
    assert backend.get_last_action("/ints") == mock_backend.Action("/ints", [1, 2, 3])
    assert backend.get_last_value("/ints") == [1, 2, 3]
    assert backend.get_last_action("/mixed") == mock_backend.Action(
        "/mixed", [1.0, 2.5]
    )
    assert backend.get_last_value("/mixed") == [1.0, 2.5]
    assert backend.get_last_action("/strings") == mock_backend.Action(
        "/strings", ["a", "b"]
    )
    assert backend.get_last_value("/strings") == ["a", "b"]
    assert backend.get_last_action("/fallback") == mock_backend.Action(
        "/fallback", ["{'x': 1}"]
    )
    assert backend.get_last_value("/fallback") == ["{'x': 1}"]


@pytest.mark.parametrize(
    "value, expected",
    [
        (b"a\x00\xff", b"a\x00\xff"),
        (bytearray(b"a\x00\xff"), b"a\x00\xff"),
        (memoryview(b"abcdef"), b"abcdef"),
        (memoryview(b"abcdef")[::2], b"ace"),
        (memoryview(b"abcdef")[::-1], b"fedcba"),
        (memoryview(b"abcdef").cast("B", shape=(2, 3)), b"abcdef"),
        (memoryview(array.array("H", [0x0101, 0xFEFE])), b"\x01\x01\xfe\xfe"),
        (b"", b""),
        (memoryview(array.array("B")), b""),
    ],
)
def test_log_buffer_as_raw(backend, value, expected):
    telemetry.log("raw", value, type_string="custom")

    assert backend.get_last_action("/raw") == mock_backend.Action(
        "/raw", mock_backend.LogRawValue(expected, "custom")
    )


@pytest.mark.parametrize("layout", ["c", "fortran", "strided", "scalar", "empty"])
def test_log_numpy_memoryview_as_raw(backend, layout):
    np = pytest.importorskip("numpy")
    value = np.array(
        [[1, 2], [3, 4]], dtype="<u2", order="F" if layout == "fortran" else "C"
    )
    expected = b"\x01\x00\x02\x00\x03\x00\x04\x00"
    if layout == "strided":
        value = value[::-1, ::-1]
        expected = b"\x04\x00\x03\x00\x02\x00\x01\x00"
    elif layout == "scalar":
        value = np.array(258, dtype="<u2")
        expected = b"\x02\x01"
    elif layout == "empty":
        value = value[:0]
        expected = b""

    telemetry.log("raw", memoryview(value))

    assert backend.get_last_action("/raw") == mock_backend.Action(
        "/raw", mock_backend.LogRawValue(expected, "raw")
    )


def test_log_indirect_memoryview_as_raw(backend):
    tb = pytest.importorskip("_testbuffer")
    value = memoryview(tb.ndarray(list(b"abcdef"), shape=[2, 3], flags=tb.ND_PIL))

    telemetry.log("raw", value)
    assert backend.get_last_value("/raw") == b"abcdef"


def test_numpy_array_keeps_sequence_dispatch(backend):
    np = pytest.importorskip("numpy")
    value = np.array([1.5, 2.5], dtype=np.float64)

    with pytest.raises(TypeError, match="element type must be specified"):
        telemetry.log("values", value)
    telemetry.log("values", value, element_type=float)

    assert backend.get_last_value("/values") == [1.5, 2.5]


def test_array_exporter_keeps_sequence_dispatch(backend):
    value = array.array("H", [1, 255])
    with pytest.raises(TypeError, match="element type must be specified"):
        telemetry.log("values", value)
    telemetry.log("values", value, element_type=int)
    assert backend.get_last_value("/values") == [1, 255]


@pytest.mark.parametrize("raise_in_backend", [False, True])
def test_log_buffer_export_lives_through_backend_call(backend, raise_in_backend):
    source = bytearray(b"abc")

    class Entry(RecordingTelemetryEntry):
        def log_raw(self, value, type_string, timestamp):
            with pytest.raises(BufferError):
                source.extend(b"d")
            super().log_raw(value, type_string, timestamp)
            if raise_in_backend:
                raise RuntimeError("backend failure")

    class Backend(RecordingTelemetryBackend):
        def get_entry(self, path):
            return self.entries.setdefault(path, Entry())

    recording = Backend()
    telemetry.TelemetryRegistry.register_backend("/pinned", recording)
    if raise_in_backend:
        with pytest.raises(RuntimeError, match="backend failure"):
            telemetry.log("pinned/raw", source)
    else:
        telemetry.log("pinned/raw", source)
    source.extend(b"d")

    assert recording.entries["/pinned/raw"].actions == [("raw", b"abc", "raw")]


def test_buffer_rejects_sequence_element_type(backend):
    with pytest.raises(TypeError, match="element_type.*sequences"):
        telemetry.log("raw", memoryview(array.array("B", [1, 2])), element_type=int)


def test_log_released_buffer_errors_unless_discarded(backend):
    value = memoryview(b"abc")
    value.release()
    with pytest.raises(ValueError):
        telemetry.log("raw", value)

    telemetry.TelemetryRegistry.register_backend(
        "/discard", telemetry.DiscardTelemetryBackend()
    )
    telemetry.log("discard/raw", value)
    assert backend.get_actions() == []


def test_sequence_without_element_type_raises(backend):
    with pytest.raises(TypeError, match="element type must be specified"):
        telemetry.log("empty", [])

    with pytest.raises(TypeError, match="element type must be specified"):
        telemetry.log("values", [1.25])

    with pytest.raises(TypeError, match="element type must be specified"):
        telemetry.log("points", [TelemetryPoint(1.0, 2)])

    assert backend.get_actions() == []


@pytest.mark.parametrize("use_table", [False, True])
def test_sequence_with_string_element_type_raises(backend, use_table):
    log = telemetry.get_table().log if use_table else telemetry.log
    with pytest.raises(TypeError, match="element_type must be a Python type"):
        log("values", [1.25], element_type="double[]")

    assert backend.get_actions() == []


def test_sequence_element_type_mismatch_raises(backend):
    with pytest.raises(TypeError, match="string telemetry arrays require str values"):
        telemetry.log("values", [object()], element_type=str)

    assert backend.get_actions() == []


def test_empty_sequence_with_element_type_logs_typed_empty(backend):
    telemetry.log("values", [], element_type=float)

    assert backend.get_last_action("/values") == mock_backend.Action("/values", [])
    assert backend.get_last_value("/values") == []


def test_table_helpers(backend):
    table = telemetry.get_table("drive")

    assert table.get_path() == "/drive/"
    table.keep_duplicates("speed")
    table.set_property("speed", "unit", '"m/s"')
    table.log("speed", 4.5)

    assert backend.get_last_action("/drive/speed").value == pytest.approx(4.5)
    actions = backend.get_actions()
    assert any(
        isinstance(action.value, mock_backend.KeepDuplicatesValue) for action in actions
    )
    assert any(
        isinstance(action.value, mock_backend.SetPropertyValue) for action in actions
    )


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


def test_multi_backend_get_entry_is_exposed_and_fans_out():
    first = telemetry.MockTelemetryBackend()
    second = telemetry.MockTelemetryBackend()
    backend = telemetry.MultiTelemetryBackend([first, second])

    assert "get_entry" in telemetry.MultiTelemetryBackend.__dict__
    backend.get_entry("/value").log_int64(7, 42)

    assert first.get_last_action("/value") == mock_backend.Action("/value", 7, 42)
    assert second.get_last_action("/value") == mock_backend.Action("/value", 7, 42)
    assert first.get_last_value("/value") == 7
    assert second.get_last_value("/value") == 7


def test_multi_backend_remove_entry_is_exposed_and_fans_out():
    first = RecordingTelemetryBackend()
    second = RecordingTelemetryBackend()
    backend = telemetry.MultiTelemetryBackend([first, second])

    assert "remove_entry" in telemetry.MultiTelemetryBackend.__dict__
    backend.remove_entry("/value")

    assert first.removed == ["/value"]
    assert second.removed == ["/value"]


def test_multi_backend_has_schema_is_exposed_and_checks_every_child():
    first = telemetry.MockTelemetryBackend()
    second = telemetry.MockTelemetryBackend()
    backend = telemetry.MultiTelemetryBackend([first, second])
    first.add_schema("example", "test", "first")

    assert "has_schema" in telemetry.MultiTelemetryBackend.__dict__
    assert not backend.has_schema("example")

    second.add_schema("example", "test", "second")

    assert backend.has_schema("example")


def test_multi_backend_add_schema_raw_overload_is_exposed_and_fans_out():
    first = telemetry.MockTelemetryBackend()
    second = telemetry.MockTelemetryBackend()
    backend = telemetry.MultiTelemetryBackend([first, second])

    assert "add_schema" in telemetry.MultiTelemetryBackend.__dict__
    backend.add_schema("raw", "test", b"\x00\x01\xff")

    expected = {
        "type": "test",
        "schema_bytes": b"\x00\x01\xff",
        "schema_string": "",
    }
    assert first.get_schema("raw") == expected
    assert second.get_schema("raw") == expected


def test_multi_backend_add_schema_string_overload_is_exposed_and_fans_out():
    first = telemetry.MockTelemetryBackend()
    second = telemetry.MockTelemetryBackend()
    backend = telemetry.MultiTelemetryBackend([first, second])

    assert "add_schema" in telemetry.MultiTelemetryBackend.__dict__
    backend.add_schema("text", "test", "schema text")

    expected = {
        "type": "test",
        "schema_bytes": b"",
        "schema_string": "schema text",
    }
    assert first.get_schema("text") == expected
    assert second.get_schema("text") == expected


def test_registry_has_schema_uses_backend_schema_state():
    backend = telemetry.MockTelemetryBackend()

    assert not telemetry.TelemetryRegistry.has_schema(backend, "example")

    backend.add_schema("example", "test", "schema")

    assert telemetry.TelemetryRegistry.has_schema(backend, "example")


def test_registry_add_schema_raw_overload_updates_backend_schema_state():
    backend = telemetry.MockTelemetryBackend()

    telemetry.TelemetryRegistry.add_schema(backend, "raw", "test", b"\x00\x01\xff")

    assert backend.get_schema("raw") == {
        "type": "test",
        "schema_bytes": b"\x00\x01\xff",
        "schema_string": "",
    }


def test_registry_add_schema_string_overload_updates_backend_schema_state():
    backend = telemetry.MockTelemetryBackend()

    telemetry.TelemetryRegistry.add_schema(backend, "text", "test", "schema text")

    assert backend.get_schema("text") == {
        "type": "test",
        "schema_bytes": b"",
        "schema_string": "schema text",
    }


def test_registry_add_struct_schema_adds_existing_wpistruct_class():
    backend = telemetry.MockTelemetryBackend()

    telemetry.TelemetryRegistry.add_struct_schema(backend, TelemetryPoint)

    assert backend.get_schema("struct:TelemetryPoint") == {
        "type": "structschema",
        "schema_bytes": b"",
        "schema_string": "double x; int32 y",
    }


def test_registry_add_struct_schema_adds_new_wpistruct_class():
    @wpistruct.make_wpistruct(name="TelemetryVelocity")
    @dataclasses.dataclass
    class TelemetryVelocity:
        speed: wpistruct.double

    backend = telemetry.MockTelemetryBackend()

    telemetry.TelemetryRegistry.add_struct_schema(backend, TelemetryVelocity)

    assert backend.get_schema("struct:TelemetryVelocity") == {
        "type": "structschema",
        "schema_bytes": b"",
        "schema_string": "double speed",
    }


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
    assert backend.get_last_action("/point") == mock_backend.Action(
        "/point",
        mock_backend.LogRawValue(wpistruct.pack(point), "struct:TelemetryPoint"),
    )


def test_struct_array_logging_registers_schema_and_raw_bytes(backend):
    points = [TelemetryPoint(1.0, 2), TelemetryPoint(3.0, 4)]

    telemetry.log("points", points, element_type=TelemetryPoint)

    assert backend.get_schema("struct:TelemetryPoint")["schema_string"] == (
        "double x; int32 y"
    )
    assert backend.get_last_action("/points") == mock_backend.Action(
        "/points",
        mock_backend.LogRawValue(
            wpistruct.pack_array(points), "struct:TelemetryPoint[]"
        ),
    )


def test_empty_struct_array_with_element_type_logs_schema_and_empty_raw_bytes(
    backend,
):
    telemetry.log("points", [], element_type=TelemetryPoint)

    assert backend.get_schema("struct:TelemetryPoint")["schema_string"] == (
        "double x; int32 y"
    )
    assert backend.get_last_action("/points") == mock_backend.Action(
        "/points",
        mock_backend.LogRawValue(b"", "struct:TelemetryPoint[]"),
    )


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
