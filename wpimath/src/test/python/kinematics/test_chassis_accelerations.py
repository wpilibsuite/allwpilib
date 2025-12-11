import math
import pytest
from wpimath.kinematics import ChassisAccelerations
from wpimath.geometry import Rotation2d
from wpimath.units import feetToMeters

K_EPSILON = 1e-9


def test_default_constructor():
    accelerations = ChassisAccelerations()

    assert accelerations.ax == pytest.approx(0.0, abs=K_EPSILON)
    assert accelerations.ay == pytest.approx(0.0, abs=K_EPSILON)
    assert accelerations.alpha == pytest.approx(0.0, abs=K_EPSILON)


def test_parameterized_constructor():
    # 1.0 m/s^2, 2.0 m/s^2, 3.0 rad/s^2
    accelerations = ChassisAccelerations(1.0, 2.0, 3.0)

    assert accelerations.ax == pytest.approx(1.0, abs=K_EPSILON)
    assert accelerations.ay == pytest.approx(2.0, abs=K_EPSILON)
    assert accelerations.alpha == pytest.approx(3.0, abs=K_EPSILON)


def test_to_robot_relative():
    start_accel = ChassisAccelerations(1.0, 0.0, 0.5)

    chassis_accelerations = start_accel.toRobotRelative(Rotation2d.fromDegrees(-90))

    assert chassis_accelerations.ax == pytest.approx(0.0, abs=K_EPSILON)
    assert chassis_accelerations.ay == pytest.approx(1.0, abs=K_EPSILON)
    assert chassis_accelerations.alpha == pytest.approx(0.5, abs=K_EPSILON)


def test_to_field_relative():
    start_accel = ChassisAccelerations(1.0, 0.0, 0.5)

    chassis_accelerations = start_accel.toFieldRelative(Rotation2d.fromDegrees(45))

    expected_val = 1.0 / math.sqrt(2.0)

    assert chassis_accelerations.ax == pytest.approx(expected_val, abs=K_EPSILON)
    assert chassis_accelerations.ay == pytest.approx(expected_val, abs=K_EPSILON)
    assert chassis_accelerations.alpha == pytest.approx(0.5, abs=K_EPSILON)


def test_plus():
    left = ChassisAccelerations(1.0, 0.5, 0.75)
    right = ChassisAccelerations(2.0, 1.5, 0.25)

    chassis_accelerations = left + right

    assert chassis_accelerations.ax == pytest.approx(3.0, abs=K_EPSILON)
    assert chassis_accelerations.ay == pytest.approx(2.0, abs=K_EPSILON)
    assert chassis_accelerations.alpha == pytest.approx(1.0, abs=K_EPSILON)


def test_minus():
    left = ChassisAccelerations(1.0, 0.5, 0.75)
    right = ChassisAccelerations(2.0, 0.5, 0.25)

    chassis_accelerations = left - right

    assert chassis_accelerations.ax == pytest.approx(-1.0, abs=K_EPSILON)
    assert chassis_accelerations.ay == pytest.approx(0.0, abs=K_EPSILON)
    assert chassis_accelerations.alpha == pytest.approx(0.5, abs=K_EPSILON)


def test_unary_minus():
    accel = ChassisAccelerations(1.0, 0.5, 0.75)

    chassis_accelerations = -accel

    assert chassis_accelerations.ax == pytest.approx(-1.0, abs=K_EPSILON)
    assert chassis_accelerations.ay == pytest.approx(-0.5, abs=K_EPSILON)
    assert chassis_accelerations.alpha == pytest.approx(-0.75, abs=K_EPSILON)


def test_multiplication():
    accel = ChassisAccelerations(1.0, 0.5, 0.75)

    chassis_accelerations = accel * 2.0

    assert chassis_accelerations.ax == pytest.approx(2.0, abs=K_EPSILON)
    assert chassis_accelerations.ay == pytest.approx(1.0, abs=K_EPSILON)
    assert chassis_accelerations.alpha == pytest.approx(1.5, abs=K_EPSILON)


def test_division():
    accel = ChassisAccelerations(2.0, 1.0, 1.5)

    chassis_accelerations = accel / 2.0

    assert chassis_accelerations.ax == pytest.approx(1.0, abs=K_EPSILON)
    assert chassis_accelerations.ay == pytest.approx(0.5, abs=K_EPSILON)
    assert chassis_accelerations.alpha == pytest.approx(0.75, abs=K_EPSILON)


def test_feet_constructor():
    chassis_accelerations = ChassisAccelerations.fromFpsSq(10, 11, math.radians(45))

    assert chassis_accelerations.ax == pytest.approx(feetToMeters(10), abs=K_EPSILON)
    assert chassis_accelerations.ay == pytest.approx(feetToMeters(11), abs=K_EPSILON)
    assert chassis_accelerations.alpha == pytest.approx(math.radians(45), abs=K_EPSILON)

    assert chassis_accelerations.ax_fpss == pytest.approx(10, abs=K_EPSILON)
    assert chassis_accelerations.ay_fpss == pytest.approx(11, abs=K_EPSILON)
    assert chassis_accelerations.alpha_dpss == pytest.approx(45, abs=K_EPSILON)


def test_repr():
    chassis_accelerations = ChassisAccelerations(1, 2, 3)

    assert (
        str(chassis_accelerations)
        == "ChassisAccelerations(ax=1.000000, ay=2.000000, alpha=3.000000)"
    )
