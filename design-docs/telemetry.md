# Summary

This document describes the Telemetry API for publishing robot-program data to dashboards, debug tools, or log files.

The API is centered around four main concepts:

- `Telemetry`, a static convenience facade for simple logging

- `TelemetryTable`, a hierarchical namespace for related telemetry values

- `TelemetryLoggable`, an interface for complex objects that publish themselves into a table

- `TelemetryEntry`, the backend-facing sink for individual values

This document focuses on how teams use the API from robot code, and how backend implementers consume the lower-level interfaces.

# Motivation

Robot programs need to expose data for a variety of reasons:

- Observability during debugging (sensor values, controller outputs, state estimates)

- Runtime diagnostics (fault flags, communication health, current draw, timing)

- Logging of structured values for later analysis

- Dashboard display of complex robot objects such as poses, mechanisms, or subsystem state

WPILib already provides lower-level mechanisms for publishing data, especially NetworkTables and DataLog. Those APIs are flexible, but robot code often ends up repeating the same tasks:

- Building and reusing hierarchical paths

- Choosing serialization formats for complex objects

- Grouping related values under a common namespace

- Adapting custom objects into a flat set of primitive or structured values

Telemetry provides a simpler surface for these common tasks while still allowing advanced users to drop down to lower-level APIs when necessary.

# Background

The Telemetry API is transport-agnostic. Robot code logs to `Telemetry` or `TelemetryTable`, and `TelemetryRegistry` routes those calls to a registered `TelemetryBackend` based on path prefix.

This split is intentional:

- Robot code should describe *what* is being published, not *how* it is transported

- Different backends may target dashboards, log files, tests, or future tooling

- Unit tests should be able to observe telemetry output without requiring a live dashboard connection

The primary user-facing API is table-oriented. A table represents a path such as `/Drive/` or `/PoseEstimator/`, and entries within that table represent named values such as `leftVelocity` or `estimatedPose`.

Complex objects integrate by implementing `TelemetryLoggable`, which receives a table and populates it with its internal state.

At the lowest layer, a backend exposes `TelemetryEntry` objects. These receive the already-resolved per-value logging calls (`logBoolean`, `logStruct`, `logRaw`, and so on).

# Requirements and Desirable Features

- Common telemetry use cases should be expressible with a one-line static call.

- Related values should be easy to organize hierarchically.

- Primitive types should have direct logging paths.

- Struct- and Protobuf-serializable objects should be supported.

- Complex objects should be able to publish themselves without forcing callers to manually flatten them.

- The backend should be pluggable so telemetry can target different transports or test doubles.

- Type mismatches and unsupported serialization situations should produce warnings rather than silent corruption.

- The API should make the common robot-code path simple while keeping the backend-facing path explicit.

# Design

## Overview / Key Features

- `Telemetry` is a static facade over the root telemetry table (`/`).

- `TelemetryTable` represents a path in a hierarchical namespace and caches child tables and entries.

- `TelemetryLoggable` is for complex objects that want to publish multiple related values into a subtable.

- `TelemetryEntry` is implemented by backends and receives type-specific logging calls.

- Automatic handling exists for `StructSerializable` and `ProtobufSerializable` values when the corresponding static `struct` or `proto` field is available.

- Arbitrary object types can be supported through `TelemetryRegistry.registerTypeHandler()`.

- Table types are supported via `.type` entries to help dashboards interpret complex tables.

- Duplicate suppression is the default behavior; callers can opt in to preserving duplicates on a per-entry basis.

## Core APIs

### Telemetry

`Telemetry` is the primary entry point for simple use cases. It operates on the root table and forwards all work to `TelemetryTable`.

```java
public final class Telemetry {
  public static TelemetryTable getTable() {...}
  public static TelemetryTable getTable(String name) {...}

  public static void keepDuplicates(String name) {...}
  public static void setProperty(String name, String key, String value) {...}

  public static void log(String name, Object value) {...}
  public static <T> void log(String name, T value, Struct<T> struct) {...}
  public static <T> void log(String name, T value, Protobuf<T, ?> proto) {...}
  public static void log(String name, Object[] value) {...}

  public static void log(String name, boolean value) {...}
  public static void log(String name, byte value) {...}
  public static void log(String name, short value) {...}
  public static void log(String name, int value) {...}
  public static void log(String name, long value) {...}
  public static void log(String name, float value) {...}
  public static void log(String name, double value) {...}
  public static void log(String name, String value) {...}
  public static void log(String name, String value, String typeString) {...}

  public static void log(String name, boolean[] value) {...}
  public static void log(String name, short[] value) {...}
  public static void log(String name, int[] value) {...}
  public static void log(String name, long[] value) {...}
  public static void log(String name, float[] value) {...}
  public static void log(String name, double[] value) {...}
  public static void log(String name, String[] value) {...}
  public static void log(String name, byte[] value) {...}
  public static void log(String name, byte[] value, String typeString) {...}
}
```

