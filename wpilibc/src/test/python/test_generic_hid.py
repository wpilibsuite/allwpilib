import pytest

from wpilib import DriverStation, DriverStationBackend, GenericHID
from wpilib.simulation import DriverStationSim, GenericHIDSim


RumbleType = GenericHID.RumbleType
RUMBLE_TYPES = (
    RumbleType.LEFT_RUMBLE,
    RumbleType.RIGHT_RUMBLE,
    RumbleType.LEFT_TRIGGER_RUMBLE,
    RumbleType.RIGHT_TRIGGER_RUMBLE,
)


@pytest.fixture(autouse=True)
def reset_driver_station_sim():
    DriverStationSim.resetData()
    yield
    DriverStationBackend.resetCachedHIDData()
    DriverStationSim.resetData()


def test_rumble_range() -> None:
    hid = DriverStationBackend.constructGenericHID(0)
    sim = GenericHIDSim(0)

    for i in range(101):
        rumble_value = i / 100.0

        for rumble_type in RUMBLE_TYPES:
            hid.setRumble(rumble_type, rumble_value)
            assert sim.getRumble(rumble_type) == pytest.approx(
                rumble_value, abs=0.0001
            )


def test_rumble_types() -> None:
    hid = DriverStationBackend.constructGenericHID(0)
    sim = GenericHIDSim(0)

    for rumble_type in RUMBLE_TYPES:
        hid.setRumble(rumble_type, 0)

    for rumble_type in RUMBLE_TYPES:
        assert sim.getRumble(rumble_type) == pytest.approx(0, abs=0.0001)

    for active_rumble_type in RUMBLE_TYPES:
        hid.setRumble(active_rumble_type, 1)

        for rumble_type in RUMBLE_TYPES:
            expected = 1 if rumble_type == active_rumble_type else 0
            assert sim.getRumble(rumble_type) == pytest.approx(
                expected, abs=0.0001
            )

        hid.setRumble(active_rumble_type, 0)


def test_cached_hid_data_reset() -> None:
    DriverStation.getGenericHID(0)
    DriverStation.getGamepad(0)

    DriverStationBackend.resetCachedHIDData()

    assert DriverStation.getGenericHID(0).getPort() == 0
    assert DriverStation.getGamepad(0).getPort() == 0
