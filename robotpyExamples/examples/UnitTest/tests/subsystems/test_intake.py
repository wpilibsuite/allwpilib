import pytest
import wpilib
from wpilib.simulation import DoubleSolenoidSim, PWMMotorControllerSim

from constants import IntakeConstants
from subsystems.intake import Intake


@pytest.fixture
def intake() -> Intake:
    return Intake()


@pytest.fixture
def sim_motor() -> PWMMotorControllerSim:
    return PWMMotorControllerSim(IntakeConstants.MOTOR_PORT)


@pytest.fixture
def sim_piston() -> DoubleSolenoidSim:
    return DoubleSolenoidSim(
        wpilib.PneumaticsModuleType.CTRE_PCM,
        IntakeConstants.PISTON_FWD_CHANNEL,
        IntakeConstants.PISTON_REV_CHANNEL,
    )


def test_doesnt_work_when_closed(
    intake: Intake, sim_motor: PWMMotorControllerSim
) -> None:
    intake.retract()  # close the intake
    intake.activate(0.5)  # try to activate the motor
    # make sure that the value set to the motor is 0
    assert sim_motor.get_throttle() == pytest.approx(0.0)


def test_works_when_open(intake: Intake, sim_motor: PWMMotorControllerSim) -> None:
    intake.deploy()
    intake.activate(0.5)
    assert sim_motor.get_throttle() == pytest.approx(0.5)


def test_retract(intake: Intake, sim_piston: DoubleSolenoidSim) -> None:
    intake.retract()
    assert sim_piston.get() == wpilib.DoubleSolenoid.Value.REVERSE


def test_deploy(intake: Intake, sim_piston: DoubleSolenoidSim) -> None:
    intake.deploy()
    assert sim_piston.get() == wpilib.DoubleSolenoid.Value.FORWARD