Key behavior:

- `Telemetry.getTable()` returns the root table.

- `Telemetry.getTable("Drive")` returns `/Drive/`.

- `keepDuplicates()` changes the behavior for a single named entry so repeated identical values are retained by the backend instead of being coalesced.

- `setProperty()` attaches backend-specific metadata to a named entry. Property values are JSON strings.

- `log(String, Object)` performs runtime dispatch:
  - `TelemetryLoggable` values are logged into a child table
  - `StructSerializable` values use their static `struct` field
  - `ProtobufSerializable` values use their static `proto` field
  - boxed primitives and strings route to the corresponding primitive overloads
  - registered type handlers are consulted for other object types
  - values without a handler fall back to `toString()`

### Telemetry Usage Examples

Simple primitive logging:

```java
Telemetry.log("batteryVoltage", RobotController.getBatteryVoltage());
Telemetry.log("enabled", DriverStation.isEnabled());
Telemetry.log("matchTime", Timer.getMatchTime());
```

Logging grouped values through subtables:

```java
TelemetryTable drive = Telemetry.getTable("Drive");

drive.log("leftVelocity", leftEncoder.getRate());
drive.log("rightVelocity", rightEncoder.getRate());
drive.log("currentLimitEnabled", currentLimitEnabled);
```

Configuring entry behavior:

```java
Telemetry.keepDuplicates("loopOverrunCount");
Telemetry.setProperty("loopOverrunCount", "unit", "\"count\"");
Telemetry.log("loopOverrunCount", loopOverrunCount);
```

Logging a structured value explicitly:

```java
Telemetry.log("targetPose", pose, Pose2d.struct);
Telemetry.log("detectedTag", tagDetection, AprilTagDetection.proto);
```

Logging raw bytes with a custom type string:

```java
Telemetry.log("cameraFrame", compressedFrameBytes, "image/jpeg");
```

### TelemetryTable

`TelemetryTable` is the core abstraction behind the static facade. It represents a single path and provides hierarchical logging operations.

```java
public final class TelemetryTable {
  public String getPath() {...}

  public boolean setType(String typeString) {...}
  public String getType() {...}

  public TelemetryTable getTable(String name) {...}

  public void keepDuplicates(String name) {...}
  public void setProperty(String name, String key, String value) {...}

  public <T> void log(String name, T value) {...}
  public <T> void log(String name, T value, Struct<T> struct) {...}
  public <T> void log(String name, T value, Protobuf<T, ?> proto) {...}
  public <T> void log(String name, T[] value) {...}
  public <T> void log(String name, T[] value, Struct<T> struct) {...}

  public void log(String name, boolean value) {...}
  public void log(String name, byte value) {...}
  public void log(String name, short value) {...}
  public void log(String name, int value) {...}
  public void log(String name, long value) {...}
  public void log(String name, float value) {...}
  public void log(String name, double value) {...}
  public void log(String name, String value) {...}
  public void log(String name, String value, String typeString) {...}

  public void log(String name, boolean[] value) {...}
  public void log(String name, short[] value) {...}
  public void log(String name, int[] value) {...}
  public void log(String name, long[] value) {...}
  public void log(String name, float[] value) {...}
  public void log(String name, double[] value) {...}
  public void log(String name, String[] value) {...}
  public void log(String name, byte[] value) {...}
  public void log(String name, byte[] value, String typeString) {...}
}
```

Key behavior:

- Tables are hierarchical. `getTable("Drive").getTable("Left")` corresponds to `/Drive/Left/`.

- Tables are cached by path, so repeated calls for the same path return the same logical table object.

- `setType()` establishes a stable type string for the table. A later attempt to set a different type results in a warning and returns `false`.

- When logging a `TelemetryLoggable`, the object is logged to a child table named by the caller.

- For object arrays, `StructSerializable[]` can use automatic struct-based logging. Other object arrays currently fall back to string arrays.

