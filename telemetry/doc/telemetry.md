# Overview

A unified, extensible, data-oriented, output-only telemetry API for WPILib with hierarchical structure and flexible backend implementation.

# Summary

This document describes the Telemetry API for publishing robot-program data to dashboards, debug tools, or log files.

The API is centered around four main concepts:

- `Telemetry`, a static convenience facade for simple logging

- `TelemetryTable`, a hierarchical namespace for related telemetry values

- `TelemetryLoggable`, an interface for complex objects that publish themselves into a table

- `TelemetryEntry`, the backend-facing sink for individual values

This document focuses on how teams use the API from robot code, and how backend implementers consume the lower-level interfaces.

Language-specific API details are split into separate files:

- [C++ API](cpp.md)
- [Python API](python.md)

# Motivation

Telemetry, logging, and visualization of data values are critical to debugging of real-time systems, in particular complex software-electromechanical systems such as robots, as it provides the ability to visualize the internals of system operation that are not externally observable. To this end, WPILib provides a number of core libraries for both sending real-time data values to interactive GUI (dashboard) applications (NetworkTables) and logging data values to files for offline analysis (DataLog). However, these core libraries are feature-rich, and to maximize user flexibility, have fairly complex APIs with large API surfaces.

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

# Background (WPILib 2026)

Because of the complexity of DataLog and NT APIs, most users, particularly beginners, in WPILib 2026 instead used simplified APIs that were originally designed for use with specific dashboards (SmartDashboard and Shuffleboard). These existing classes have misleading names, as all of them use NetworkTables under the hood, they are not limited to use for that specific dashboard--indeed it is quite common for users to use the SmartDashboard API with other dashboards. These APIs are also dashboard-oriented, limited (don't support modern serialization methods such as struct and protobuf), and generally have a flat structure (don't support a hierarchy of values, at least not at the API level).

As users have increasingly used the NetworkTables and DataLog APIs, demand is also rising for an API that unifies these two APIs while supporting directing the telemetry values to different destinations based on need. For example, sending some values to NetworkTables and others to DataLog. While some of this demand is being driven by performance limitations of the RoboRIO, it's also useful to have other backend options such as a mock implementation for unit testing.

# References

- [NetworkTables](../../ntcore/doc/networktables4.adoc)
- [DataLog](../../datalog/doc/datalog.adoc)

# Approach

The Telemetry API is transport-agnostic. Robot code logs to `Telemetry` or `TelemetryTable`, and `TelemetryRegistry` routes those calls to a registered `TelemetryBackend` based on path prefix.

This split is intentional:

- The main part of robot code should describe _what_ is being published, not _how_ it is transported; any transport-level configuration is generally done at the top level rather than in the details

- Different backends may target dashboards, log files, tests, or future tooling

- Unit tests should be able to observe telemetry output without requiring a live dashboard connection

The primary user-facing API is table-oriented. A table represents a path such as `/Drive/` or `/PoseEstimator/`, and entries within that table represent named values such as `leftVelocity` or `estimatedPose`.

Complex objects integrate by either implementing `TelemetryLoggable`, which receives a table and populates it with its internal state, or by implementing the `StructSerializable` or `ProtobufSerializable` interfaces.

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

## Frontend Overview / Key Features

The Java frontend API is centered on `Telemetry` and `TelemetryTable`.

`Telemetry` is a static facade over the root telemetry table (`/`). `Telemetry.getTable()` returns the root table, and `Telemetry.getTable(String name)` returns a child table. The logging methods on `Telemetry` delegate to the root table.

`TelemetryTable` represents a path in a hierarchical namespace and caches child tables and entries. Table paths are normalized to start with `/` and end with `/`; for example, `TelemetryRegistry.getTable("drive")` has path `/drive/`. Entries under a table append the entry name to the table path, so `Telemetry.getTable("drive").log("speed", 1.0)` logs `/drive/speed`.

The frontend supports logging the following value categories:

