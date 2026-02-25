import logging

import pytest
import ntcore
import wpilib
from wpilib.simulation._simulation import _resetWpilibSimulationData

pytest_plugins = "pytester"


@pytest.fixture
def cfg_logging(caplog):
    caplog.set_level(logging.INFO)


@pytest.fixture(scope="function")
def wpilib_state():
    try:
        yield None
    finally:
        _resetWpilibSimulationData()


@pytest.fixture(scope="function")
def nt(cfg_logging, wpilib_state):
    instance = ntcore.NetworkTableInstance.getDefault()
    instance.startLocal()

    try:
        yield instance
    finally:
        instance.stopLocal()
        instance._reset()
