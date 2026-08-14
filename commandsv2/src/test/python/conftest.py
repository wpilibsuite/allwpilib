import commands2
import pytest
from ntcore import NetworkTableInstance
from wpilib.simulation import DriverStationSim


@pytest.fixture(autouse=True)
def scheduler():
    commands2.CommandScheduler.reset_instance()
    DriverStationSim.set_enabled(True)
    DriverStationSim.notify_new_data()
    return commands2.CommandScheduler.get_instance()


@pytest.fixture()
def nt_instance():
    inst = NetworkTableInstance.create()
    inst.start_local()
    yield inst
    inst.stop_local()
