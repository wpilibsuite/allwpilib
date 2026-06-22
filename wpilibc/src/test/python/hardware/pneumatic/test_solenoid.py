import pytest

from wpilib import DoubleSolenoid, PneumaticsModuleType, Solenoid


def test_valid_initialization_ctre(wpilib_state):
    solenoid = Solenoid(0, 3, PneumaticsModuleType.CTRE_PCM, 2)
    assert solenoid.getChannel() == 2

    solenoid.set(True)
    assert solenoid.get()

    solenoid.set(False)
    assert not solenoid.get()


def test_double_initialization_ctre(wpilib_state):
    s = Solenoid(0, 3, PneumaticsModuleType.CTRE_PCM, 2)
    with pytest.raises(RuntimeError):
        Solenoid(0, 3, PneumaticsModuleType.CTRE_PCM, 2)


def test_double_initialization_from_double_solenoid_ctre(wpilib_state):
    ds = DoubleSolenoid(0, 3, PneumaticsModuleType.CTRE_PCM, 2, 3)
    with pytest.raises(RuntimeError):
        Solenoid(0, 3, PneumaticsModuleType.CTRE_PCM, 2)


def test_invalid_channel_ctre(wpilib_state):
    with pytest.raises(RuntimeError):
        Solenoid(0, 3, PneumaticsModuleType.CTRE_PCM, 100)


def test_toggle_ctre(wpilib_state):
    solenoid = Solenoid(0, 3, PneumaticsModuleType.CTRE_PCM, 2)
    solenoid.set(True)
    assert solenoid.get()

    solenoid.toggle()
    assert not solenoid.get()

    solenoid.toggle()
    assert solenoid.get()


def test_valid_initialization_rev(wpilib_state):
    solenoid = Solenoid(0, 3, PneumaticsModuleType.REV_PH, 2)
    assert solenoid.getChannel() == 2

    solenoid.set(True)
    assert solenoid.get()

    solenoid.set(False)
    assert not solenoid.get()


def test_double_initialization_rev(wpilib_state):
    s = Solenoid(0, 3, PneumaticsModuleType.REV_PH, 2)
    with pytest.raises(RuntimeError):
        Solenoid(0, 3, PneumaticsModuleType.REV_PH, 2)


def test_double_initialization_from_double_solenoid_rev(wpilib_state):
    ds = DoubleSolenoid(0, 3, PneumaticsModuleType.REV_PH, 2, 3)
    with pytest.raises(RuntimeError):
        Solenoid(0, 3, PneumaticsModuleType.REV_PH, 2)


def test_invalid_channel_rev(wpilib_state):
    with pytest.raises(RuntimeError):
        Solenoid(0, 3, PneumaticsModuleType.REV_PH, 100)


def test_toggle_rev(wpilib_state):
    solenoid = Solenoid(0, 3, PneumaticsModuleType.REV_PH, 2)
    solenoid.set(True)
    assert solenoid.get()

    solenoid.toggle()
    assert not solenoid.get()

    solenoid.toggle()
    assert solenoid.get()