- Primitive scalar values: `boolean`, `byte`, `short`, `int`, `long`, `float`, and `double`.
- Strings, with either the default `"string"` type string or a custom type string.
- Primitive arrays: `boolean[]`, `short[]`, `int[]`, `long[]`, `float[]`, and `double[]`.
- `String[]`.
- Raw byte arrays, with either the default `"raw"` type string or a custom type string.
- Struct-serialized objects with an explicit `Struct<T>`.
- Protobuf-serialized objects with an explicit `Protobuf<T, ?>`.
- Struct-serialized object arrays with an explicit `Struct<T>`.
- Collections with either an explicit element class or an explicit `Struct<T>`.
- Generic objects and generic object arrays.

Automatic handling exists for `StructSerializable` and `ProtobufSerializable` scalar values when the corresponding static `struct` or `proto` field is available. Struct-serializable arrays and collections are supported through the object-array path or the collection overloads. Collections without an explicit element type are rejected so empty collections and erased generic types do not accidentally choose the wrong backend representation.

In addition, the frontend supports customized value logging via two mechanisms:

- `TelemetryLoggable` is the interface for complex objects that log multiple fields to a table. Its `logTo(TelemetryTable table)` method writes the object's fields. `getTelemetryType()` may return a table type string; the default is `null`, meaning no specified type. When a typed loggable object is logged, the table records the type in a `.type` string entry. Future logs to the same table with a different type report a warning and are skipped for that value.

- Arbitrary object types can be supported through `TelemetryRegistry.registerTypeHandler()`.

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
  public static <T> void log(String name, T value, Struct<? super T> struct) {...}
  public static <T> void log(String name, T value, Protobuf<? super T, ?> proto) {...}
  public static void log(String name, Object[] value) {...}
  public static <T> void log(String name, T[] value, Struct<? super T> struct) {...}
  public static void log(String name, Collection<?> value) {...}
  public static <T> void log(String name, Collection<T> value, Struct<? super T> struct) {...}
  public static void log(String name, Collection<?> value, Class<?> elementType) {...}

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

All functions operate as if they were called via the root `TelemetryTable`.

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

Logging a structured value implicitly:

```java
Telemetry.log("targetPose", pose);
Telemetry.log("detectedTag", tagDetection);
```

Logging a structured value explicitly:

```java
Telemetry.log("targetPose", pose, Pose2d.struct);
Telemetry.log("detectedTag", tagDetection, AprilTagDetection.proto);
```

Robot-periodic status logging:

```java
public final class Robot extends TimedRobot {
  private final DriveSubsystem m_drive = new DriveSubsystem();
  private final TelemetryTable m_robotTelemetry = Telemetry.getTable("Robot");

  @Override
  public void robotPeriodic() {
    m_robotTelemetry.log("batteryVoltage", RobotController.getBatteryVoltage());
    m_robotTelemetry.log("enabled", DriverStation.isEnabled());
    m_robotTelemetry.log("matchTime", Timer.getMatchTime());

    Telemetry.log("drivePose", m_drive.getPose());
    Telemetry.log("driveState", m_drive.getState().name());
  }
}
```

### TelemetryTable

`TelemetryTable` is the core abstraction behind the static facade. It represents a single path and provides hierarchical logging operations.

