from . import _init__telemetry

from abc import ABC, abstractmethod

from ._telemetry import (
    DiscardTelemetryBackend,
    MockTelemetryBackend,
    Telemetry,
    TelemetryBackend,
    TelemetryEntry,
    TelemetryRegistry,
    TelemetryTable,
)


class TelemetryLoggable(ABC):
    """An object that can log its state to a telemetry table."""

    @abstractmethod
    def log_to(self, table: TelemetryTable) -> None:
        """Logs this object's current state to ``table``."""

    def get_telemetry_type(self) -> str:
        """Returns this object's stable telemetry type, or an empty string."""
        return ""

__all__ = [
    "DiscardTelemetryBackend",
    "MockTelemetryBackend",
    "Telemetry",
    "TelemetryBackend",
    "TelemetryEntry",
    "TelemetryLoggable",
    "TelemetryRegistry",
    "TelemetryTable",
]

try:
    from .version import version as __version__
except ImportError:
    __version__ = "master"

del _init__telemetry
