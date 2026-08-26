import pytest
import wpilib
import wpilib_drivers
from wpilib.simulation import PWMMotorControllerSim


@pytest.mark.parametrize(
    "motor_type",
    [
        wpilib_drivers.Koors40,
        wpilib_drivers.PWMSparkFlex,
        wpilib_drivers.PWMSparkMax,
        wpilib_drivers.PWMTalonFX,
        wpilib_drivers.PWMTalonSRX,
        wpilib_drivers.PWMVenom,
        wpilib_drivers.PWMVictorSPX,
        wpilib_drivers.Spark,
        wpilib_drivers.SparkMini,
        wpilib_drivers.Talon,
        wpilib_drivers.VictorSP,
    ],
)
def test_pwm_motor_controller(motor_type):
    motor = motor_type(0)
    sim = PWMMotorControllerSim(motor)

    motor.set_throttle(0.354)

    assert isinstance(motor, wpilib.PWMMotorController)
    assert sim.get_throttle() == pytest.approx(0.354)
