import re

import pytest
import telemetry
import tunables
import wpilib
import wpilib.simulation


@pytest.fixture
def mock_tunable_backend():
    tunables.TunableRegistry.reset()
    backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("", backend)
    try:
        yield backend
    finally:
        tunables.TunableRegistry.reset()


def _register_networktables_telemetry_backend(nt) -> None:
    telemetry.TelemetryRegistry.register_backend(
        "", wpilib.NetworkTablesTelemetryBackend(nt, "/Telemetry")
    )


def _register_networktables_tunable_backend(nt) -> None:
    tunables.TunableRegistry.register_backend(
        "", wpilib.NetworkTablesTunableBackend(nt, "/Tunables")
    )


def test_networktables_backend_registry_publication_uses_native_implementation(nt):
    class Backend(wpilib.NetworkTablesTunableBackend):
        def publish(self, *args):
            return False

    tunables.TunableRegistry.reset()
    backend = Backend(nt, "/Tunables")
    subscriber = nt.get_topic("/Tunables/value").generic_subscribe("int")
    try:
        tunables.TunableRegistry.register_backend("", backend)
        value = tunables.Tunable(1)

        assert tunables.publish("value", value) is True
        assert subscriber.get_integer(0) == 1

        value.set(2)
        tunables.TunableRegistry.update()
        assert subscriber.get_integer(0) == 2
    finally:
        tunables.TunableRegistry.reset()


class DashboardSelectable:
    def __init__(self, nt, path: str) -> None:
        self._default = nt.get_topic(f"{path}/default").generic_subscribe("string")
        self._selected = nt.get_topic(f"{path}/selected/tune").generic_subscribe(
            "string"
        )
        self._selected_publisher = nt.get_topic(
            f"{path}/selected/tune"
        ).generic_publish_ex("string", {"retained": True})
        self._active = nt.get_topic(f"{path}/selected/value").generic_subscribe(
            "string"
        )
        self._options = nt.get_topic(f"{path}/options").generic_subscribe("string[]")

    def close(self) -> None:
        for resource in (
            self._default,
            self._selected,
            self._selected_publisher,
            self._active,
            self._options,
        ):
            close = getattr(resource, "close", None)
            if close is not None:
                close()

    def exists(self) -> bool:
        return self._options.get_topic().exists()

    def get_default(self) -> str:
        return self._default.get_string("")

    def get_selected(self) -> str:
        return self._selected.get_string("")

    def get_active(self) -> str:
        active = self._active.get_string("")
        return active or self.get_default()

    def get_options(self) -> list[str]:
        return self._options.get_string_array([])

    def set_selected(self, value: str) -> None:
        self._selected_publisher.set_string(value)


def make_selectable() -> tunables.Selectable[int]:
    chooser = tunables.Selectable()
    chooser.add_default("one", 1)
    chooser.add("two", 2)
    return chooser


def test_telemetry_tunable_flat_namespace():
    for name in (
        "ComplexTunable",
        "DiscardTelemetryBackend",
        "MockTelemetryBackend",
        "MockTunableBackend",
        "MultiTelemetryBackend",
        "Selectable",
        "Telemetry",
        "TelemetryBackend",
        "TelemetryEntry",
        "TelemetryLoggable",
        "TelemetryRegistry",
        "TelemetryTable",
        "Tunable",
        "TunableBackend",
        "TunableRegistry",
        "TunableTable",
        "Tunables",
    ):
        assert not hasattr(wpilib, name)


def test_field2d_python_publish_override_receives_native_table(
    mock_tunable_backend,
):
    received: list[tunables.TunableTable] = []

    class CustomField(wpilib.Field2d):
        def publish_tunable(self, table: tunables.TunableTable) -> None:
            received.append(table)
            table.add_int("custom", 3)

    tunables.publish("field", CustomField())

    assert len(received) == 1
    assert type(received[0]) is tunables.TunableTable
    assert mock_tunable_backend.get_value("/field/custom") == 3


def test_field2d_python_subclass_uses_native_tunable_fallback(
    mock_tunable_backend,
):
    class DerivedField2d(wpilib.Field2d):
        pass

    field = DerivedField2d()

    assert tunables.publish("derivedField", field) is True
    assert mock_tunable_backend.get_value("/derivedField/Robot") == bytes(24)


def test_register_networktables_telemetry_backend(nt):
    telemetry.TelemetryRegistry.reset()
    _register_networktables_telemetry_backend(nt)

    telemetry.log("helperTelemetry", 2.5)

    assert nt.get_entry("/Telemetry/helperTelemetry").get_double(0.0) == pytest.approx(
        2.5
    )


def test_register_networktables_tunable_backend(nt):
    tunables.TunableRegistry.reset()
    _register_networktables_tunable_backend(nt)

    value = tunables.Tunable(1.0)
    tunables.publish("helperTunable", value)

    entry = nt.get_entry("/Tunables/helperTunable")
    assert entry.get_double(0.0) == pytest.approx(1.0)

    entry.set_double(3.5)
    nt.flush()
    tunables.TunableRegistry.update()

    assert value.get() == pytest.approx(3.5)


