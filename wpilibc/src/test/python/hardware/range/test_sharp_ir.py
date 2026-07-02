import pytest

from wpilib import SharpIR
from wpilib.simulation import SharpIRSim


def test_sim_devices(wpilib_state):
    s = SharpIR.GP2Y0A02YK0F(1)
    sim = SharpIRSim(s)

    assert s.getRange() == pytest.approx(0.2)

    sim.setRange(0.3)
    assert s.getRange() == pytest.approx(0.3)

    # Clamped to max range of 1.5 m for GP2Y0A02YK0F
    sim.setRange(3.0)
    assert s.getRange() == pytest.approx(1.5)
