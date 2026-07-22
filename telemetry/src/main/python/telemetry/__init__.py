from . import _init__telemetry

from ._telemetry import TelemetryBackend, TelemetryEntry

__all__ = [
    "TelemetryBackend",
    "TelemetryEntry",
]

try:
    from .version import version as __version__
except ImportError:
    __version__ = "master"

del _init__telemetry
