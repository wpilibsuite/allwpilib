import pytest

import telemetry
import tunables
from wpimath import PIDController


@pytest.fixture
def telemetry_backend():
    telemetry.TelemetryRegistry.reset()
    backend = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("", backend)
    try:
        yield backend
    finally:
        telemetry.TelemetryRegistry.reset()


@pytest.fixture
def tunable_backend():
    tunables.TunableRegistry.reset()
    backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("", backend)
    try:
        yield backend
    finally:
        tunables.TunableRegistry.reset()


def test_pid_controller_logs_telemetry(telemetry_backend):
    controller = PIDController(0.5, 0.1, 0.01)
    controller.set_i_zone(5.0)
    controller.calculate(7.0, 10.0)

    telemetry.log("pid", controller)

    assert telemetry_backend.get_last_value("/pid/.type") == "PIDController"
    assert telemetry_backend.get_last_value("/pid/p") == pytest.approx(0.5)
    assert telemetry_backend.get_last_value("/pid/i") == pytest.approx(0.1)
    assert telemetry_backend.get_last_value("/pid/d") == pytest.approx(0.01)
    assert telemetry_backend.get_last_value("/pid/izone") == pytest.approx(5.0)
    assert telemetry_backend.get_last_value("/pid/setpoint") == pytest.approx(10.0)
    assert telemetry_backend.get_last_value("/pid/measurement") == pytest.approx(7.0)
    assert telemetry_backend.get_last_value("/pid/error") == pytest.approx(3.0)


def test_pid_controller_python_log_override_receives_native_table(telemetry_backend):
    from telemetry import _telemetry

    received = []

    class CustomPID(PIDController):
        def log_to(self, table: _telemetry._NativeTelemetryTable) -> None:
            received.append(table)
            table.log("value", 9, element_type=None)
            table.log("values", [1, 2], element_type=int)
            with pytest.raises(TypeError, match="element_type must be a Python type"):
                table.log("invalid", [1], element_type="int[]")

    telemetry.log("pid", CustomPID(0.5, 0.0, 0.0))

    assert len(received) == 1
    assert isinstance(received[0], _telemetry._NativeTelemetryTable)
    assert telemetry_backend.get_last_value("/pid/value") == 9
    assert telemetry_backend.get_last_value("/pid/values") == [1, 2]
    assert telemetry_backend.get_last_value("/pid/invalid") is None


def test_pid_controller_python_publish_override_receives_native_table(
    tunable_backend,
):
    received: list[tunables.TunableTable] = []

    class CustomPID(PIDController):
        def publish_tunable(self, table: tunables.TunableTable) -> None:
            received.append(table)
            table.add_double("custom", 4.0)

    tunables.publish("pid", CustomPID(0.5, 0.0, 0.0))

    assert len(received) == 1
    assert type(received[0]) is tunables.TunableTable
    assert tunable_backend.get_value("/pid/custom") == pytest.approx(4.0)


def test_pid_controller_python_subclass_uses_native_tunable_fallback(
    tunable_backend,
):
    class DerivedPIDController(PIDController):
        pass

    controller = DerivedPIDController(0.5, 0.1, 0.01)
    controller.set_i_zone(5.0)

    assert tunables.publish("derivedPid", controller) is True
    assert tunable_backend.get_value("/derivedPid/p") == pytest.approx(0.5)
    assert tunable_backend.get_value("/derivedPid/i") == pytest.approx(0.1)
    assert tunable_backend.get_value("/derivedPid/d") == pytest.approx(0.01)
    assert tunable_backend.get_value("/derivedPid/izone") == pytest.approx(5.0)


def test_pid_controller_tuned_setpoint_updates_setpoint_state(tunable_backend):
    controller = PIDController(0.5, 0.0, 0.0)
    tunables.publish("pid", controller)

    assert controller.at_setpoint() is False

    tunable_backend.set_double("/pid/setpoint", 50.0)
    tunables.TunableRegistry.update()

    assert controller.get_setpoint() == pytest.approx(50.0)
    assert controller.get_error() == pytest.approx(50.0)

    controller.calculate(50.0)

    assert controller.at_setpoint() is True
