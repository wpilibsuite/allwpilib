import pytest
from wpimath import DifferentialDriveWheelAccelerations
from wpimath.units import feet_to_meters

EPSILON = 1e-9


def test_default_constructor():
    wheel_accelerations = DifferentialDriveWheelAccelerations()

    assert wheel_accelerations.left == pytest.approx(0.0, abs=EPSILON)
    assert wheel_accelerations.right == pytest.approx(0.0, abs=EPSILON)


def test_parameterized_constructor():
    wheel_accelerations = DifferentialDriveWheelAccelerations(1.5, 2.5)

    assert wheel_accelerations.left == pytest.approx(1.5, abs=EPSILON)
    assert wheel_accelerations.right == pytest.approx(2.5, abs=EPSILON)


def test_plus():
    left = DifferentialDriveWheelAccelerations(1.0, 0.5)
    right = DifferentialDriveWheelAccelerations(2.0, 1.5)

    wheel_accelerations = left + right

    assert wheel_accelerations.left == pytest.approx(3.0, abs=EPSILON)
    assert wheel_accelerations.right == pytest.approx(2.0, abs=EPSILON)


def test_minus():
    left = DifferentialDriveWheelAccelerations(1.0, 0.5)
    right = DifferentialDriveWheelAccelerations(2.0, 0.5)

    wheel_accelerations = left - right

    assert wheel_accelerations.left == pytest.approx(-1.0, abs=EPSILON)
    assert wheel_accelerations.right == pytest.approx(0.0, abs=EPSILON)


def test_unary_minus():
    accel = DifferentialDriveWheelAccelerations(1.0, 0.5)

    wheel_accelerations = -accel

    assert wheel_accelerations.left == pytest.approx(-1.0, abs=EPSILON)
    assert wheel_accelerations.right == pytest.approx(-0.5, abs=EPSILON)


def test_multiplication():
    accel = DifferentialDriveWheelAccelerations(1.0, 0.5)

    wheel_accelerations = accel * 2.0

    assert wheel_accelerations.left == pytest.approx(2.0, abs=EPSILON)
    assert wheel_accelerations.right == pytest.approx(1.0, abs=EPSILON)


def test_division():
    accel = DifferentialDriveWheelAccelerations(1.0, 0.5)

    wheel_accelerations = accel / 2.0

    assert wheel_accelerations.left == pytest.approx(0.5, abs=EPSILON)
    assert wheel_accelerations.right == pytest.approx(0.25, abs=EPSILON)


def test_feet_constructor():
    accel = DifferentialDriveWheelAccelerations.from_fps(10, 11)

    assert accel.left == pytest.approx(feet_to_meters(10), abs=EPSILON)
    assert accel.right == pytest.approx(feet_to_meters(11), abs=EPSILON)

    assert accel.left_fpss == pytest.approx(10, abs=EPSILON)
    assert accel.right_fpss == pytest.approx(11, abs=EPSILON)


def test_repr():
    accel = DifferentialDriveWheelAccelerations(1, 2)

    assert (
        str(accel)
        == "DifferentialDriveWheelAccelerations(left=1.000000, right=2.000000)"
    )