```java
public final class TelemetryTable {
  public TelemetryTable(TelemetryBackend backend) {...}

  public String getPath() {...}

  public boolean setType(String typeString) {...}
  public String getType() {...}

  public TelemetryTable getTable(String name) {...}

  public void keepDuplicates(String name) {...}
  public void setProperty(String name, String key, String value) {...}

  public <T> void log(String name, T value) {...}
  public <T> void log(String name, T value, Struct<? super T> struct) {...}
  public <T> void log(String name, T value, Protobuf<? super T, ?> proto) {...}
  public <T> void log(String name, T[] value) {...}
  public <T> void log(String name, T[] value, Struct<? super T> struct) {...}
  public void log(String name, Collection<?> value) {...}
  public <T> void log(String name, Collection<T> value, Struct<? super T> struct) {...}
  public void log(String name, Collection<?> value, Class<?> elementType) {...}

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

`log(String, Object)` performs runtime dispatch with a fixed precedence order:

1. If the value implements `TelemetryLoggable`, it is logged to a child table with the provided name.
2. If the value implements `StructSerializable`, the frontend looks for a public static `struct` field and logs it with that serializer.
3. If the value implements `ProtobufSerializable`, the frontend looks for a public static `proto` field and logs it with that serializer.
4. Boxed `Boolean`, `Float`, `Double`, `Byte`, `Short`, `Integer`, `Long`, other `Number` values, and `String` values are logged as their corresponding telemetry types. Boxed integral values are logged as `long`; generic `Number` values other than boxed integral types are logged as `double`.
5. A registered `TelemetryRegistry.TypeHandler` may handle the value.
6. If no handler matches, the value is logged as `value.toString()`.

Generic object arrays should not be used for primitive arrays. Primitive arrays have dedicated overloads. For arrays of structured values, prefer the explicit `log(String name, T[] value, Struct<? super T> struct)` overload. Boxed `Byte[]` arrays are logged as raw byte arrays. Boxed `Short[]`, `Integer[]`, and `Long[]` arrays are logged as long arrays. Boxed `Float[]` and `Double[]` arrays are logged as float or double arrays. Other `Number[]` arrays are logged as double arrays, and other object arrays are converted element-by-element with `toString()` and logged as a string array.

`Collection<?>` overloads require an explicit element type or explicit `Struct` serializer. Boolean collections log as boolean arrays; byte element types (`byte` and `Byte`) log as raw byte arrays; short, int, and long element types and their boxed equivalents log as long arrays; float and double element types log as float or double arrays; other `Number` element types log as double arrays; string collections log as string arrays; struct-serializable element types log as struct arrays; and unsupported element types fall back to string arrays.

`TelemetryTable.setType(String typeString)` can also set a table type directly. It returns `false` on a type mismatch and reports a warning. Callers should check the return value and avoid logging data when it returns `false`.

`keepDuplicates(String name)` marks an entry so duplicate values are preserved. By default, backends may suppress duplicate values.

`setProperty(String name, String key, String value)` sets metadata for an entry. The value must be a valid JSON value string, such as `"meters"` (written in Java as `"\"meters\""`), `1`, `true`, `null`, an object, or an array.

Warnings, such as type mismatches or missing serializer fields, are reported through `TelemetryRegistry.reportWarning()`. The default warning function writes to `System.err`; callers can replace it with `TelemetryRegistry.setReportWarning()`.

`setType()` establishes a stable type string for the table. A later attempt to set a different type results in a warning and returns `false`.

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
odometry.log("pose", currentPose);
```

Publishing arrays and collections with explicit element types:

```java
TelemetryTable drive = Telemetry.getTable("Drive");

drive.log(
    "moduleSpeedsMetersPerSecond",
    new double[] {
      frontLeft.getDriveVelocity(),
      frontRight.getDriveVelocity(),
      backLeft.getDriveVelocity(),
      backRight.getDriveVelocity()
    });
drive.log("activeFaults", activeFaultNames, String.class);
drive.log("plannedPath", plannedPoses, Pose2d.struct);
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
    table.log("pose", m_pose);
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

Type handlers can also log a value directly to the provided entry name instead of creating a subtable. This is how `RobotBase` registers unit `Measure` telemetry:

```java
TelemetryRegistry.registerTypeHandler(
    Measure.class,
    (table, name, value) -> {
      table.setProperty(name, "unit", "\"" + value.unit().name() + "\"");
      table.log(name, value.magnitude());
    });
