from types import MethodType
from typing import Any

import pytest
from wpimath.controller import ProfiledPIDController, ProfiledPIDControllerRadians
from wpimath.trajectory import TrapezoidProfile, TrapezoidProfileRadians

from commands2 import ProfiledPIDSubsystem

MAX_VELOCITY = 30  # Radians per second
MAX_ACCELERATION = 500  # Radians per sec squared
PID_KP = 50


class EvalSubsystem(ProfiledPIDSubsystem):
    def __init__(self, controller, state_factory):
        self._state_factory = state_factory
        super().__init__(controller, 0)


def simple_use_output(self, output: float, setpoint: Any):
    """A simple useOutput method that saves the current state of the controller."""
    self._output = output
    self._setpoint = setpoint


def simple_get_measurement(self) -> float:
    """A simple getMeasurement method that returns zero (frozen or stuck plant)."""
    return 0.0


controller_types = [
    (
        ProfiledPIDControllerRadians,
        TrapezoidProfileRadians.Constraints,
        TrapezoidProfileRadians.State,
    ),
    (ProfiledPIDController, TrapezoidProfile.Constraints, TrapezoidProfile.State),
]
controller_ids = ["radians", "dimensionless"]


@pytest.fixture(params=controller_types, ids=controller_ids)
def subsystem(request):
    """
    Fixture that returns an EvalSubsystem object for each type of controller.
    """
    controller, profile_factory, state_factory = request.param
    profile = profile_factory(MAX_VELOCITY, MAX_ACCELERATION)
    pid = controller(PID_KP, 0, 0, profile)
    return EvalSubsystem(pid, state_factory)


def test_profiled_pid_subsystem_init(subsystem):
    """
    Verify that the ProfiledPIDSubsystem can be initialized using
    all supported profiled PID controller / trapezoid profile types.
    """
    assert isinstance(subsystem, EvalSubsystem)


def test_profiled_pid_subsystem_not_implemented_get_measurement(subsystem):
    """
    Verify that the ProfiledPIDSubsystem.getMeasurement method
    raises NotImplementedError.
    """
    with pytest.raises(NotImplementedError):
        subsystem.getMeasurement()


def test_profiled_pid_subsystem_not_implemented_use_output(subsystem):
    """
    Verify that the ProfiledPIDSubsystem.useOutput method raises
    NotImplementedError.
    """
    with pytest.raises(NotImplementedError):
        subsystem.useOutput(0, subsystem._state_factory())


@pytest.mark.parametrize("use_float", [True, False])
def test_profiled_pid_subsystem_set_goal(subsystem, use_float):
    """
    Verify that the ProfiledPIDSubsystem.setGoal method sets the goal.
    """
    if use_float:
        subsystem.setGoal(1.0)
        assert subsystem.getController().getGoal().position == 1.0
        assert subsystem.getController().getGoal().velocity == 0.0
    else:
        subsystem.setGoal(subsystem._state_factory(1.0, 2.0))
        assert subsystem.getController().getGoal().position == 1.0
        assert subsystem.getController().getGoal().velocity == 2.0


def test_profiled_pid_subsystem_enable_subsystem(subsystem):
    """
    Verify the subsystem can be enabled.
    """
    # Dynamically add useOutput and getMeasurement methods so the
    # system can be enabled
    setattr(subsystem, "useOutput", MethodType(simple_use_output, subsystem))
    setattr(subsystem, "getMeasurement", MethodType(simple_get_measurement, subsystem))
    # Enable the subsystem
    subsystem.enable()
    assert subsystem.isEnabled()


def test_profiled_pid_subsystem_disable_subsystem(subsystem):
    """
    Verify the subsystem can be disabled.
    """
    # Dynamically add useOutput and getMeasurement methods so the
    # system can be enabled
    setattr(subsystem, "useOutput", MethodType(simple_use_output, subsystem))
    setattr(subsystem, "getMeasurement", MethodType(simple_get_measurement, subsystem))
    # Enable and then disable the subsystem
    subsystem.enable()
    subsystem.disable()
    assert not subsystem.isEnabled()