### TelemetryTable Usage Examples

Subsystem-local publishing:

```java
public final class Shooter {
  private final TelemetryTable telemetry = Telemetry.getTable("Shooter");

  public void periodic() {
    telemetry.log("wheelSpeedRps", m_encoder.getRate());
    telemetry.log("commandedVoltage", m_lastVoltage);
    telemetry.log("ready", atSpeed());
  }
}
```

Using nested tables for organization:

```java
TelemetryTable estimator = Telemetry.getTable("PoseEstimator");
TelemetryTable vision = estimator.getTable("Vision");
TelemetryTable odometry = estimator.getTable("Odometry");

vision.log("tagCount", visibleTags.size());
odometry.log("pose", currentPose, Pose2d.struct);
```

Using a stable table type:

```java
TelemetryTable mechanism = Telemetry.getTable("ArmMechanism");
if (mechanism.setType("Mechanism2d")) {
  mechanism.log("layout", serializedMechanismJson, "json");
}
```

### TelemetryLoggable

`TelemetryLoggable` is the interface for complex objects that want to publish themselves into a `TelemetryTable`.

```java
@FunctionalInterface
public interface TelemetryLoggable {
  void logTo(TelemetryTable table);

  default String getTelemetryType() {
    return null;
  }
}
```

Key behavior:

- The caller logs the object once by name; the implementation decides what sub-entries appear inside that child table.

- `getTelemetryType()` is optional. If non-null, the target table records that type via `setType()`.

- A type mismatch is reported as a warning and the log operation is skipped, which prevents a single path from silently changing schema at runtime.

- This interface is the preferred way to publish a complex object made up of multiple values rather than flattening it manually at each call site.

### TelemetryLoggable Usage Examples

Publishing a subsystem snapshot:

```java
public final class DriveSnapshot implements TelemetryLoggable {
  private final DifferentialDriveWheelSpeeds m_wheelSpeeds;
  private final Pose2d m_pose;
  private final boolean m_closedLoop;

  public DriveSnapshot(
      DifferentialDriveWheelSpeeds wheelSpeeds, Pose2d pose, boolean closedLoop) {
    m_wheelSpeeds = wheelSpeeds;
    m_pose = pose;
    m_closedLoop = closedLoop;
  }

  @Override
  public String getTelemetryType() {
    return "DriveSnapshot";
  }

  @Override
  public void logTo(TelemetryTable table) {
    table.log("leftMetersPerSecond", m_wheelSpeeds.leftMetersPerSecond);
    table.log("rightMetersPerSecond", m_wheelSpeeds.rightMetersPerSecond);
    table.log("pose", m_pose, Pose2d.struct);
    table.log("closedLoop", m_closedLoop);
  }
}
```

Logging it from robot code:

```java
Telemetry.log(
    "drive",
    new DriveSnapshot(m_drive.getWheelSpeeds(), m_drive.getPose(), m_drive.isClosedLoop()));
```

That call creates or reuses the `/drive/` table and populates entries under it.

Using `TelemetryRegistry.registerTypeHandler()` for a third-party type:

```java
TelemetryRegistry.registerTypeHandler(
    ChassisSpeeds.class,
    (table, name, speeds) -> {
      TelemetryTable subtable = table.getTable(name);
      subtable.setType("ChassisSpeeds");
      subtable.log("vxMetersPerSecond", speeds.vxMetersPerSecond);
      subtable.log("vyMetersPerSecond", speeds.vyMetersPerSecond);
      subtable.log("omegaRadiansPerSecond", speeds.omegaRadiansPerSecond);
    });
```

After registration, callers can write:

```java
Telemetry.log("commandedSpeeds", chassisSpeeds);
```

### TelemetryEntry

`TelemetryEntry` is the low-level per-path sink used by telemetry backends. Most robot code should not interact with this interface directly.

```java
public interface TelemetryEntry {
  void keepDuplicates();
  void setProperty(String key, String value);

  <T> void logStruct(T value, Struct<T> struct);
  <T> void logProtobuf(T value, Protobuf<T, ?> proto);
  <T> void logStructArray(T[] value, Struct<T> struct);

  void logBoolean(boolean value);
  default void logByte(byte value) {...}
  default void logShort(short value) {...}
  default void logInt(int value) {...}
  void logLong(long value);
  void logFloat(float value);
  void logDouble(double value);
  void logString(String value, String typeString);

  void logBooleanArray(boolean[] value);
  void logShortArray(short[] value);
  void logIntArray(int[] value);
  void logLongArray(long[] value);
  void logFloatArray(float[] value);
  void logDoubleArray(double[] value);
  void logStringArray(String[] value);
  void logRaw(byte[] value, String typeString);
}
```

