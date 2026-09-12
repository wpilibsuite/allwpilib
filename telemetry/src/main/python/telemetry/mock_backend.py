"""Value types returned by :class:`telemetry.MockTelemetryBackend`."""

from dataclasses import dataclass, field
from typing import TypeAlias


@dataclass
class KeepDuplicatesValue:
    value: bool = True


@dataclass
class SetPropertyValue:
    key: str = ""
    value: str = ""


@dataclass
class LogStringValue:
    value: str = ""
    type_string: str = ""


@dataclass
class LogBooleanArrayValue:
    value: list[bool] = field(default_factory=list)


@dataclass
class LogRawValue:
    value: bytes = b""
    type_string: str = ""


ActionValue: TypeAlias = (
    KeepDuplicatesValue
    | SetPropertyValue
    | bool
    | int
    | float
    | LogStringValue
    | LogBooleanArrayValue
    | list[int]
    | list[float]
    | list[str]
    | LogRawValue
)


@dataclass
class Action:
    path: str
    value: ActionValue
    timestamp: int = 0
