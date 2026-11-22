"""Value types returned by :class:`telemetry.MockTelemetryBackend`."""

from dataclasses import dataclass, field


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


type ActionValue = (
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
