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
