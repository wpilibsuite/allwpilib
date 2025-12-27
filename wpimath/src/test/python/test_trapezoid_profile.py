import pytest

import wpimath

trapezoid_profile_types = [
    wpimath.TrapezoidProfile,
    wpimath.TrapezoidProfileRadians,
]


@pytest.mark.parametrize("TrapezoidProfile", trapezoid_profile_types)
def test_constraints_repr(TrapezoidProfile):
    expected_qualname = f"{TrapezoidProfile.__name__}.Constraints"
    constraints = TrapezoidProfile.Constraints()

    assert repr(constraints).startswith(f"{expected_qualname}(maxVelocity=0.")


@pytest.mark.parametrize("TrapezoidProfile", trapezoid_profile_types)
def test_state_repr(TrapezoidProfile):
    expected_qualname = f"{TrapezoidProfile.__name__}.State"
    constraints = TrapezoidProfile.State()

    assert repr(constraints).startswith(f"{expected_qualname}(position=0.")
