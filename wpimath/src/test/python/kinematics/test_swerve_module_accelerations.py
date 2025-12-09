import pytest
import math
from wpimath.kinematics import SwerveModuleAcceleration
from wpimath.geometry import Rotation2d

K_EPSILON = 1e-9

def test_default_constructor():
    module_accelerations = SwerveModuleAcceleration()

    assert module_accelerations.acceleration == pytest.approx(0.0, abs=K_EPSILON)
    assert module_accelerations.angle.radians() == pytest.approx(0.0, abs=K_EPSILON)

def test_parameterized_constructor():
    module_accelerations = SwerveModuleAcceleration(2.5, Rotation2d(1.5))

    assert module_accelerations.acceleration == pytest.approx(2.5, abs=K_EPSILON)
    assert module_accelerations.angle.radians() == pytest.approx(1.5, abs=K_EPSILON)

def test_equals():
    module_accelerations1 = SwerveModuleAcceleration(2.0, Rotation2d(1.5))
    module_accelerations2 = SwerveModuleAcceleration(2.0, Rotation2d(1.5))
    
    module_accelerations3 = SwerveModuleAcceleration(2.1, Rotation2d(1.5))

    assert module_accelerations1 == module_accelerations2
    assert module_accelerations1 != module_accelerations3