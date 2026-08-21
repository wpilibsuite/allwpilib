import romi
import telemetry


def test_romi():
    pass


def test_romigyro_logs_with_native_telemetry_table():
    telemetry.TelemetryRegistry.reset()
    backend = telemetry.MockTelemetryBackend()
    telemetry.TelemetryRegistry.register_backend("", backend)

    try:
        telemetry.log("gyro", romi.RomiGyro())

        assert backend.get_last_value("/gyro/.type") == "Romi Gyro"
        assert backend.get_last_value("/gyro/rate x") == 0.0
        assert backend.get_last_value("/gyro/angle z") == 0.0
    finally:
        telemetry.TelemetryRegistry.reset()