def test_networktables_tunable_getter_setter_echoes_canonical_value(nt):
    value = [1.0]

    tunables.get_table().publish_double(
        "clamped",
        lambda: value[0],
        lambda requested: value.__setitem__(0, min(requested, 5.0)),
        robust=True,
    )

    value_entry = nt.get_entry("/Tunables/clamped/value")
    tune_entry = nt.get_entry("/Tunables/clamped/tune")
    assert value_entry.get_double(0.0) == pytest.approx(1.0)

    tune_entry.set_double(10.0)
    nt.flush()
    tunables.TunableRegistry.update()
    nt.flush()

    assert value[0] == pytest.approx(5.0)
    assert value_entry.get_double(0.0) == pytest.approx(5.0)


def test_selectable():
    chooser = tunables.Selectable()
    assert chooser.get_selected() is None

    chooser.add_default("option", True)
    assert chooser.get_selected() is True


def test_selectable_dashboard_connects_after_publish(nt):
    chooser = make_selectable()
    tunables.publish("auto", chooser)
    dashboard = DashboardSelectable(nt, "/Tunables/auto")
    try:
        assert dashboard.exists()
        assert dashboard.get_default() == "one"
        assert dashboard.get_options() == ["one", "two"]
        assert dashboard.get_selected() == ""
        assert dashboard.get_active() == "one"
        assert chooser.get_selected() == 1

        dashboard.set_selected("two")
        nt.flush()
        tunables.TunableRegistry.update()

        assert dashboard.get_selected() == "two"
        assert dashboard.get_active() == "two"
        assert chooser.get_selected() == 2
    finally:
        dashboard.close()


def test_selectable_dashboard_connects_before_publish(nt):
    dashboard = DashboardSelectable(nt, "/Tunables/auto")
    try:
        chooser = make_selectable()
        tunables.publish("auto", chooser)

        assert dashboard.exists()
        assert dashboard.get_default() == "one"
        assert dashboard.get_options() == ["one", "two"]
        assert dashboard.get_active() == "one"
        assert chooser.get_selected() == 1

        dashboard.set_selected("two")
        nt.flush()
        tunables.TunableRegistry.update()

        assert dashboard.get_selected() == "two"
        assert dashboard.get_active() == "two"
        assert chooser.get_selected() == 2
    finally:
        dashboard.close()


def test_selectable_retains_dashboard_selection_after_republish(nt):
    dashboard = DashboardSelectable(nt, "/Tunables/auto")
    try:
        first_chooser = make_selectable()
        tunables.publish("auto", first_chooser)

        dashboard.set_selected("two")
        nt.flush()
        tunables.TunableRegistry.update()

        assert dashboard.get_selected() == "two"
        assert dashboard.get_active() == "two"
        assert first_chooser.get_selected() == 2

        tunables.TunableRegistry.reset()
        tunables.TunableRegistry.register_backend(
            "", wpilib.NetworkTablesTunableBackend(nt, "/Tunables")
        )

        second_chooser = make_selectable()
        tunables.publish("auto", second_chooser)
        tunables.TunableRegistry.update()

        assert dashboard.get_selected() == "two"
        assert dashboard.get_active() == "two"
        assert second_chooser.get_selected() == 2
    finally:
        dashboard.close()


def test_motorcontrollergroup():
    t1 = wpilib.PWMMotorController(7)
    t2 = wpilib.PWMMotorController(8)
    t1_sim = wpilib.simulation.PWMMotorControllerSim(t1)
    t2_sim = wpilib.simulation.PWMMotorControllerSim(t2)
    g = wpilib.MotorControllerGroup(t1, t2)

    g.set_throttle(1)
    assert t1_sim.get_throttle() == pytest.approx(1)
    assert t2_sim.get_throttle() == pytest.approx(1)

    g.set_throttle(-1)
    assert t1_sim.get_throttle() == pytest.approx(-1)
    assert t2_sim.get_throttle() == pytest.approx(-1)


def test_motorcontrollergroup_logs_with_native_telemetry_table():
    telemetry.TelemetryRegistry.reset()
    backend = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("", backend)

    t1 = wpilib.PWMMotorController(9)
    t2 = wpilib.PWMMotorController(10)
    group = wpilib.MotorControllerGroup(t1, t2)

    try:
        group.set_throttle(0.5)
        readback = group.get_throttle()
        telemetry.log("motorGroup", group)

        assert backend.get_last_value("/motorGroup/.type") == "Motor Controller"
        assert backend.get_last_value("/motorGroup/Value") == pytest.approx(readback)
    finally:
        telemetry.TelemetryRegistry.reset()


def test_motorcontrollergroup_error():
    with pytest.raises(
        TypeError, match=re.escape("Argument 1 must be a MotorController (got '1')")
    ):
        wpilib.MotorControllerGroup(1)
