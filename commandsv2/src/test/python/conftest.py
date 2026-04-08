import commands2
import pytest
from ntcore import NetworkTableInstance
from wpilib.simulation import DriverStationSim


@pytest.fixture(autouse=True)
def scheduler():
    commands2.CommandScheduler.resetInstance()
    DriverStationSim.setEnabled(True)
    DriverStationSim.notifyNewData()
    return commands2.CommandScheduler.getInstance()


@pytest.fixture()
def nt_instance():
    inst = NetworkTableInstance.create()
    inst.startLocal()
    yield inst
    inst.stopLocal()
