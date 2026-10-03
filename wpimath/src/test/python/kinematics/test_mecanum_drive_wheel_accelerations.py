import pytest
from wpimath import MecanumDriveWheelAccelerations
from wpimath.units import feet_to_meters

# Epsilon matching the C++ test
EPSILON = 1e-9


def test_default_constructor():
    wheel_accelerations = MecanumDriveWheelAccelerations()

    assert wheel_accelerations.front_left == pytest.approx(0.0, abs=EPSILON)
    assert wheel_accelerations.front_right == pytest.approx(0.0, abs=EPSILON)
    assert wheel_accelerations.rear_left == pytest.approx(0.0, abs=EPSILON)
    assert wheel_accelerations.rear_right == pytest.approx(0.0, abs=EPSILON)


def test_parameterized_constructor():
    # Accelerations: FL=1.0, FR=2.0, RL=3.0, RR=4.0 m/s^2
    wheel_accelerations = MecanumDriveWheelAccelerations(1.0, 2.0, 3.0, 4.0)

    assert wheel_accelerations.front_left == pytest.approx(1.0, abs=EPSILON)
    assert wheel_accelerations.front_right == pytest.approx(2.0, abs=EPSILON)
    assert wheel_accelerations.rear_left == pytest.approx(3.0, abs=EPSILON)
    assert wheel_accelerations.rear_right == pytest.approx(4.0, abs=EPSILON)


def test_plus():
    left = MecanumDriveWheelAccelerations(1.0, 0.5, 0.75, 0.25)
    right = MecanumDriveWheelAccelerations(1.0, 1.5, 0.25, 1.75)

    wheel_accelerations = left + right

    assert wheel_accelerations.front_left == pytest.approx(2.0, abs=EPSILON)
    assert wheel_accelerations.front_right == pytest.approx(2.0, abs=EPSILON)
    assert wheel_accelerations.rear_left == pytest.approx(1.0, abs=EPSILON)
    assert wheel_accelerations.rear_right == pytest.approx(2.0, abs=EPSILON)


def test_minus():
    left = MecanumDriveWheelAccelerations(1.0, 0.5, 0.75, 0.25)
    right = MecanumDriveWheelAccelerations(2.0, 1.5, 0.25, 0.75)

    wheel_accelerations = left - right

    assert wheel_accelerations.front_left == pytest.approx(-1.0, abs=EPSILON)
    assert wheel_accelerations.front_right == pytest.approx(-1.0, abs=EPSILON)
    assert wheel_accelerations.rear_left == pytest.approx(0.5, abs=EPSILON)
    assert wheel_accelerations.rear_right == pytest.approx(-0.5, abs=EPSILON)


def test_unary_minus():
    accel = MecanumDriveWheelAccelerations(1.0, 0.5, 0.75, 0.25)

    wheel_accelerations = -accel

    assert wheel_accelerations.front_left == pytest.approx(-1.0, abs=EPSILON)
    assert wheel_accelerations.front_right == pytest.approx(-0.5, abs=EPSILON)
    assert wheel_accelerations.rear_left == pytest.approx(-0.75, abs=EPSILON)
    assert wheel_accelerations.rear_right == pytest.approx(-0.25, abs=EPSILON)


def test_multiplication():
    accel = MecanumDriveWheelAccelerations(1.0, 0.5, 0.75, 0.25)

    wheel_accelerations = accel * 2.0

    assert wheel_accelerations.front_left == pytest.approx(2.0, abs=EPSILON)
    assert wheel_accelerations.front_right == pytest.approx(1.0, abs=EPSILON)
    assert wheel_accelerations.rear_left == pytest.approx(1.5, abs=EPSILON)
    assert wheel_accelerations.rear_right == pytest.approx(0.5, abs=EPSILON)


def test_division():
    accel = MecanumDriveWheelAccelerations(2.0, 1.0, 1.5, 0.5)

    wheel_accelerations = accel / 2.0

    assert wheel_accelerations.front_left == pytest.approx(1.0, abs=EPSILON)
    assert wheel_accelerations.front_right == pytest.approx(0.5, abs=EPSILON)
    assert wheel_accelerations.rear_left == pytest.approx(0.75, abs=EPSILON)
    assert wheel_accelerations.rear_right == pytest.approx(0.25, abs=EPSILON)


def test_feet_constructor():
    accel = MecanumDriveWheelAccelerations.from_fps(10, 11, 12, 13)

    assert accel.front_left == pytest.approx(feet_to_meters(10), abs=EPSILON)
    assert accel.front_right == pytest.approx(feet_to_meters(11), abs=EPSILON)
    assert accel.rear_left == pytest.approx(feet_to_meters(12), abs=EPSILON)
    assert accel.rear_right == pytest.approx(feet_to_meters(13), abs=EPSILON)

    assert accel.front_left_fpss == pytest.approx(10, abs=EPSILON)
    assert accel.front_right_fpss == pytest.approx(11, abs=EPSILON)
    assert accel.rear_left_fpss == pytest.approx(12, abs=EPSILON)
    assert accel.rear_right_fpss == pytest.approx(13, abs=EPSILON)


def test_repr():
    accel = MecanumDriveWheelAccelerations(1, 2, 3, 4)

    assert (
        str(accel)
        == "MecanumDriveWheelAccelerations(front_left=1.000000, front_right=2.000000, rear_left=3.000000, rear_right=4.000000)"
    )
