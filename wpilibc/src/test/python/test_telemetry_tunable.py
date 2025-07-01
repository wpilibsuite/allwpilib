import wpilib


def test_telemetry_python_object(nt):
    class Value(wpilib.TelemetryLoggable):
        def log_to(self, table: wpilib.TelemetryTable) -> None:
            table.log("value", 3)

        def get_telemetry_type(self) -> str:
            return "TestValue"

    wpilib.Telemetry.log("PythonValue", Value())


def test_tunable_and_selectable(nt):
    value = wpilib.Tunable(1)
    assert value.get() == 1
    value.set(2)
    assert value.get() == 2

    selectable: wpilib.Selectable[int] = wpilib.Selectable()
    selectable.add_default("one", 1)
    selectable.add("two", 2)
    assert selectable.get_selected() == 1

    wpilib.Tunables.publish("PythonSelectable", selectable)
    wpilib.Tunables.remove("PythonSelectable")