```

After registration, callers can write:

```java
Telemetry.log("batteryVoltage", RobotController.getMeasureBatteryVoltage());
```

That call logs a numeric value at `/batteryVoltage` and attaches the unit as entry metadata, rather than creating `/batteryVoltage/` as a table.

## Thread Safety and Concurrent Logging

The Telemetry frontend and the WPILib-provided NetworkTables, DataLog, discard, and multi backends support concurrent logging. Robot code may call `Telemetry.log()` or methods on a shared `TelemetryTable` from secondary user threads; Telemetry has no `update()` method and does not require calls to run on the main robot thread.

The thread-safety boundary is an individual logging or metadata call:

- Calls from different threads may complete in either order. Concurrent calls to the same path are safe, but the resulting value order is unspecified.

- Logging a `TelemetryLoggable` is a sequence of independent table operations, not an atomic snapshot. A consumer can observe some new fields and some old fields while the object is being logged. If the fields must be mutually consistent, the caller should copy the source state into an immutable snapshot before logging it, or hold the application's state lock while reading all of the fields.

- Telemetry protects its own tables, entry caches, and registry state; it does not protect objects owned by robot code. The caller must prevent an array, collection, serialized object, or object read by `logTo()` from being mutated concurrently while Telemetry reads it. Backends consume values during the logging call; a custom backend that keeps mutable or non-owning input for later use must copy it first.

- A path has one value type, and a table has one optional table type. If threads race to establish incompatible types, one type wins and the other value is skipped with a warning. Establish names, table types, properties, and duplicate-preservation settings during initialization when practical. Table type publication and the member logs performed by `logTo()` are separate operations, so consumers must also tolerate a newly appearing table being only partially populated.

The registry remains usable while logging, including during backend replacement, but it does not serialize calls into a backend. A custom `TelemetryBackend` and each `TelemetryEntry` it returns must therefore be thread-safe: entry creation, schema publication, metadata changes, logging, removal, and backend lifecycle operations may overlap. An entry already obtained by a logging thread may receive a final call while another thread reroutes or removes its path. Backend implementations should use fine-grained synchronization so unrelated telemetry paths do not contend on one global lock.

Registered type handlers and warning handlers can likewise be invoked concurrently by logging threads. They must protect any shared state and must not throw.

`MockTelemetryBackend` synchronizes recording, but its inspection APIs may return live collections, references, or pointers. Tests should stop or otherwise synchronize logging before iterating those results or retaining returned references.

## Backend Overview / Key Features

Backends implement `TelemetryBackend`. The backend's required factory method is `getEntry(String path)`, which returns a `TelemetryEntry` for a normalized full telemetry path. Backend entry creation, entry logging, metadata updates, discard checks, entry removal, and schema publication must not throw; recoverable failures should be reported as telemetry warnings and skipped, or represented with a discard entry. Backends may also override `removeEntry(String path)` so the registry can retire stale cached entries when backend routing changes. `TelemetryBackend` extends `AutoCloseable`, so `TelemetryRegistry.reset()` closes registered Java backends. Composite backends can override `ownsBackend()` to report child backends they close themselves.

`TelemetryEntry` is the per-entry backend interface. It receives typed logging calls from the frontend:

- `keepDuplicates()`
- `isDiscard()`, which lets a backend short-circuit work for entries that discard values
- `setProperty(String key, String value)`
- `logStruct()`, `logProtobuf()`, and `logStructArray()`
- scalar logging methods for booleans, integral values, floating-point values, and strings
- array logging methods for supported primitive and string arrays
- `logRaw()` for byte arrays

`logByte()`, `logShort()`, and `logInt()` default to `logLong()` unless a backend overrides them.

`TelemetryRegistry.registerBackend(String prefix, TelemetryBackend backend)` associates a backend with a path prefix. When an entry is requested, the registry normalizes the path and selects the backend with the longest matching prefix. Re-registering backends removes affected entries from their previous backends, clears cached entries in existing tables so later logs use the updated backend mapping, and closes displaced Java backend instances that are no longer registered or owned by a registered composite backend.

Entry metadata set through globally routed tables is retained by the registry and reapplied when an entry is recreated after a cache reset. A `TelemetryTable` may also be constructed directly with a backend for isolated tests or custom routing; that direct table bypasses the global registry and applies metadata only to the backend entry it creates.

Robot programs normally get a default NetworkTables backend from `RobotBase`, registered with an empty prefix and a NetworkTables path prefix of `/Telemetry`. Additional backends can be registered for narrower path prefixes. Unit tests can use `MockTelemetryBackend`; `DiscardTelemetryBackend` is available when logged values should be ignored.

`TelemetryRegistry.registerTypeHandler(Class<T> cls, TypeHandler<T> handler)` registers frontend object handlers. More specific classes are ordered before less specific classes. Registering a handler for an existing class replaces the previous handler.

### TelemetryEntry

`TelemetryEntry` is the low-level per-path sink used by telemetry backends. Most robot code should not interact with this interface directly.

```java
public interface TelemetryEntry {
  default boolean isDiscard() {...}