Key behavior:

- A `TelemetryBackend` creates entries by full path.

- `TelemetryTable` resolves names to entries lazily and caches them.

- `logByte`, `logShort`, and `logInt` default to widening into `logLong`, so a backend can implement integer handling with a single required path.

- Backends are responsible for storing, transporting, deduplicating, or timestamping values as appropriate for that transport.

- Metadata (`setProperty`) and duplicate-preservation (`keepDuplicates`) are handled at the entry level rather than the table level.

### TelemetryEntry Usage Example

Minimal backend skeleton:

```java
public final class ConsoleTelemetryBackend implements TelemetryBackend {
  @Override
  public TelemetryEntry getEntry(String path) {
    return new TelemetryEntry() {
      @Override
      public void keepDuplicates() {}

      @Override
      public void setProperty(String key, String value) {}

      @Override
      public <T> void logStruct(T value, Struct<T> struct) {
        System.out.println(path + " = " + value + " [struct=" + struct.getTypeName() + "]");
      }

      @Override
      public <T> void logProtobuf(T value, Protobuf<T, ?> proto) {
        System.out.println(path + " = " + value + " [proto=" + proto.getTypeString() + "]");
      }

      @Override
      public <T> void logStructArray(T[] value, Struct<T> struct) {
        System.out.println(path + " = struct array length " + value.length);
      }

      @Override
      public void logBoolean(boolean value) {
        System.out.println(path + " = " + value);
      }

      @Override
      public void logLong(long value) {
        System.out.println(path + " = " + value);
      }

      @Override
      public void logFloat(float value) {
        System.out.println(path + " = " + value);
      }

      @Override
      public void logDouble(double value) {
        System.out.println(path + " = " + value);
      }

      @Override
      public void logString(String value, String typeString) {
        System.out.println(path + " = " + value + " [type=" + typeString + "]");
      }

      @Override
      public void logBooleanArray(boolean[] value) {}

      @Override
      public void logShortArray(short[] value) {}

      @Override
      public void logIntArray(int[] value) {}

      @Override
      public void logLongArray(long[] value) {}

      @Override
      public void logFloatArray(float[] value) {}

      @Override
      public void logDoubleArray(double[] value) {}

      @Override
      public void logStringArray(String[] value) {}

      @Override
      public void logRaw(byte[] value, String typeString) {}
    };
  }

  @Override
  public void close() {}
}
```

Registering it for a prefix:

```java
TelemetryRegistry.registerBackend("/", new ConsoleTelemetryBackend());
Telemetry.log("batteryVoltage", 12.4);
```

## Typical Usage Patterns

### Pattern 1: Simple scalar telemetry

Use `Telemetry.log()` directly when a value is standalone and naturally belongs at the root or a single named child path.

```java
Telemetry.log("compressorEnabled", compressor.isEnabled());
Telemetry.log("gyroYawDegrees", gyro.getYaw().getDegrees());
```

### Pattern 2: Subsystem-local table

Store a `TelemetryTable` as a field when a subsystem publishes multiple related values repeatedly.

```java
public final class Intake {
  private final TelemetryTable telemetry = Telemetry.getTable("Intake");

  public void periodic() {
    telemetry.log("beamBreak", m_beamBreak.get());
    telemetry.log("rollerCurrent", m_motor.getOutputCurrent());
    telemetry.log("state", m_state.name());
  }
}
```

### Pattern 3: Structured object publication

Use `TelemetryLoggable` when an object should own its telemetry schema.

```java
Telemetry.log("superstructure", superstructureSnapshot);
```

This keeps schema decisions close to the type rather than scattering field names throughout robot code.

### Pattern 4: Explicit serialization choice

Pass a `Struct` or `Protobuf` explicitly when the value should be published as a single structured entry rather than broken into a table.

```java
telemetry.log("estimatedPose", estimator.getEstimatedPosition(), Pose2d.struct);
```

## Error Handling and Warnings

The API prefers warnings over silent failure when a caller does something inconsistent or unsupported.

Examples include:

