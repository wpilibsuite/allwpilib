# Telemetry Python API

This file describes the Python telemetry API exposed by the `telemetry` package. It follows the same table/entry/backend model as Java and C++, but uses snake_case method names and Python runtime type dispatch.

- For the shared Telemetry design and Java-facing API, see [Telemetry design](telemetry.md).
- For C++-specific behavior, see [C++ API](cpp.md).

## Python Overview

The primary entry points are:

- `telemetry.Telemetry` for root-level logging
- `telemetry.TelemetryTable` for grouped logging under a normalized path
- `telemetry.TelemetryLoggable` or any object with a `log_to(table)` method for table-valued objects
- `telemetry.TelemetryRegistry` for backend registration, warning hooks, and test setup

```py
import telemetry

telemetry.Telemetry.log("batteryVoltage", 12.4)
telemetry.Telemetry.log("enabled", True)

drive = telemetry.Telemetry.get_table("drive")
drive.log("leftVelocity", 3.2)
drive.log("rightVelocity", 3.1)
```

`Telemetry.get_table()` returns the root table, and `Telemetry.get_table("drive")` returns a `TelemetryTable` with path `/drive/`. `TelemetryTable.path` exposes that normalized path. Tables also expose `get_table()`, `set_type()`, `get_type()`, `has_type()`, `keep_duplicates()`, `set_property()`, and `log()`.

## Python Value Dispatch

`Telemetry.log(name, value, *, element_type=None, type_string="")` and `TelemetryTable.log(name, value, *, element_type=None, type_string="")` support:

- `bool`, `int`, `float`, and `str` scalars
- `bytes`, `bytearray`, and `memoryview` as raw bytes
- scalar `str` and bytes-like values with a custom backend type string passed as `type_string`
- objects with `log_to(table)`, optionally paired with `get_telemetry_type()`
- objects whose class has `WPIStruct`, created with `wpiutil.wpistruct`
- sequences, when `element_type` is explicitly provided

For sequences, `element_type` must be a Python type object:

- `bool` logs a boolean array and requires all elements to be `bool`
- `int` logs an integer array and requires non-boolean ints
- `float` logs a double array and accepts ints or floats
- `str` logs a string array and requires strings
- `object` logs a string array using `str()` for each element
- a WPIStruct class logs a struct array, registers its schema, and emits raw bytes with the `struct:<name>[]` type string

String `element_type` arguments are rejected. Use Python type objects for sequence element selection and `type_string` only for scalar string/raw metadata.

```py
import dataclasses

import telemetry
from wpiutil import wpistruct

@wpistruct.make_wpistruct(name="TelemetryPoint")
@dataclasses.dataclass
class TelemetryPoint:
    x: wpistruct.double
    y: wpistruct.int32

telemetry.Telemetry.log("rawFrame", bytearray(b"abc"), type_string="image/custom")
telemetry.Telemetry.log("wheelSpeeds", [1.0, 2.5], element_type=float)
telemetry.Telemetry.log("point", TelemetryPoint(1.5, 2))
telemetry.Telemetry.log("points", [TelemetryPoint(1.0, 2)], element_type=TelemetryPoint)
```

If an unsupported scalar object does not provide `log_to()`, the Python frontend logs `str(value)` as a string when the selected entry is not discarded.

## Python Complex Objects

`telemetry.TelemetryLoggable` is a small Python ABC, but the dispatcher also accepts any object with a `log_to(table)` method. `get_telemetry_type()` may return a non-empty string to set the child table type.

```py
class DriveSnapshot(telemetry.TelemetryLoggable):
    def __init__(self, left: float, right: float) -> None:
        self.left = left
        self.right = right

    def get_telemetry_type(self) -> str:
        return "DriveSnapshot"

    def log_to(self, table: telemetry.TelemetryTable) -> None:
        table.log("leftMetersPerSecond", self.left)
        table.log("rightMetersPerSecond", self.right)

telemetry.Telemetry.log("drive", DriveSnapshot(3.2, 3.1))
```

As in C++ and Java, table-valued objects are skipped when their target entry is discarded unless a more specific descendant backend can receive values under the child table.

## Python Backend and Test APIs

Python exposes `TelemetryRegistry.set_report_warning(func_or_none)`, `report_warning(path, msg)`, `register_backend(prefix, backend)`, `get_backend(path)`, `get_entry(path)`, `get_table(path)`, and `reset()`. Paths are normalized before backend lookup. Custom warning callbacks must not raise.

Available built-in backends are:

- `telemetry.MockTelemetryBackend`, with `get_actions()`, `get_last_action(path)`, `get_last_value(path)`, `get_schema(schema_name)`, and `clear()`
- `telemetry.DiscardTelemetryBackend`, which returns discard entries
- `telemetry.MultiTelemetryBackend`, which fans logging and schema calls out to multiple child backends

```py
backend = telemetry.MockTelemetryBackend()
telemetry.TelemetryRegistry.reset()
telemetry.TelemetryRegistry.register_backend("", backend)

telemetry.Telemetry.log("enabled", True)
assert backend.get_last_value("/enabled") is True
```
