import pytest
from wpimath.kinematics import MecanumDriveWheelAccelerations
from wpimath.units import feetToMeters

# Epsilon matching the C++ test
K_EPSILON = 1e-9


def test_default_constructor():
    wheel_accelerations = MecanumDriveWheelAccelerations()

    assert wheel_accelerations.frontLeft == pytest.approx(0.0, abs=K_EPSILON)
    assert wheel_accelerations.frontRight == pytest.approx(0.0, abs=K_EPSILON)
    assert wheel_accelerations.rearLeft == pytest.approx(0.0, abs=K_EPSILON)
    assert wheel_accelerations.rearRight == pytest.approx(0.0, abs=K_EPSILON)


def test_parameterized_constructor():
    # Accelerations: FL=1.0, FR=2.0, RL=3.0, RR=4.0 m/s^2
    wheel_accelerations = MecanumDriveWheelAccelerations(1.0, 2.0, 3.0, 4.0)

    assert wheel_accelerations.frontLeft == pytest.approx(1.0, abs=K_EPSILON)
    assert wheel_accelerations.frontRight == pytest.approx(2.0, abs=K_EPSILON)
    assert wheel_accelerations.rearLeft == pytest.approx(3.0, abs=K_EPSILON)
    assert wheel_accelerations.rearRight == pytest.approx(4.0, abs=K_EPSILON)


def test_plus():
    left = MecanumDriveWheelAccelerations(1.0, 0.5, 0.75, 0.25)
    right = MecanumDriveWheelAccelerations(1.0, 1.5, 0.25, 1.75)

    wheel_accelerations = left + right

    assert wheel_accelerations.frontLeft == pytest.approx(2.0, abs=K_EPSILON)
    assert wheel_accelerations.frontRight == pytest.approx(2.0, abs=K_EPSILON)
    assert wheel_accelerations.rearLeft == pytest.approx(1.0, abs=K_EPSILON)
    assert wheel_accelerations.rearRight == pytest.approx(2.0, abs=K_EPSILON)


def test_minus():
    left = MecanumDriveWheelAccelerations(1.0, 0.5, 0.75, 0.25)
    right = MecanumDriveWheelAccelerations(2.0, 1.5, 0.25, 0.75)

    wheel_accelerations = left - right

    assert wheel_accelerations.frontLeft == pytest.approx(-1.0, abs=K_EPSILON)
    assert wheel_accelerations.frontRight == pytest.approx(-1.0, abs=K_EPSILON)
    assert wheel_accelerations.rearLeft == pytest.approx(0.5, abs=K_EPSILON)
    assert wheel_accelerations.rearRight == pytest.approx(-0.5, abs=K_EPSILON)


def test_unary_minus():
    accel = MecanumDriveWheelAccelerations(1.0, 0.5, 0.75, 0.25)

    wheel_accelerations = -accel

    assert wheel_accelerations.frontLeft == pytest.approx(-1.0, abs=K_EPSILON)
    assert wheel_accelerations.frontRight == pytest.approx(-0.5, abs=K_EPSILON)
    assert wheel_accelerations.rearLeft == pytest.approx(-0.75, abs=K_EPSILON)
    assert wheel_accelerations.rearRight == pytest.approx(-0.25, abs=K_EPSILON)


def test_multiplication():
    accel = MecanumDriveWheelAccelerations(1.0, 0.5, 0.75, 0.25)

    wheel_accelerations = accel * 2.0

    assert wheel_accelerations.frontLeft == pytest.approx(2.0, abs=K_EPSILON)
    assert wheel_accelerations.frontRight == pytest.approx(1.0, abs=K_EPSILON)
    assert wheel_accelerations.rearLeft == pytest.approx(1.5, abs=K_EPSILON)
    assert wheel_accelerations.rearRight == pytest.approx(0.5, abs=K_EPSILON)


def test_division():
    accel = MecanumDriveWheelAccelerations(2.0, 1.0, 1.5, 0.5)

    wheel_accelerations = accel / 2.0

    assert wheel_accelerations.frontLeft == pytest.approx(1.0, abs=K_EPSILON)
    assert wheel_accelerations.frontRight == pytest.approx(0.5, abs=K_EPSILON)
    assert wheel_accelerations.rearLeft == pytest.approx(0.75, abs=K_EPSILON)
    assert wheel_accelerations.rearRight == pytest.approx(0.25, abs=K_EPSILON)


def test_feet_constructor():
    accel = MecanumDriveWheelAccelerations.fromFpsSq(10, 11, 12, 13)

    assert accel.frontLeft == pytest.approx(feetToMeters(10), abs=K_EPSILON)
    assert accel.frontRight == pytest.approx(feetToMeters(11), abs=K_EPSILON)
    assert accel.rearLeft == pytest.approx(feetToMeters(12), abs=K_EPSILON)
    assert accel.rearRight == pytest.approx(feetToMeters(13), abs=K_EPSILON)

    assert accel.front_left_fpss == pytest.approx(10, abs=K_EPSILON)
    assert accel.front_right_fpss == pytest.approx(11, abs=K_EPSILON)
    assert accel.rear_left_fpss == pytest.approx(12, abs=K_EPSILON)
    assert accel.rear_right_fpss == pytest.approx(13, abs=K_EPSILON)


def test_repr():
    accel = MecanumDriveWheelAccelerations(1, 2, 3, 4)

    assert (
        str(accel)
        == "MecanumDriveWheelAccelerations(frontLeft=1.000000, frontRight=2.000000, rearLeft=3.000000, rearRight=4.000000)"
    )