- Logging a `TelemetryLoggable` to a table whose type has already been set to a different value

- Attempting automatic struct/protobuf detection when the static `struct` or `proto` field is missing or inaccessible

- Detecting that the serializer's declared type does not match the runtime object type

- Logging mutable non-cloneable struct/protobuf values in backends that need to snapshot them

Warnings are routed through `TelemetryRegistry.reportWarning()` and can be customized with `TelemetryRegistry.setReportWarning()`.

## Relationship to Lower-Level APIs

Telemetry is intended to cover the common cases where robot code wants to publish data without directly depending on a specific transport.

For more advanced cases, teams may still prefer to use NetworkTables or DataLog directly, for example:

- when transport-specific publisher/subscriber behavior is needed

- when precise control of timestamps, retention, or subscriptions is required

- when a dashboard already expects a transport-specific schema

The Telemetry API should be viewed as a convenience layer and object-modeling layer, not a replacement for every transport-specific feature.

# Drawbacks

- The generic `log(String, Object)` path necessarily relies on runtime dispatch and reflection for some object types.

- Automatic serializer discovery depends on agreed-upon static field names (`struct` and `proto`).

- Object-array support is intentionally conservative; non-struct object arrays currently fall back to string arrays.

- Because table schemas are built dynamically, dashboards and tools need to tolerate fields appearing over time.

# Alternatives

Use lower-level APIs directly for all telemetry publication.

Downsides of that approach:

- Repeated path and publisher boilerplate in robot code

- More manual serializer selection

- Harder reuse of complex object schemas

- Less convenient unit testing for telemetry output

# Unresolved Questions

- Should the API expose first-class timestamped logging operations, or should timestamps remain backend-managed?

- Should object-array support be expanded beyond struct arrays to other common array types?

- Should there be a standardized set of table type strings for common WPILib object categories?

# Appendix: C++ API

This appendix describes the C++ Telemetry API as it is used from C++ robot code. The API follows the same overall model as the Java API, but the way users extend and consume it is intentionally C++-idiomatic:

- the main entry points are static functions on `wpi::Telemetry` and member functions on `wpi::TelemetryTable`

- generic logging is template-based rather than overload-plus-reflection based

- structured logging support uses `wpi::util::StructSerializable` and `wpi::util::ProtobufSerializable`

- custom telemetry support is typically provided with non-member `LogTo()`, `LogValueTo()`, and `GetTelemetryTypeName()` functions found by ADL

## C++ Overview

For most robot code, a C++ user interacts with three pieces:

- `wpi::Telemetry` for simple one-line publishing

- `wpi::TelemetryTable` for grouping related values under a table path

- `wpi::TelemetryLoggable` or ADL-based helpers for complex custom types

The API is header-driven and highly generic. In many cases there is no need to manually choose between primitive, string, struct, or protobuf logging. Instead, the correct path is selected by the template constraints on `TelemetryTable::Log()`.

## `wpi::Telemetry`

`wpi::Telemetry` is the top-level convenience facade. It forwards to the root telemetry table.

```cpp
class Telemetry final {
 public:
  static wpi::TelemetryTable& GetTable();
  static wpi::TelemetryTable& GetTable(std::string_view name);

  static void KeepDuplicates(std::string_view name);
  static void SetProperty(std::string_view name, std::string_view key,
                          std::string_view value);

  template <typename T, typename... I>
  static void Log(std::string_view name, const T& value, I... info);

  template <typename T, typename... I>
  static void Log(std::string_view name, std::span<const T> value, I... info);

  static void Log(std::string_view name, bool value);
  static void Log(std::string_view name, int8_t value);
  static void Log(std::string_view name, int16_t value);
  static void Log(std::string_view name, int32_t value);
  static void Log(std::string_view name, int64_t value);
  static void Log(std::string_view name, float value);
  static void Log(std::string_view name, double value);
  static void Log(std::string_view name, std::string_view value);
  static void Log(std::string_view name, std::string_view value,
                  std::string_view typeString);

  static void Log(std::string_view name, std::span<const bool> value);
  static void Log(std::string_view name, std::span<const int16_t> value);
  static void Log(std::string_view name, std::span<const int32_t> value);
  static void Log(std::string_view name, std::span<const int64_t> value);
  static void Log(std::string_view name, std::span<const float> value);
  static void Log(std::string_view name, std::span<const double> value);
  static void Log(std::string_view name, std::span<const std::string> value);
  static void Log(std::string_view name,
                  std::span<const std::string_view> value);
  static void Log(std::string_view name, std::span<const uint8_t> value);
  static void Log(std::string_view name, std::span<const uint8_t> value,
                  std::string_view typeString);
};
```

