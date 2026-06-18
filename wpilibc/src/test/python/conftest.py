import logging

import pytest
from wpilib.simulation._simulation import _resetWpilibSimulationData

import ntcore

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