  void keepDuplicates();
  void setProperty(String key, String value);

  <T> void logStruct(T value, Struct<? super T> struct, long timestamp);
  <T> void logProtobuf(T value, Protobuf<? super T, ?> proto, long timestamp);
  <T> void logStructArray(T[] value, Struct<? super T> struct, long timestamp);

  void logBoolean(boolean value, long timestamp);
  default void logByte(byte value, long timestamp) {...}
  default void logShort(short value, long timestamp) {...}
  default void logInt(int value, long timestamp) {...}
  void logLong(long value, long timestamp);
  void logFloat(float value, long timestamp);
  void logDouble(double value, long timestamp);
  void logString(String value, String typeString, long timestamp);

  void logBooleanArray(boolean[] value, long timestamp);
  void logShortArray(short[] value, long timestamp);
  void logIntArray(int[] value, long timestamp);
  void logLongArray(long[] value, long timestamp);
  void logFloatArray(float[] value, long timestamp);
  void logDoubleArray(double[] value, long timestamp);
  void logStringArray(String[] value, long timestamp);
  void logRaw(byte[] value, String typeString, long timestamp);
}
```

Key behavior:

- A `TelemetryBackend` creates entries by full path.

- `TelemetryTable` resolves names to entries lazily and caches them.

- `logByte`, `logShort`, and `logInt` default to widening into `logLong`, so a backend can implement integer handling with a single required path.

- Backends are responsible for storing, transporting, deduplicating, and applying timestamps as appropriate for that transport. The `timestamp` parameter is in nanoseconds in the same time base as `WPIUtilJNI.now()`, or `0` to use the current time.

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
      public <T> void logStruct(T value, Struct<? super T> struct, long timestamp) {
        System.out.println(path + " = " + value + " [struct=" + struct.getTypeName() + "]");
      }

      @Override
      public <T> void logProtobuf(T value, Protobuf<? super T, ?> proto, long timestamp) {
        System.out.println(path + " = " + value + " [proto=" + proto.getTypeString() + "]");
      }

      @Override
      public <T> void logStructArray(T[] value, Struct<? super T> struct, long timestamp) {
        System.out.println(path + " = struct array length " + value.length);
      }

      @Override
      public void logBoolean(boolean value, long timestamp) {
        System.out.println(path + " = " + value);
      }

      @Override
      public void logLong(long value, long timestamp) {
        System.out.println(path + " = " + value);
      }

      @Override
      public void logFloat(float value, long timestamp) {
        System.out.println(path + " = " + value);
      }

      @Override
      public void logDouble(double value, long timestamp) {
        System.out.println(path + " = " + value);
      }

      @Override
      public void logString(String value, String typeString, long timestamp) {
        System.out.println(path + " = " + value + " [type=" + typeString + "]");
      }

      @Override
      public void logBooleanArray(boolean[] value, long timestamp) {}

      @Override
      public void logShortArray(short[] value, long timestamp) {}

      @Override
      public void logIntArray(int[] value, long timestamp) {}

      @Override
      public void logLongArray(long[] value, long timestamp) {}

      @Override
      public void logFloatArray(float[] value, long timestamp) {}

      @Override
      public void logDoubleArray(double[] value, long timestamp) {}

      @Override
      public void logStringArray(String[] value, long timestamp) {}

      @Override
      public void logRaw(byte[] value, String typeString, long timestamp) {}
    };
  }

  @Override
  public void removeEntry(String path) {}

  @Override
  public void close() {}
}
```

