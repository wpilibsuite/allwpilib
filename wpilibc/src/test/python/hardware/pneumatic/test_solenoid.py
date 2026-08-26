import pytest

from wpilib import CANBus, DoubleSolenoid, PneumaticsModuleType, Solenoid

BUS = CANBus.CAN_S0


def test_valid_initialization_ctre(wpilib_state):
    solenoid = Solenoid(BUS, 3, PneumaticsModuleType.CTRE_PCM, 2)
    assert solenoid.get_channel() == 2

    solenoid.set(True)
    assert solenoid.get()

    solenoid.set(False)
    assert not solenoid.get()


def test_double_initialization_ctre(wpilib_state):
    s = Solenoid(BUS, 3, PneumaticsModuleType.CTRE_PCM, 2)
    with pytest.raises(RuntimeError):
        Solenoid(BUS, 3, PneumaticsModuleType.CTRE_PCM, 2)


def test_double_initialization_from_double_solenoid_ctre(wpilib_state):
    ds = DoubleSolenoid(BUS, 3, PneumaticsModuleType.CTRE_PCM, 2, 3)
    with pytest.raises(RuntimeError):
        Solenoid(BUS, 3, PneumaticsModuleType.CTRE_PCM, 2)


def test_invalid_channel_ctre(wpilib_state):
    with pytest.raises(RuntimeError):
        Solenoid(BUS, 3, PneumaticsModuleType.CTRE_PCM, 100)


def test_toggle_ctre(wpilib_state):
    solenoid = Solenoid(BUS, 3, PneumaticsModuleType.CTRE_PCM, 2)
    solenoid.set(True)
    assert solenoid.get()

    solenoid.toggle()
    assert not solenoid.get()

    solenoid.toggle()
    assert solenoid.get()


def test_valid_initialization_rev(wpilib_state):
    solenoid = Solenoid(BUS, 3, PneumaticsModuleType.REV_PH, 2)
    assert solenoid.get_channel() == 2

    solenoid.set(True)
    assert solenoid.get()

    solenoid.set(False)
    assert not solenoid.get()


def test_double_initialization_rev(wpilib_state):
    s = Solenoid(BUS, 3, PneumaticsModuleType.REV_PH, 2)
    with pytest.raises(RuntimeError):
        Solenoid(BUS, 3, PneumaticsModuleType.REV_PH, 2)


def test_double_initialization_from_double_solenoid_rev(wpilib_state):
    ds = DoubleSolenoid(BUS, 3, PneumaticsModuleType.REV_PH, 2, 3)
    with pytest.raises(RuntimeError):
        Solenoid(BUS, 3, PneumaticsModuleType.REV_PH, 2)


def test_invalid_channel_rev(wpilib_state):
    with pytest.raises(RuntimeError):
        Solenoid(BUS, 3, PneumaticsModuleType.REV_PH, 100)


def test_toggle_rev(wpilib_state):
    solenoid = Solenoid(BUS, 3, PneumaticsModuleType.REV_PH, 2)
    solenoid.set(True)
    assert solenoid.get()

    solenoid.toggle()
    assert not solenoid.get()

    solenoid.toggle()
    assert solenoid.get()
