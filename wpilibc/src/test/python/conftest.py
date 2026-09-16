import logging

import pytest
import ntcore
import telemetry
import tunables
import wpilib
from wpilib.simulation._simulation import _reset_wpilib_simulation_data

pytest_plugins = "pytester"


@pytest.fixture
def cfg_logging(caplog):
    caplog.set_level(logging.INFO)


@pytest.fixture(scope="function")
def wpilib_state():
    try:
        yield None
    finally:
        _reset_wpilib_simulation_data()


@pytest.fixture(scope="function")
def nt(cfg_logging, wpilib_state):
    instance = ntcore.NetworkTableInstance.get_default()
    instance.start_local()
    telemetry.TelemetryRegistry.reset()
    telemetry.TelemetryRegistry.register_backend(
        "",
        wpilib.NetworkTablesTelemetryBackend(instance, "/Telemetry"),
    )
    tunables.TunableRegistry.reset()
    tunables.TunableRegistry.register_backend(
        "",
        wpilib.NetworkTablesTunableBackend(instance, "/Tunables"),
    )

    try:
        yield instance
    finally:
        telemetry.TelemetryRegistry.reset()
        tunables.TunableRegistry.reset()
        instance.stop_local()
        instance._reset()