Registering it for a prefix:

```java
TelemetryRegistry.registerBackend("/", new ConsoleTelemetryBackend());
Telemetry.log("batteryVoltage", 12.4);
```

# Examples

## Typical Usage Patterns

### Pattern 1: Simple scalar telemetry

Use `Telemetry.log()` directly when a value is standalone and naturally belongs at the root or a single named child path.

```java
Telemetry.log("compressorEnabled", compressor.isEnabled());
Telemetry.log("gyroYaw", gyro.getYaw());
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

### Pattern 3: Structured object publication with subtable

Use `TelemetryLoggable` when an object wants to publish multiple values within a subtable.

```java
Telemetry.log("superstructure", superstructureSnapshot);
```

This keeps schema decisions close to the type rather than scattering field names throughout robot code.

### Pattern 4: Structured object publication with serialization

The `Object` overload will automatically log objects that implement `StructSerializable` and `ProtobufSerializable` as single serialized values.

```java
Pose2d pose = ...;

Telemetry.log("pose", pose);
```

Additionally, a `Struct` or `Protobuf` serializer can be specified explicitly for objects that do not implement the corresponding interface.

```java
telemetry.log("complexObject", complexObject, complexObjectStruct);
```

### Pattern 5: Robot-loop diagnostics

Use a root or subsystem table for values that make sense to update every loop, such as electrical health, control state, and the latest pose estimate.

```java
public final class DriveSubsystem extends SubsystemBase {
  private final TelemetryTable m_telemetry = Telemetry.getTable("Drive");

  @Override
  public void periodic() {
    m_telemetry.log("leftPositionMeters", m_leftEncoder.getDistance());
    m_telemetry.log("rightPositionMeters", m_rightEncoder.getDistance());
    m_telemetry.log("leftVelocityMetersPerSecond", m_leftEncoder.getRate());
    m_telemetry.log("rightVelocityMetersPerSecond", m_rightEncoder.getRate());
    m_telemetry.log("estimatedPose", m_poseEstimator.getEstimatedPosition());
    m_telemetry.log("closedLoop", m_closedLoopEnabled);
  }
}
```

This keeps the dashboard path stable (`/Drive/...`) while keeping the telemetry calls next to the subsystem state they describe.

## Error Handling and Warnings

The API prefers warnings over silent failure when a caller does something inconsistent or unsupported.

Examples include:

- Logging a `TelemetryLoggable` to a table whose type has already been set to a different value

- Attempting automatic struct/protobuf detection when the static `struct` or `proto` field is missing or inaccessible

- Detecting that the serializer's declared type does not match the runtime object type

- Logging mutable non-cloneable struct/protobuf values in backends that need to snapshot them

Warnings are routed through `TelemetryRegistry.reportWarning()` and can be customized with `TelemetryRegistry.setReportWarning()`. Custom warning handlers must not throw.

## Relationship to Lower-Level APIs

Telemetry is intended to cover the common cases where robot code wants to publish data without directly depending on a specific transport.

For more advanced cases, teams may still prefer to use NetworkTables or DataLog directly, for example:

- when transport-specific publisher/subscriber behavior is needed

- when precise control of timestamps, retention, or subscriptions is required

- when a dashboard already expects a transport-specific schema

The Telemetry API should be viewed as a convenience layer and object-modeling layer, not a replacement for every transport-specific feature.

# Migration from WPILib 2026

For values that were only displayed with `SmartDashboard.put*()`, use `Telemetry`. For values that were displayed and then read back with `SmartDashboard.get*()` so the dashboard could change robot behavior, use `Tunable` instead.

## SmartDashboard Output to Telemetry

**Was (WPILib 2026):**

```java
public void robotPeriodic() {
  SmartDashboard.putNumber(
      "Drive/leftVelocity", m_leftEncoder.getRate());
  SmartDashboard.putNumber(
      "Drive/rightVelocity", m_rightEncoder.getRate());
  SmartDashboard.putBoolean("Drive/ready", atSpeed());
}
```

**Is (Telemetry):**

```java
private final TelemetryTable m_driveTelemetry =
    Telemetry.getTable("Drive");