Typical usage:

```cpp
wpi::Telemetry::Log("batteryVoltage", wpi::RobotController::GetBatteryVoltage());
wpi::Telemetry::Log("enabled", wpi::DriverStation::IsEnabled());
wpi::Telemetry::Log("state", std::string_view{"Ready"});
```

Using the root facade is appropriate when:

- a value naturally lives at the top level

- the code only needs to publish one or two values

- there is no need to cache a table reference in a subsystem or helper object

## `wpi::TelemetryTable`

`wpi::TelemetryTable` is the main C++ abstraction for grouped telemetry. A table corresponds to a path such as `/Drive/` or `/PoseEstimator/Vision/`.

```cpp
class TelemetryTable final {
 public:
  std::string_view GetPath() const;

  bool SetType(std::string_view typeString);
  std::string GetType() const;
  bool HasType() const;

  TelemetryTable& GetTable(std::string_view name) const;

  void KeepDuplicates(std::string_view name);
  void SetProperty(std::string_view name, std::string_view key,
                   std::string_view value);

  template <typename T, typename... I>
  void Log(std::string_view name, const T& value, I... info);

  template <typename T, typename... I>
  void Log(std::string_view name, std::span<const T> value, I... info);

  void Log(std::string_view name, bool value);
  void Log(std::string_view name, int8_t value);
  void Log(std::string_view name, int16_t value);
  void Log(std::string_view name, int32_t value);
  void Log(std::string_view name, int64_t value);
  void Log(std::string_view name, float value);
  void Log(std::string_view name, double value);
  void Log(std::string_view name, std::string_view value);
  void Log(std::string_view name, std::string_view value,
           std::string_view typeString);
};
```

Typical subsystem pattern:

```cpp
class Shooter {
 public:
  void Periodic() {
    m_telemetry.Log("wheelSpeedRps", m_encoder.GetRate());
    m_telemetry.Log("appliedVoltage", m_lastVoltage.value());
    m_telemetry.Log("ready", AtSpeed());
  }

 private:
  wpi::TelemetryTable& m_telemetry = wpi::Telemetry::GetTable("Shooter");
  wpi::Encoder m_encoder{0, 1};
  units::volt_t m_lastVoltage = 0_V;
};
```

Nested tables are used when the hierarchy itself is meaningful:

```cpp
auto& estimator = wpi::Telemetry::GetTable("PoseEstimator");
auto& vision = estimator.GetTable("Vision");
auto& odometry = estimator.GetTable("Odometry");

vision.Log("visibleTags", static_cast<int32_t>(tagIds.size()));
odometry.Log("pose", estimatedPose);
```

`SetType()` is used when the table represents a known structured kind of object and consumers should interpret that table consistently:

```cpp
auto& mechanism = wpi::Telemetry::GetTable("ArmMechanism");
if (mechanism.SetType("Mechanism2d")) {
  mechanism.Log("layout", mechanismJson, "json");
}
```

## How `TelemetryTable::Log()` Selects Behavior

The C++ API is designed so users usually write the same call shape regardless of the value type:

```cpp
table.Log("name", value);
```

From a user's perspective, that call works as follows:

- if the type supports table-style telemetry via `LogTo()`, it is logged into a child table

- if the type supports value-style telemetry via `LogValueTo()`, it is logged as a single named value

- if the type is `StructSerializable`, it is packed and logged as raw bytes with the corresponding struct schema/type string

- if the type is `ProtobufSerializable`, it is packed and logged as raw bytes with the corresponding protobuf schema/type string

- integral and floating-point values are logged as numeric primitives

- string-like types are logged as strings

- span-convertible containers are routed to the array logging path

- `fmt`-formattable types can fall back to formatted strings

This means the common case remains simple while still allowing strong customization for user-defined types.

## Arrays and `std::span`

In C++, array-style logging is based on `std::span`, not Java arrays.

Primitive arrays:

```cpp
std::array<double, 3> wheelSpeeds{left, right, average};
wpi::Telemetry::Log("wheelSpeeds", std::span{wheelSpeeds});

wpi::Telemetry::Log("setpoints", {1.0, 2.0, 3.0});
```

