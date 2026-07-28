import pytest
import math
from wpimath import Rotation2d, SwerveModuleAcceleration
from wpimath.units import feet_to_meters

EPSILON = 1e-9


def test_default_constructor():
    module_accelerations = SwerveModuleAcceleration()

    assert module_accelerations.acceleration == pytest.approx(0.0, abs=EPSILON)
    assert module_accelerations.angle.radians() == pytest.approx(0.0, abs=EPSILON)


def test_parameterized_constructor():
    module_accelerations = SwerveModuleAcceleration(2.5, Rotation2d(1.5))

    assert module_accelerations.acceleration == pytest.approx(2.5, abs=EPSILON)
    assert module_accelerations.angle.radians() == pytest.approx(1.5, abs=EPSILON)


def test_equals():
    module_accelerations1 = SwerveModuleAcceleration(2.0, Rotation2d(1.5))
    module_accelerations2 = SwerveModuleAcceleration(2.0, Rotation2d(1.5))

    module_accelerations3 = SwerveModuleAcceleration(2.1, Rotation2d(1.5))

    assert module_accelerations1 == module_accelerations2
    assert module_accelerations1 != module_accelerations3


def test_feet_constructor():
    accel = SwerveModuleAcceleration.from_fps(10, Rotation2d(1.5))

    assert accel.acceleration == pytest.approx(feet_to_meters(10), abs=EPSILON)
    assert accel.angle.radians() == pytest.approx(1.5, abs=EPSILON)

    assert accel.acceleration_fpss == pytest.approx(10, abs=EPSILON)


def test_repr():
    accel = SwerveModuleAcceleration(1, Rotation2d(1.5))

    assert (
        str(accel) == "SwerveModuleAcceleration(acceleration=1.000000, angle=1.500000)"
    )