public void robotPeriodic() {
  m_driveTelemetry.log("leftVelocity", m_leftEncoder.getRate());
  m_driveTelemetry.log("rightVelocity", m_rightEncoder.getRate());
  m_driveTelemetry.log("ready", atSpeed());
}
```

## Structured Dashboard Values

**Was (WPILib 2026):**

```java
public void robotPeriodic() {
  Pose2d pose = m_poseEstimator.getEstimatedPosition();
  SmartDashboard.putNumberArray(
      "RobotPose",
      new double[] {
        pose.getX(),
        pose.getY(),
        pose.getRotation().getRadians()
      });
}
```

**Is (Telemetry):**

```java
public void robotPeriodic() {
  Telemetry.log(
      "RobotPose",
      m_poseEstimator.getEstimatedPosition());
}
```

## Complex Sendable Values

For objects that were previously published once with `SmartDashboard.putData()`, log the object periodically instead. This keeps the dashboard value refreshed through the Telemetry backend.

**Was (WPILib 2026):**

```java
private final Field2d m_field = new Field2d();

public void robotInit() {
  SmartDashboard.putData("Field", m_field);
}

public void robotPeriodic() {
  m_field.setRobotPose(m_poseEstimator.getEstimatedPosition());
}
```

**Is (Telemetry):**

```java
private final Field2d m_field = new Field2d();

public void robotPeriodic() {
  m_field.setRobotPose(m_poseEstimator.getEstimatedPosition());
  Telemetry.log("Field", m_field);
}
```

## SmartDashboard Tuning to Tunable

If the old code used `getNumber()` or another `get*()` call to let dashboard changes feed back into robot behavior, migrate that value to the Tunable API instead of Telemetry.

**Was (WPILib 2026):**

```java
private double m_intakeSpeed = 0.65;

public void robotPeriodic() {
  SmartDashboard.putNumber("Intake/speed", m_intakeSpeed);
  m_intakeSpeed =
      SmartDashboard.getNumber("Intake/speed", m_intakeSpeed);
  m_intakeMotor.set(m_intakeSpeed);
}
```

**Is (Tunable):**

```java
private final TunableDouble m_intakeSpeed =
    Tunables.addDouble("Intake/speed", 0.65);

public void robotPeriodic() {
  m_intakeMotor.set(m_intakeSpeed.get());
}
```

# Drawbacks

- The API is output-only; code that needs bidirectional data exchange should use NetworkTables or the tunable APIs directly.

- Generic object logging relies on runtime type checks and reflection for `StructSerializable` and `ProtobufSerializable` values.

- Object-array support is intentionally conservative; non-struct object arrays currently fall back to string arrays.

- Because table schemas are built dynamically, dashboards and tools need to tolerate fields appearing over time.

- A default backend must be registered before logging values. Without a matching backend, entry creation cannot succeed.

# Alternatives

Use lower-level APIs such as NetworkTables or DataLog directly for all telemetry publication.

Downsides of that approach:

- Repeated path and publisher boilerplate in robot code

- More manual serializer selection

- Harder reuse of complex object schemas

- Less convenient unit testing for telemetry output

# Unresolved Questions

- Should the API expose first-class timestamped logging operations, or should timestamps remain backend-managed?

- Should object-array support be expanded beyond struct arrays to other common array types?

- Should there be a standardized set of table type strings for common WPILib object categories?