String arrays:

```cpp
std::array<std::string_view, 3> states{"Idle", "Aiming", "Shooting"};
wpi::Telemetry::Log("availableStates", std::span{states});
```

Raw bytes with a custom type string:

```cpp
std::span<const uint8_t> packet = GetSerializedFrame();
wpi::Telemetry::Log("cameraFrame", packet, "image/jpeg");
```

For arrays of custom objects, the most natural path is usually a `std::span<const T>` where `T` is struct-serializable.

```cpp
std::array<wpi::Pose2d, 2> poses{startPose, goalPose};
wpi::Telemetry::Log("waypoints", std::span{poses});
```

## Logging Structured Types with `Struct` and `Protobuf`

C++ users do not typically pass serializer objects manually. Instead, the normal pattern is to make the type satisfy the corresponding WPILib serialization concept and then call `Log()` directly.

Example with a struct-serializable type:

```cpp
wpi::Pose2d pose = estimator.GetEstimatedPosition();
wpi::Telemetry::Log("estimatedPose", pose);
```

Example with a protobuf-serializable type:

```cpp
MyProtoCompatibleType message = BuildMessage();
wpi::Telemetry::Log("visionResult", message);
```

When logging those values, the telemetry layer also ensures the corresponding schema is registered with the backend.

Additional template parameters can be forwarded when a struct serializer requires type info:

```cpp
table.Log("sample", value, info1, info2);
```

From a user's perspective, these extra arguments are part of the type-specific serialization contract for that type.

## Publishing Complex Objects with `wpi::TelemetryLoggable`

For non-final classes or types that naturally own a telemetry schema, subclassing `wpi::TelemetryLoggable` is the most direct C++ pattern.

```cpp
class DriveSnapshot : public wpi::TelemetryLoggable {
 public:
  DriveSnapshot(const wpi::DifferentialDriveWheelSpeeds& wheelSpeeds,
                const wpi::Pose2d& pose, bool closedLoop)
      : m_wheelSpeeds{wheelSpeeds}, m_pose{pose}, m_closedLoop{closedLoop} {}

  std::string_view GetTelemetryType() const override {
    return "DriveSnapshot";
  }

  void LogTo(wpi::TelemetryTable& table) const override {
    table.Log("leftMetersPerSecond", m_wheelSpeeds.left.value());
    table.Log("rightMetersPerSecond", m_wheelSpeeds.right.value());
    table.Log("pose", m_pose);
    table.Log("closedLoop", m_closedLoop);
  }

 private:
  wpi::DifferentialDriveWheelSpeeds m_wheelSpeeds;
  wpi::Pose2d m_pose;
  bool m_closedLoop;
};
```

Logging it is just:

```cpp
DriveSnapshot snapshot{m_drive.GetWheelSpeeds(), m_drive.GetPose(), m_drive.IsClosedLoop()};
wpi::Telemetry::Log("drive", snapshot);
```

That call creates or reuses a `/drive/` child table, checks the table type if one is provided, and then lets the object populate entries under that table.

## Publishing Custom Types with ADL Hooks

One of the most important differences in the C++ API is that users do not need to inherit from a base class just to make a type loggable. A type can participate in telemetry by defining non-member functions in the same namespace so they are found by argument-dependent lookup.

There are three relevant patterns.

### Pattern 1: Table-style logging via `LogTo()`

Use this when the type expands into multiple fields under a child table.

```cpp
namespace example {

struct VisionMeasurement {
  wpi::Pose2d pose;
  double timestamp;
  int tagCount;
};

inline std::string_view GetTelemetryTypeName(const VisionMeasurement&) {
  return "VisionMeasurement";
}

inline void LogTo(wpi::TelemetryTable& table, const VisionMeasurement& value) {
  table.Log("pose", value.pose);
  table.Log("timestamp", value.timestamp);
  table.Log("tagCount", value.tagCount);
}

}  // namespace example
```

Usage:

```cpp
example::VisionMeasurement measurement{pose, timestamp, tagCount};
wpi::Telemetry::Log("latestVision", measurement);
```

### Pattern 2: Value-style logging via `LogValueTo()`

Use this when a type should be emitted as a single value rather than a table.

```cpp
namespace example {

struct RgbColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

inline void LogValueTo(wpi::TelemetryTable& table, std::string_view name,
                       const RgbColor& value) {
  table.Log(name,
            fmt::format("#{:02X}{:02X}{:02X}", value.r, value.g, value.b),
            "color");
}

}  // namespace example
```

