import pytest

from wpilib import DoubleSolenoid, PneumaticsModuleType, Solenoid


def test_valid_initialization_ctre(wpilib_state):
    solenoid = DoubleSolenoid(0, 3, PneumaticsModuleType.CTRE_PCM, 2, 3)
    solenoid.set(DoubleSolenoid.Value.REVERSE)
    assert solenoid.get() == DoubleSolenoid.Value.REVERSE

    solenoid.set(DoubleSolenoid.Value.FORWARD)
    assert solenoid.get() == DoubleSolenoid.Value.FORWARD

    solenoid.set(DoubleSolenoid.Value.OFF)
    assert solenoid.get() == DoubleSolenoid.Value.OFF


def test_throw_forward_port_already_initialized_ctre(wpilib_state):
    s = Solenoid(0, 5, PneumaticsModuleType.CTRE_PCM, 2)
    with pytest.raises(RuntimeError):
        DoubleSolenoid(0, 5, PneumaticsModuleType.CTRE_PCM, 2, 3)


def test_throw_reverse_port_already_initialized_ctre(wpilib_state):
    s = Solenoid(0, 6, PneumaticsModuleType.CTRE_PCM, 3)
    with pytest.raises(RuntimeError):
        DoubleSolenoid(0, 6, PneumaticsModuleType.CTRE_PCM, 2, 3)


def test_throw_both_ports_already_initialized_ctre(wpilib_state):
    s0 = Solenoid(0, 6, PneumaticsModuleType.CTRE_PCM, 2)
    s1 = Solenoid(0, 6, PneumaticsModuleType.CTRE_PCM, 3)
    with pytest.raises(RuntimeError):
        DoubleSolenoid(0, 6, PneumaticsModuleType.CTRE_PCM, 2, 3)


def test_toggle_ctre(wpilib_state):
    solenoid = DoubleSolenoid(0, 4, PneumaticsModuleType.CTRE_PCM, 2, 3)
    solenoid.set(DoubleSolenoid.Value.REVERSE)

    solenoid.toggle()
    assert solenoid.get() == DoubleSolenoid.Value.FORWARD

    solenoid.toggle()
    assert solenoid.get() == DoubleSolenoid.Value.REVERSE

    solenoid.set(DoubleSolenoid.Value.OFF)
    solenoid.toggle()
    assert solenoid.get() == DoubleSolenoid.Value.OFF


def test_invalid_forward_port_ctre(wpilib_state):
    with pytest.raises(RuntimeError):
        DoubleSolenoid(0, 0, PneumaticsModuleType.CTRE_PCM, 100, 1)


def test_invalid_reverse_port_ctre(wpilib_state):
    with pytest.raises(RuntimeError):
        DoubleSolenoid(0, 0, PneumaticsModuleType.CTRE_PCM, 0, 100)


def test_valid_initialization_rev(wpilib_state):
    solenoid = DoubleSolenoid(0, 3, PneumaticsModuleType.REV_PH, 2, 3)
    solenoid.set(DoubleSolenoid.Value.REVERSE)
    assert solenoid.get() == DoubleSolenoid.Value.REVERSE

    solenoid.set(DoubleSolenoid.Value.FORWARD)
    assert solenoid.get() == DoubleSolenoid.Value.FORWARD

    solenoid.set(DoubleSolenoid.Value.OFF)
    assert solenoid.get() == DoubleSolenoid.Value.OFF


def test_throw_forward_port_already_initialized_rev(wpilib_state):
    s = Solenoid(0, 5, PneumaticsModuleType.REV_PH, 2)
    with pytest.raises(RuntimeError):
        DoubleSolenoid(0, 5, PneumaticsModuleType.REV_PH, 2, 3)


def test_throw_reverse_port_already_initialized_rev(wpilib_state):
    s = Solenoid(0, 6, PneumaticsModuleType.REV_PH, 3)
    with pytest.raises(RuntimeError):
        DoubleSolenoid(0, 6, PneumaticsModuleType.REV_PH, 2, 3)


def test_throw_both_ports_already_initialized_rev(wpilib_state):
    s0 = Solenoid(0, 6, PneumaticsModuleType.REV_PH, 2)
    s1 = Solenoid(0, 6, PneumaticsModuleType.REV_PH, 3)
    with pytest.raises(RuntimeError):
        DoubleSolenoid(0, 6, PneumaticsModuleType.REV_PH, 2, 3)


def test_toggle_rev(wpilib_state):
    solenoid = DoubleSolenoid(0, 4, PneumaticsModuleType.REV_PH, 2, 3)
    solenoid.set(DoubleSolenoid.Value.REVERSE)

    solenoid.toggle()
    assert solenoid.get() == DoubleSolenoid.Value.FORWARD

    solenoid.toggle()
    assert solenoid.get() == DoubleSolenoid.Value.REVERSE

    solenoid.set(DoubleSolenoid.Value.OFF)
    solenoid.toggle()
    assert solenoid.get() == DoubleSolenoid.Value.OFF


def test_invalid_forward_port_rev(wpilib_state):
    with pytest.raises(RuntimeError):
        DoubleSolenoid(0, 0, PneumaticsModuleType.REV_PH, 100, 1)


def test_invalid_reverse_port_rev(wpilib_state):
    with pytest.raises(RuntimeError):
        DoubleSolenoid(0, 0, PneumaticsModuleType.REV_PH, 0, 100)
