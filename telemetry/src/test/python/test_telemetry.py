import pytest

import telemetry


@pytest.fixture
def backend():
    telemetry.TelemetryRegistry.reset()
    backend = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("", backend)
    try:
        yield backend
    finally:
        telemetry.TelemetryRegistry.reset()


def test_telemetry_logs_python_object(backend):
    class Value(telemetry.TelemetryLoggable):
        def log_to(self, table: telemetry.TelemetryTable) -> None:
            table.log("value", 3)

        def get_telemetry_type(self) -> str:
            return "TestValue"

    telemetry.Telemetry.log("PythonValue", Value())

    assert backend.get_last_action("/PythonValue/.type") == {
        "path": "/PythonValue/.type",
        "kind": "string",
        "value": "TestValue",
        "type_string": "string",
    }
    assert backend.get_last_value("/PythonValue/value") == 3


def test_telemetry_log_infers_python_types(backend):
    telemetry.Telemetry.log("flag", True)
    telemetry.Telemetry.log("count", 3)
    telemetry.Telemetry.log("ratio", 0.25)
    telemetry.Telemetry.log("label", "ready")
    telemetry.Telemetry.log("raw", bytearray(b"abc"), "custom")
    telemetry.Telemetry.log("ints", [1, 2, 3])
    telemetry.Telemetry.log("mixed", [1, 2.5])
    telemetry.Telemetry.log("fallback", [{"x": 1}])

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
    assert backend.get_last_action("/fallback")["kind"] == "string[]"
    assert backend.get_last_value("/fallback") == ["{'x': 1}"]


def test_table_helpers(backend):
    table = telemetry.Telemetry.get_table("drive")

    assert table.path == "/drive/"
    table.keep_duplicates("speed")
    table.set_property("speed", "unit", '"m/s"')
    table.log("speed", 4.5)

    assert backend.get_last_action("/drive/speed")["value"] == pytest.approx(
        4.5
    )
    actions = backend.get_actions()
    assert any(action["kind"] == "keep_duplicates" for action in actions)
    assert any(action["kind"] == "set_property" for action in actions)