Usage:

```cpp
example::RgbColor color{255, 128, 0};
wpi::Telemetry::Log("ledColor", color);
```

### Pattern 3: Class member functions without inheritance

The header also provides bridge helpers so a class with member functions named `LogTo()` and `GetTelemetryType()` can work without explicitly inheriting from `wpi::TelemetryLoggable`.

```cpp
class SuperstructureSnapshot {
 public:
  std::string_view GetTelemetryType() const { return "SuperstructureSnapshot"; }

  void LogTo(wpi::TelemetryTable& table) const {
    table.Log("elevatorHeight", m_elevatorHeight);
    table.Log("armAngle", m_armAngle);
  }

 private:
  double m_elevatorHeight;
  double m_armAngle;
};
```

Because the required member functions exist, `wpi::Telemetry::Log("superstructure", snapshot);` works without additional glue.

## Duplicate Preservation and Properties

The C++ API exposes the same per-entry controls as the Java facade, but in C++ spelling.

```cpp
wpi::Telemetry::KeepDuplicates("loopOverrunCount");
wpi::Telemetry::SetProperty("loopOverrunCount", "unit", "\"count\"");
wpi::Telemetry::Log("loopOverrunCount", overrunCount);
```

Or from a table:

```cpp
auto& drive = wpi::Telemetry::GetTable("Drive");
drive.KeepDuplicates("leftVelocity");
drive.SetProperty("leftVelocity", "unit", "\"m/s\"");
drive.Log("leftVelocity", leftVelocity);
```

Property values are strings. When the backend expects JSON-formatted property values, callers are responsible for passing a valid JSON string representation.

## Advanced: User-Facing Registry APIs

Most robot code does not need `wpi::TelemetryRegistry`, but advanced C++ users may interact with it in a few cases:

- registering a telemetry backend for a path prefix

- overriding warning reporting for tests or diagnostics

- resetting the registry in unit tests

Relevant API:

```cpp
class TelemetryRegistry final {
 public:
  static void SetReportWarning(
      std::function<void(std::string_view path, std::string_view msg)> func);
  static void RegisterBackend(std::string_view prefix,
                              std::shared_ptr<TelemetryBackend> backend);
  static void Reset();
};
```

Typical test setup:

```cpp
wpi::TelemetryRegistry::Reset();
wpi::TelemetryRegistry::RegisterBackend("/", std::make_shared<wpi::MockTelemetryBackend>());
```

## Backend Layer: `wpi::TelemetryEntry`

Most C++ users will never implement `wpi::TelemetryEntry`, but it is the interface used by telemetry backends to receive resolved values.

```cpp
class TelemetryEntry {
 public:
  virtual void KeepDuplicates() = 0;
  virtual void SetProperty(std::string_view key, std::string_view value) = 0;

  virtual void LogBoolean(bool value) = 0;
  virtual void LogInt64(int64_t value) = 0;
  virtual void LogFloat(float value) = 0;
  virtual void LogDouble(double value) = 0;
  virtual void LogString(std::string_view value,
                         std::string_view typeString) = 0;
  virtual void LogBooleanArray(std::span<const bool> value) = 0;
  virtual void LogInt16Array(std::span<const int16_t> value) = 0;
  virtual void LogInt32Array(std::span<const int32_t> value) = 0;
  virtual void LogInt64Array(std::span<const int64_t> value) = 0;
  virtual void LogFloatArray(std::span<const float> value) = 0;
  virtual void LogDoubleArray(std::span<const double> value) = 0;
  virtual void LogRaw(std::span<const uint8_t> value,
                      std::string_view typeString) = 0;
};
```

This layer is primarily relevant to backend authors rather than normal robot-code authors.

## Typical C++ Usage Guidance

- Use `wpi::Telemetry::Log()` for isolated values.

- Hold a `wpi::TelemetryTable&` in subsystem code when publishing related values every loop.

- Prefer `LogTo()`/`GetTelemetryTypeName()` or `wpi::TelemetryLoggable` for multi-field custom objects.

- Prefer struct/protobuf support for values that already have WPILib serialization support.

- Use `std::span` or initializer lists for arrays.

- Reach for `wpi::TelemetryRegistry` and `wpi::TelemetryEntry` only when implementing advanced integration points such as custom backends or test harnesses.