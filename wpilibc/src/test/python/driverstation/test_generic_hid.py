import pytest

from wpilib import GenericHID
from wpilib.simulation import GenericHIDSim

RumbleType = GenericHID.RumbleType
_EPSILON = 0.0001


def test_rumble_range(wpilib_state):
    hid = GenericHID(0)
    sim = GenericHIDSim(0)

    for i in range(101):
        value = i / 100.0

        hid.setRumble(RumbleType.LEFT_RUMBLE, value)
        assert sim.getRumble(RumbleType.LEFT_RUMBLE) == pytest.approx(value, abs=_EPSILON)

        hid.setRumble(RumbleType.RIGHT_RUMBLE, value)
        assert sim.getRumble(RumbleType.RIGHT_RUMBLE) == pytest.approx(value, abs=_EPSILON)

        hid.setRumble(RumbleType.LEFT_TRIGGER_RUMBLE, value)
        assert sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE) == pytest.approx(
            value, abs=_EPSILON
        )

        hid.setRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, value)
        assert sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE) == pytest.approx(
            value, abs=_EPSILON
        )


def test_rumble_types(wpilib_state):
    hid = GenericHID(0)
    sim = GenericHIDSim(0)

    # Make sure all are off
    hid.setRumble(RumbleType.LEFT_RUMBLE, 0)
    hid.setRumble(RumbleType.LEFT_TRIGGER_RUMBLE, 0)
    hid.setRumble(RumbleType.RIGHT_RUMBLE, 0)
    hid.setRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, 0)
    assert sim.getRumble(RumbleType.LEFT_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.RIGHT_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE) == pytest.approx(0, abs=_EPSILON)

    # Left only
    hid.setRumble(RumbleType.LEFT_RUMBLE, 1)
    assert sim.getRumble(RumbleType.LEFT_RUMBLE) == pytest.approx(1, abs=_EPSILON)
    assert sim.getRumble(RumbleType.RIGHT_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    hid.setRumble(RumbleType.LEFT_RUMBLE, 0)

    # Right only
    hid.setRumble(RumbleType.RIGHT_RUMBLE, 1)
    assert sim.getRumble(RumbleType.LEFT_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.RIGHT_RUMBLE) == pytest.approx(1, abs=_EPSILON)
    assert sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    hid.setRumble(RumbleType.RIGHT_RUMBLE, 0)

    # Left trigger only
    hid.setRumble(RumbleType.LEFT_TRIGGER_RUMBLE, 1)
    assert sim.getRumble(RumbleType.LEFT_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.RIGHT_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE) == pytest.approx(1, abs=_EPSILON)
    assert sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    hid.setRumble(RumbleType.LEFT_TRIGGER_RUMBLE, 0)

    # Right trigger only
    hid.setRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, 1)
    assert sim.getRumble(RumbleType.LEFT_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.RIGHT_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE) == pytest.approx(0, abs=_EPSILON)
    assert sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE) == pytest.approx(1, abs=_EPSILON)
    hid.setRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, 0)
