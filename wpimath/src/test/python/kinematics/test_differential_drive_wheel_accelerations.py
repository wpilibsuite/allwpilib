import pytest
from wpimath.kinematics import DifferentialDriveWheelAccelerations
from wpimath.units import feetToMeters

K_EPSILON = 1e-9


def test_default_constructor():
    wheel_accelerations = DifferentialDriveWheelAccelerations()

    assert wheel_accelerations.left == pytest.approx(0.0, abs=K_EPSILON)
    assert wheel_accelerations.right == pytest.approx(0.0, abs=K_EPSILON)


def test_parameterized_constructor():
    wheel_accelerations = DifferentialDriveWheelAccelerations(1.5, 2.5)

    assert wheel_accelerations.left == pytest.approx(1.5, abs=K_EPSILON)
    assert wheel_accelerations.right == pytest.approx(2.5, abs=K_EPSILON)


def test_plus():
    left = DifferentialDriveWheelAccelerations(1.0, 0.5)
    right = DifferentialDriveWheelAccelerations(2.0, 1.5)

    wheel_accelerations = left + right

    assert wheel_accelerations.left == pytest.approx(3.0, abs=K_EPSILON)
    assert wheel_accelerations.right == pytest.approx(2.0, abs=K_EPSILON)


def test_minus():
    left = DifferentialDriveWheelAccelerations(1.0, 0.5)
    right = DifferentialDriveWheelAccelerations(2.0, 0.5)

    wheel_accelerations = left - right

    assert wheel_accelerations.left == pytest.approx(-1.0, abs=K_EPSILON)
    assert wheel_accelerations.right == pytest.approx(0.0, abs=K_EPSILON)


def test_unary_minus():
    accel = DifferentialDriveWheelAccelerations(1.0, 0.5)

    wheel_accelerations = -accel

    assert wheel_accelerations.left == pytest.approx(-1.0, abs=K_EPSILON)
    assert wheel_accelerations.right == pytest.approx(-0.5, abs=K_EPSILON)


def test_multiplication():
    accel = DifferentialDriveWheelAccelerations(1.0, 0.5)

    wheel_accelerations = accel * 2.0

    assert wheel_accelerations.left == pytest.approx(2.0, abs=K_EPSILON)
    assert wheel_accelerations.right == pytest.approx(1.0, abs=K_EPSILON)


def test_division():
    accel = DifferentialDriveWheelAccelerations(1.0, 0.5)

    wheel_accelerations = accel / 2.0

    assert wheel_accelerations.left == pytest.approx(0.5, abs=K_EPSILON)
    assert wheel_accelerations.right == pytest.approx(0.25, abs=K_EPSILON)


def test_feet_constructor():
    accel = DifferentialDriveWheelAccelerations.fromFpsSq(10, 11)

    assert accel.left == pytest.approx(feetToMeters(10), abs=K_EPSILON)
    assert accel.right == pytest.approx(feetToMeters(11), abs=K_EPSILON)

    assert accel.left_fpss == pytest.approx(10, abs=K_EPSILON)
    assert accel.right_fpss == pytest.approx(11, abs=K_EPSILON)


def test_repr():
    accel = DifferentialDriveWheelAccelerations(1, 2)

    assert (
        str(accel)
        == "DifferentialDriveWheelAccelerations(left=1.000000, right=2.000000)"
    )
