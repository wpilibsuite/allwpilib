from typing import Callable, Protocol, TypeVar, Union

from typing_extensions import TypeAlias
from wpimath.controller import ProfiledPIDController, ProfiledPIDControllerRadians
from wpimath.trajectory import TrapezoidProfile, TrapezoidProfileRadians

# Generic Types
TProfiledPIDController = TypeVar(
    "TProfiledPIDController", ProfiledPIDControllerRadians, ProfiledPIDController
)
TTrapezoidProfileState = TypeVar(
    "TTrapezoidProfileState",
    TrapezoidProfileRadians.State,
    TrapezoidProfile.State,
)


# Protocols - Structural Typing
class UseOutputFunction(Protocol):

    def __init__(self, *args, **kwargs) -> None: ...

    def __call__(self, t: float, u: TTrapezoidProfileState) -> None: ...

    def accept(self, t: float, u: TTrapezoidProfileState) -> None: ...


# Type Aliases
FloatSupplier: TypeAlias = Callable[[], float]
FloatOrFloatSupplier: TypeAlias = Union[float, Callable[[], float]]
