import pytest

from wpilib import AnalogInput, AnalogPotentiometer
from wpilib.simulation import AnalogInputSim, RoboRioSim


def test_initialize_with_analog_input(wpilib_state):
    ai = AnalogInput(0)
    pot = AnalogPotentiometer(ai)
    sim = AnalogInputSim(ai)

    RoboRioSim.reset_data()
    sim.set_voltage(2.8)
    assert pot.get() == pytest.approx(2.8 / 3.3)


def test_initialize_with_analog_input_and_scale(wpilib_state):
    ai = AnalogInput(0)
    pot = AnalogPotentiometer(ai, 270.0)
    RoboRioSim.reset_data()
    sim = AnalogInputSim(ai)

    sim.set_voltage(3.3)
    assert pot.get() == pytest.approx(270.0)

    sim.set_voltage(2.5)
    assert pot.get() == pytest.approx((2.5 / 3.3) * 270.0)

    sim.set_voltage(0.0)
    assert pot.get() == pytest.approx(0.0)


def test_initialize_with_channel(wpilib_state):
    pot = AnalogPotentiometer(1)
    sim = AnalogInputSim(1)

    sim.set_voltage(3.3)
    assert pot.get() == pytest.approx(1.0)


def test_initialize_with_channel_and_scale(wpilib_state):
    pot = AnalogPotentiometer(1, 180.0)
    RoboRioSim.reset_data()
    sim = AnalogInputSim(1)

    sim.set_voltage(3.3)
    assert pot.get() == pytest.approx(180.0)

    sim.set_voltage(0.0)
    assert pot.get() == pytest.approx(0.0)


def test_with_modified_battery_voltage(wpilib_state):
    pot = AnalogPotentiometer(1, 180.0, 90.0)
    RoboRioSim.reset_data()
    sim = AnalogInputSim(1)

    # Test at 3.3 V
    sim.set_voltage(3.3)
    assert pot.get() == pytest.approx(270.0)

    sim.set_voltage(0.0)
    assert pot.get() == pytest.approx(90.0)

    # Simulate lower battery voltage
    RoboRioSim.set_user_voltage_3v3(2.5)

    sim.set_voltage(2.5)
    assert pot.get() == pytest.approx(270.0)

    sim.set_voltage(2.0)
    assert pot.get() == pytest.approx(234.0)

    sim.set_voltage(0.0)
    assert pot.get() == pytest.approx(90.0)
