# Summary

This document describes a standardized approach for robot program values to be observed and modified from dashboards or debug tools at runtime, without recompilation of the robot program.

A "tunable" is a value that can be read and/or written from an external tool such as a dashboard.

# Motivation

The ability to monitor and adjust robot program values from a dashboard is a very common need across FTC and FRC teams.  Common use cases include:

- Tuning control loop gains (PID constants, feedforward coefficients) without stopping the robot or recompiling code

- Toggling feature flags or operational modes during testing

- Providing a selection control (e.g. autonomous strategy selection, drive mode selection) driven by dashboard input

WPILib 2026's primary mechanism for this is the `SmartDashboard` API, which provides a simplified interface to get and set values. Complex objects implement the `Sendable` interface that works with `SmartDashboard.putData()` to refresh data each periodic loop.

Alternatively teams can use the NetworkTables API directly, which requires teams to manually create `NetworkTableEntry` or publisher/subscriber objects at specific NT paths, and to manually call `getDouble()`/`setDouble()` (or equivalent) inside the robot loop.  This is flexible but verbose, makes the intent unclear at the declaration site, and tightly couples robot code to the NT API.

# Background

WPILib ships `SmartDashboard` and `Shuffleboard`, both of which are thin wrappers around NetworkTables.

There are two options at present in `SmartDashboard` for tuning values. One option is for teams to call e.g. `SmartDashboard.putNumber("kP", kP)` every loop to publish a value and `SmartDashboard.getNumber("kP", 0)` to read it back.  This works but has several drawbacks:

- The pattern conflates "publish a value for observation" with "allow a value to be changed remotely"; there is no explicit mutability flag.
- There is no support for complex objects; Struct/Protobuf objects require manual serialization boilerplate.

`SmartDashboard.putData()` allows background publishing with use of the `Sendable` interface implemented by the published object. This provides a one-shot approach to users for certain objects, but is not friendly to tuning single values. The current `Sendable` interface also does not expose NT configuration; it's necessary to use the `NTSendable` interface for this.

# Requirements and Desirable Features

- Robot code should be able to declare a tunable value at the point of use (e.g. as a field next to the constant it replaces), not in a separate initialization method.

- In Java, primitive types (`boolean`, `int`, `long`, `float`, `double`) should be supported without boxing overhead in the common path.

- Struct- and Protobuf-serializable types should be supported with automatic serializer detection.

- Complex objects that consist of multiple internal values (e.g. a PID controller) should be able to publish themselves to a named sub-table.

- A hierarchical namespace (table/subtable model) should be available so that related tunables can be grouped together.

- The transport mechanism (e.g. NetworkTables) should be pluggable; robot code should not depend directly on NT.

- A mock backend should exist to enable unit testing without needing to map the values through NT.

- Callbacks should be available to allow robot code to react to a value being changed from the dashboard.

- The API should be usable from Java, C++, and Python.

# Design

## Frontend Overview / Key Features

The Java frontend API is centered on `Tunables`, `TunableTable`, and tunable value objects.

- A `Tunable<T>` object wraps a single value.  It can be backed by an internal stored value or delegated to a getter/setter pair from existing robot code.

- In Java, primitive-specialized subclasses (`TunableBoolean`, `TunableInt`, `TunableLong`, `TunableFloat`, `TunableDouble`) avoid boxing for the common numeric types.

- `ComplexTunable` is an interface for objects that own multiple related tunables.  Implementing classes call `TunableTable` methods inside `publishTunable()` to describe their structure; the backend receives a subtable and populates it accordingly.

- `TunableTable` represents a hierarchical namespace node.  Tables can be nested arbitrarily.  Tunables are published to a table by name.

- `Tunables` is a static utility class providing convenience factory methods that publish to the root table.  It is the primary entry point for simple use cases.

- `TunableConfig` / `TunableOption` carry optional metadata: mutability, robustness, type string, JSON-valued properties, whether getter-backed tunables should be read every update, and an `onTune` callback.

- `Selectable<V>` is a `ComplexTunable` that presents a drop-down list of named options and calls an optional listener when the selection changes.

Robot code normally uses the frontend API only.  Publishing a tunable registers the value with the backend layer; subsequent calls to `TunableRegistry.update()` allow the backend to apply remote writes and run callbacks.

## Core APIs

### TunableBase

`TunableBase` is the abstract base class for all tunable objects.  It stores an optional `TunableConfig` and a changed flag that subclasses set to `true` when the value is updated via `set()`.  The flag is cleared by calling `resetChanged()`.

```java
public abstract class TunableBase {
  // returns true if the value has changed since the flag was last reset
  public boolean hasChanged() {...}

  // clears the changed flag; generally only called by backends
  public void resetChanged() {...}

  // returns the configuration for this tunable
  public TunableConfig getConfig() {...}

  // returns true if this tunable is configured as robust (echoes value back after setting)
  public boolean isRobust() {...}

  // returns the properties JSON string
  public String getProperties() {...}
}
```

### Tunable\<T\>

`Tunable<T>` is a generic tunable for a simple value type.  It extends `TunableBase` and implements both `Supplier<T>` and `Consumer<T>`.

The preferred creation patterns are:

- **From a non-null initial value** – the value is stored internally.  For types that implement `StructSerializable` or `ProtobufSerializable`, the appropriate serializer is detected automatically via reflection.
- **From a getter/setter pair** – the tunable delegates to existing state; `ALWAYS_GET` is set automatically so the backend reads the current value every update cycle.
- **Null initial value** – useful when a value is not yet known at construction time; use `createNull()` or an explicit Struct/Protobuf serializer.

Struct and Protobuf serializers can also be supplied explicitly to override automatic detection.

```java
public abstract class Tunable<T> extends TunableBase implements Supplier<T>, Consumer<T> {
  // creates a tunable backed by an internal stored value; initialValue must be non-null
  // and auto-detects Struct/Protobuf
  public static <T> Tunable<T> create(T initialValue) {...}

  // creates a tunable with an explicit Struct serializer; initialValue may be null
  public static <T> Tunable<T> create(T initialValue, Struct<T> struct) {...}

  // creates a tunable with an explicit Protobuf serializer; initialValue may be null
  public static <T> Tunable<T> create(T initialValue, Protobuf<T, ?> proto) {...}

  // creates a tunable delegating to an existing getter/setter
  public static <T> Tunable<T> create(Supplier<T> getter, Consumer<T> setter, Class<T> cls) {...}

  // creates a tunable with a null initial value
  public static <T> Tunable<T> createNull(Class<T> cls) {...}

  // "Config" variants of all of the above accept a TunableConfig as a final parameter

  public abstract void set(T value) {...}

  @Override
  public abstract T get() {...}

  // Consumer<T> delegation
  @Override
  public void accept(T value) { set(value); }

  public abstract Class<T> getTypeClass() {...}
}
```

### Primitive Tunable Subclasses (Java only)

In Java, to avoid boxing, primitive-specialized subclasses are provided.  Each follows the same factory pattern as `Tunable<T>`: `create()` variants for internal storage or getter/setter delegation, a no-argument `create()` using the type's zero/false default, and `createConfig()` variants that accept a `TunableConfig`.

```java
public abstract class TunableBoolean extends TunableBase implements BooleanSupplier, BooleanConsumer {
  public static TunableBoolean create() {...}
  public static TunableBoolean create(boolean initialValue) {...}
  public static TunableBoolean create(BooleanSupplier getter, BooleanConsumer setter) {...}
  public abstract void set(boolean value);
  public abstract boolean get();
}

public abstract class TunableInt extends TunableBase implements IntSupplier, IntConsumer {
  public static TunableInt create() {...}
  public static TunableInt create(int initialValue) {...}
  public static TunableInt create(IntSupplier getter, IntConsumer setter) {...}
  public abstract void set(int value);
  public abstract int get();
}

public abstract class TunableLong extends TunableBase implements LongSupplier, LongConsumer {
  public static TunableLong create() {...}
  public static TunableLong create(long initialValue) {...}
  public static TunableLong create(LongSupplier getter, LongConsumer setter) {...}
  public abstract void set(long value);
  public abstract long get();
}

public abstract class TunableFloat extends TunableBase implements FloatSupplier, FloatConsumer {
  public static TunableFloat create() {...}
  public static TunableFloat create(float initialValue) {...}
  public static TunableFloat create(FloatSupplier getter, FloatConsumer setter) {...}
  public abstract void set(float value);
  public abstract float get();
}

public abstract class TunableDouble extends TunableBase implements DoubleSupplier, DoubleConsumer {
  public static TunableDouble create() {...}
  public static TunableDouble create(double initialValue) {...}
  public static TunableDouble create(DoubleSupplier getter, DoubleConsumer setter) {...}
  public abstract void set(double value);
  public abstract double get();
}
```

### ComplexTunable

`ComplexTunable` is an interface for objects whose tunable state is composed of multiple sub-values.  Implementing classes receive a `TunableTable` and are responsible for publishing their own sub-entries in `publishTunable()`.  An optional `getTunableType()` can be overridden to indicate to dashboards what type of widget to display.  `updateTunable()` is called by the backend on every update cycle and may be overridden to push new computed values into the published entries.

```java
public interface ComplexTunable {
  // optional type hint for dashboards; null by default
  default String getTunableType() { return null; }

  // called once when the object is published; implementations should
  // call table.publish*() to register their sub-entries
  void publishTunable(TunableTable table);

  // called every update cycle; may be overridden to push new values
  default void updateTunable() {}
}
```

### TunableTable

`TunableTable` represents a node in the hierarchical tunable namespace.  Tables are obtained via `Tunables.getTable(name)` or by calling `getTable(name)` on an existing table.  `TunableTable` objects are cached by path in `TunableRegistry` so the same path always returns the same instance.

```java
public final class TunableTable {
  // returns the path of this table (with trailing "/")
  public String getPath() {...}

  // gets a child table
  public TunableTable getTable(String name) {...}

  // publishes a TunableBase under this table
  public void publish(String name, TunableBase tunable) {...}

  // publishes a ComplexTunable under this table
  public void publish(String name, ComplexTunable tunable) {...}

  // convenience methods that create and publish in one call
  public TunableBoolean publishBoolean(String name, BooleanSupplier getter, BooleanConsumer setter) {...}
  public TunableInt    publishInt(String name, IntSupplier getter, IntConsumer setter) {...}
  public TunableLong   publishLong(String name, LongSupplier getter, LongConsumer setter) {...}
  public TunableFloat  publishFloat(String name, FloatSupplier getter, FloatConsumer setter) {...}
  public TunableDouble publishDouble(String name, DoubleSupplier getter, DoubleConsumer setter) {...}
  public <T> Tunable<T> publishValue(String name, Supplier<T> getter, Consumer<T> setter, Class<T> cls) {...}

  // removes a tunable from this table
  public void remove(String name) {...}
}
```

### Tunables

`Tunables` is a static utility class that operates on the root table (`"/"`).  It is the primary entry point for simple use cases where hierarchy is not needed.

```java
public final class Tunables {
  // returns the root TunableTable
  public static TunableTable getTable() {...}

  // returns a named child of the root table
  public static TunableTable getTable(String name) {...}

  // creates, publishes, and returns a Tunable<T> at the root
  public static <T> Tunable<T> add(String name, T initialValue) {...}

  // primitive-specific variants
  public static TunableBoolean addBoolean(String name, boolean initialValue) {...}
  public static TunableInt     addInt(String name, int initialValue) {...}
  public static TunableLong    addLong(String name, long initialValue) {...}
  public static TunableFloat   addFloat(String name, float initialValue) {...}
  public static TunableDouble  addDouble(String name, double initialValue) {...}

  // publishes a ComplexTunable and returns it
  public static <T extends ComplexTunable> T addComplex(String name, T tunable) {...}

  // lower-level publish (does not create the tunable)
  public static void publish(String name, TunableBase tunable) {...}
  public static void publish(String name, ComplexTunable tunable) {...}

  // getter/setter-backed publishing at the root table
  public static TunableBoolean publishBoolean(String name, BooleanSupplier getter, BooleanConsumer setter) {...}
  public static TunableInt     publishInt(String name, IntSupplier getter, IntConsumer setter) {...}
  public static TunableLong    publishLong(String name, LongSupplier getter, LongConsumer setter) {...}
  public static TunableFloat   publishFloat(String name, FloatSupplier getter, FloatConsumer setter) {...}
  public static TunableDouble  publishDouble(String name, DoubleSupplier getter, DoubleConsumer setter) {...}
  public static <T> Tunable<T> publishValue(String name, Supplier<T> getter,
                                            Consumer<T> setter, Class<T> cls) {...}

  // removes a tunable from the root table
  public static void remove(String name) {...}
}
```

### TunableConfig and TunableOption

`TunableConfig` carries optional metadata for a tunable.  It is immutable; wither-style methods return new instances.  The most common way to create one is `TunableConfig.of(TunableOption...)`.

`TunableOption` is a sealed interface whose records encode each possible option.  `Property` values are strings containing valid JSON values, such as `"0"`, `"true"`, or `"\"label\""`.

| Option | Meaning |
|---|---|
| `ROBUST` / `NOT_ROBUST` | When robust, the backend echoes the value back after a remote set, confirming delivery. |
| `MUTABLE` / `IMMUTABLE` | When immutable, the backend never calls `set()` in response to remote writes; the value is read-only from the dashboard's perspective. |
| `ALWAYS_GET` / `GET_ON_CHANGE` | When `ALWAYS_GET`, the backend reads `get()` every update cycle rather than only when the value has changed.  Automatically applied to getter/setter-backed tunables. |
| `Property(key, value)` | JSON-valued property passed to the backend (e.g. min/max hints for a slider widget). |
| `TypeString(typeString)` | Overrides the type string used by the backend for this entry. |
| `OnTune(callback)` | A `Runnable` invoked by the backend after a remote write is applied. |

```java
// example
TunableConfig config = TunableConfig.of(
    TunableOption.ROBUST,
    TunableOption.property("min", "0"),
    TunableOption.property("max", "1"),
    TunableOption.onTune(() -> System.out.println("kP was tuned!"))
);
TunableDouble kP = TunableDouble.createConfig(0.1, config);
Tunables.publish("kP", kP);
```

### Selectable\<V\>

`Selectable<V>` is a `ComplexTunable` that provides the equivalent of `SendableChooser` from WPILib 2026.  It maintains a map of `String` → `V` and publishes three sub-entries to its table:

- `"default"` (immutable) – the default option name.
- `"options"` (immutable) – a string array of all option names.
- `"selected"` (robust, mutable) – the currently selected option name; remote writes trigger the `onTune` callback.

Its tunable type string is `"Selectable"`.

```java
public final class Selectable<V> implements ComplexTunable {
  // adds an option; updates the published options array
  public void add(String name, V object) {...}

  // adds an option and marks it as the default
  public void addDefault(String name, V object) {...}

  // sets the default option name
  public void setDefault(String name) {...}

  // clears all options and the default; the selected option name is unchanged
  public void clear() {...}

  // registers a listener that is called when the selection changes
  public void onChange(Consumer<V> listener) {...}

  // returns the currently selected value, or the default if no selection has been made
  public V getSelected() {...}

  @Override
  public void publishTunable(TunableTable table) {...}
}
```

Example usage for autonomous selection:

```java
Selectable<Command> autoChooser = new Selectable<>();
autoChooser.add("Drive Straight", new DriveStraightCommand(robot));
autoChooser.add("Score Preload", new ScorePreloadCommand(robot));
autoChooser.setDefault("Drive Straight");
Tunables.addComplex("auto", autoChooser);
```

## Backend Overview / Key Features

Backends implement `TunableBackend`.  Publishing a frontend tunable normalizes its path and registers it with the backend selected by `TunableRegistry`.  `TunableRegistry.update()` calls each registered backend so remote writes can be applied, local values can be published, and `onTune` callbacks can run.

`TunableRegistry.registerBackend(String prefix, TunableBackend backend)` associates a backend with a path prefix.  When a tunable is published, the registry selects the backend with the longest matching prefix.  This allows different backends to be used for different subtrees, such as a test backend for `/test/` and an NT backend for `/`.  If a backend is registered after tunables have already been published, any existing tunables whose longest matching prefix now resolves to the newly registered backend are removed from their previous backend and republished to the new backend.

Robot programs normally get a default `NetworkTablesTunableBackend` from `RobotBase`, registered with an empty prefix and a NetworkTables path prefix of `/Tunables`.  Unit tests can use `MockTunableBackend`.

`TunableRegistry` also owns type handlers used by the frontend when `Tunable.createConfig(T initialValue, TunableConfig config)` sees a type that is not automatically handled by Struct or Protobuf serialization.

### TunableRegistry

`TunableRegistry` is the central backend-facing registry.  It is not normally used by robot code directly except in tests or when registering custom backends or type handlers.

```java
public final class TunableRegistry {
  // registers a backend for the given path prefix
  public static void registerBackend(String prefix, TunableBackend backend) {...}

  // registers a type handler for the given class
  public static <T> void registerTypeHandler(Class<T> cls, TypeHandler<T> handler) {...}

  // publishes a tunable (routes to the appropriate backend)
  public static void publish(String path, TunableBase tunable) {...}
  public static void publish(String path, ComplexTunable tunable) {...}

  // removes a tunable
  public static void remove(String path) {...}

  // calls update() on all registered backends
  public static void update() {...}

  // runs a function while holding the same mutex used for update()
  public static void withUpdateMutex(Runnable func) {...}

  // clears type handlers and closes/removes all backends; intended for tests
  public static void reset() {...}

  // gets or creates a TunableTable for the given path
  public static TunableTable getTable(String path) {...}

  // normalizes a name to start with "/" and collapses doubled slashes
  public static String normalizeName(String path) {...}

  // warning reporting (e.g. for type mismatches detected at publish time)
  public static void setReportWarning(Consumer<String> func) {...}
  public static Consumer<String> getReportWarning() {...}
  public static void reportWarning(String msg) {...}
}
```

### TunableBackend

`TunableBackend` is the interface that production and test backends implement.

```java
public interface TunableBackend extends AutoCloseable {
  record PublishedTunable(String path, TunableBase tunable, ComplexTunable complex) {}

  void publish(String path, TunableBase tunable);
  void publishComplex(String path, ComplexTunable tunable);
  void remove(String path);
  List<PublishedTunable> removePrefix(String prefix);

  // called periodically; reads changed values, calls onTune callbacks, etc.
  void update();
}
```

The standard production backend is `NetworkTablesTunableBackend`, registered by `RobotBase` with the NT prefix `/Tunables`.  It publishes non-robust tunables to `/Tunables/<path>` and robust tunables as separate `/Tunables/<path>/value` and `/Tunables/<path>/tune` topics.  The backend calls `tunable.get()` to publish the current value and `tunable.set(value)` to apply a remotely-written value, subject to the mutability flag.

`MockTunableBackend` is provided for unit testing.  It stores all published tunables in a `HashMap` and exposes typed getters (e.g. `getBoolean(path)`, `getInteger(path)`, `getDouble(path)`) so tests can verify that the correct values were published.  External writes can be queued by calling `setBoolean(path, value)`, `setInt(path, value)`, etc., then applied by calling `update()`.

## Java Robot Code Examples

### Simple tunable constants

The most common use case: declaring tunable constants at the point of use with no additional boilerplate.

```java
public class DriveSubsystem {
  // Stored internally; dashboard can read and write this value
  private final TunableDouble kP = Tunables.addDouble("drive/kP", 0.05);
  private final TunableDouble kI = Tunables.addDouble("drive/kI", 0.0);
  private final TunableDouble kD = Tunables.addDouble("drive/kD", 0.001);

  public void periodic() {
    pid.setP(kP.get());
    pid.setI(kI.get());
    pid.setD(kD.get());
    velocity.set(encoder.getRate());
  }
}
```

### Tunable backed by an existing field (getter/setter)

When the value already lives in another object, use the getter/setter form.  The `ALWAYS_GET` option is applied automatically.

```java
private final TunableDouble kPLink = Tunables.getTable("shooter")
    .publishDouble("kP", pid::getP, pid::setP);
```

### onTune callback

React immediately when a value is changed from the dashboard.

```java
private final TunableDouble tolerance;

public ShooterSubsystem() {
  tolerance = TunableDouble.createConfig(
      0.02,
      TunableConfig.of(TunableOption.onTune(this::applyTolerance)));
  Tunables.publish("shooter/tolerance", tolerance);
}

private void applyTolerance() {
  pid.setTolerance(tolerance.get());
}
```

### Struct-serializable type

Types implementing `StructSerializable` are automatically detected; no extra code is needed.

```java
// Pose2d implements StructSerializable; struct serializer is found automatically
private final Tunable<Pose2d> targetPose = Tunables.add("drive/targetPose", new Pose2d());
```

### ComplexTunable implementation

A PID controller that publishes its own gains:

```java
public class TunablePIDController implements ComplexTunable {
  private TunableDouble m_kP, m_kI, m_kD;
  private final PIDController m_pid;

  public TunablePIDController(double kP, double kI, double kD) {
    m_pid = new PIDController(kP, kI, kD);
  }

  @Override
  public String getTunableType() {
    return "PIDController";
  }

  @Override
  public void publishTunable(TunableTable table) {
    m_kP = table.publishDouble("kP", m_pid::getP, m_pid::setP);
    m_kI = table.publishDouble("kI", m_pid::getI, m_pid::setI);
    m_kD = table.publishDouble("kD", m_pid::getD, m_pid::setD);
  }
}
```

Registration at the use site:

```java
private final TunablePIDController armPID =
    Tunables.addComplex("arm/pid", new TunablePIDController(1.0, 0.0, 0.1));
```

### Hierarchical tables

Child tables provide natural grouping for complex subsystems.

```java
TunableTable arm = Tunables.getTable("arm");
TunableTable pivot = arm.getTable("pivot");
TunableDouble pivotSpeed = pivot.publishDouble("maxSpeed", () -> pivotMax, v -> pivotMax = v);
TunableDouble pivotAccel = pivot.publishDouble("maxAccel", () -> pivotAccel, v -> pivotAccel = v);
```

This publishes to normalized paths `/arm/pivot/maxSpeed` and `/arm/pivot/maxAccel`, which the default robot backend places under `/Tunables` in NetworkTables.

## Unit Testing with MockTunableBackend

```java
@BeforeEach
void setUp() {
  TunableRegistry.reset();
  MockTunableBackend backend = new MockTunableBackend();
  TunableRegistry.registerBackend("", backend);
  this.backend = backend;
}

@Test
void testKpPublished() {
  new DriveSubsystem();
  assertEquals(0.05, backend.getDouble("/drive/kP"), 1e-9);
}

@Test
void testKpTunable() {
  var drive = new DriveSubsystem();
  backend.setDouble("/drive/kP", 0.1);
  backend.update();
  assertEquals(0.1, drive.getPID().getP(), 1e-9);
}
```

# Migration from WPILib 2026

Key differences from 2026:

- `SmartDashboard.putNumber("key", value)` / `SmartDashboard.getNumber("key", default)` called every loop is replaced with a single `Tunables.addDouble("key", initialValue)` declaration that returns a `TunableDouble`.  Read it with `tunable.get()` and write it with `tunable.set(value)` (or let the dashboard write it).
- `NetworkTableEntry` / `DoublePublisher` / `DoubleSubscriber` boilerplate is replaced by the same `TunableDouble` pattern; the NT backend handles the underlying NT entry lifecycle.
- `SendableChooser<T>` is replaced by `Selectable<T>`.  The API is similar: `add(name, object)`, `setDefault(name)`, `getSelected()`.
- The `Sendable` interface and `SmartDashboard.putData()` are not part of the Tunable API; subsystems and mechanisms that previously implemented `Sendable` should implement `ComplexTunable` and register via `Tunables.addComplex()`.

# Drawbacks

- Adding a `Tunable` object per-field increases the per-instance memory footprint slightly compared to bare primitive fields.  This is generally acceptable given the number of tunables in a typical robot program.

# Alternatives

- **Direct NetworkTables API** – flexible but verbose and transport-coupled.  Teams already use this; the Tunable API is intended to be a simpler first-class alternative.

- **`Epilogue`-style annotation processor** – generates telemetry code at compile time.  This covers the read-only (log) use case well but does not address the write-back (tune) use case without additional complexity.

- **A single `Sendable` replacement** – a single interface combining logging and tuning.  This was considered but abandoned because the read-only and read-write cases have different semantics (mutability, robustness) that are cleaner to express as separate concerns.

# Trades

- Naming: `Tunable` vs `Adjustable` vs `Parameter`?  `Tunable` was chosen because it clearly communicates that the value can be observed and tuned from external tools.

- Should `Selectable` be renamed `TunableChooser` for consistency with `SendableChooser`?  `Selectable` was chosen to reduce coupling to the `Tunable` naming scheme (it is a `ComplexTunable` rather than a `TunableBase` subclass) and to reflect its broader applicability.

# Appendix A: C++ Tunable API

This appendix documents the C++ API as a C++ user would consume it.

## C++ Overview

- The primary value type is `wpi::Tunable<T>`.
- For common primitive and container types, aliases are provided (`wpi::TunableBool`, `wpi::TunableInt32`, `wpi::TunableInt64`, `wpi::TunableFloat`, `wpi::TunableDouble`, `wpi::TunableString`, `wpi::TunableRaw`, and vector aliases).
- Values are typically read/written using assignment and implicit conversion operators.
- Complex object publishing uses `wpi::ComplexTunable` plus `wpi::TunableTable`.

```cpp
wpi::TunableDouble kP{0.05};
kP = 0.1;
double p = kP;
```

## C++ Core Types

### `wpi::Tunable<T>`

`wpi::Tunable<T>` is a template with compile-time adaptation:

- value storage for value types
- custom adapters via `CustomTunable<T>` or `GetCustomTunable()` ADL
- struct serialization for `StructSerializable` types
- protobuf serialization for `ProtobufSerializable` types

Common aliases:

```cpp
using wpi::TunableBool;
using wpi::TunableInt32;
using wpi::TunableInt64;
using wpi::TunableFloat;
using wpi::TunableDouble;
using wpi::TunableString;
using wpi::TunableRaw;
using wpi::TunableBoolVector;
using wpi::TunableInt32Vector;
using wpi::TunableInt64Vector;
using wpi::TunableFloatVector;
using wpi::TunableDoubleVector;
using wpi::TunableStringVector;
```

### `wpi::TunableConfig`

`wpi::TunableConfig` is a mutable struct, including:

- `properties` (`wpi::util::json` object)
- `robust`
- `typeString`
- `isMutable`
- `onTune` (`std::function<void(wpi::detail::TunableBase&, wpi::ComplexTunable*)>`)
- `parent`
- `alwaysGet`

```cpp
wpi::TunableConfig config;
config.robust = true;
config.isMutable = true;
config.properties["min"] = 0;
config.properties["max"] = 1;
config.alwaysGet = true;
config.onTune = [](wpi::detail::TunableBase& tunable, wpi::ComplexTunable* parent) {
  // Called from wpi::TunableRegistry::Update().
};
```

The callback receives the tuned object and the move-tracked parent pointer.  Do not capture a tunable or its owning `ComplexTunable` by `this` for callbacks that must survive moves; use the callback parameters instead.

### `wpi::Tunables`

Root-level helper entry point.  The root facade mirrors the hierarchical
publishing surface of `wpi::TunableTable`, except it does not expose
`TunableTable::GetPath()`.

```cpp
class Tunables final {
 public:
  static wpi::TunableTable GetTable();
  static wpi::TunableTable GetTable(std::string_view name);

  template <typename T, typename... Args>
  static wpi::Tunable<T> Add(std::string_view name, Args&&... params);

  template <std::derived_from<wpi::ComplexTunable> T, typename... Args>
  static T AddComplex(std::string_view name, Args&&... args);

  static void Publish(std::string_view name, wpi::detail::TunableBase& tunable);

  template <typename T, typename... I>
    requires wpi::detail::IsCustomTunable<T, I...>
  static void Publish(std::string_view name, wpi::Tunable<T, I...>& tunable);

  static void Publish(std::string_view name, wpi::ComplexTunable* tunable,
                      std::unique_ptr<wpi::detail::TunableMemberBase> member);

  template <typename T, std::derived_from<wpi::ComplexTunable> Class, typename... I>
  static void Publish(std::string_view name, Class* tunable, T Class::*member, I&&... info);

  template <typename T, std::derived_from<wpi::ComplexTunable> Class, typename... I>
  static void Publish(std::string_view name, Class* tunable, T Class::*member,
                      const wpi::TunableConfig& config, I&&... info);

  static void Remove(std::string_view name);
};
```

`GetTable(name)`, `Publish()`, and `Remove()` delegate through the root table, so
root-level and child-table paths are normalized consistently.

`AddComplex<T>()` constructs and returns a value; it does not take an existing object parameter.

### `wpi::TunableTable`

Hierarchical publishing API:

```cpp
class TunableTable final {
 public:
  const std::string& GetPath() const;
  TunableTable GetTable(std::string_view name);
  void Publish(std::string_view name, wpi::detail::TunableBase& tunable);

  template <typename T, typename... I>
    requires wpi::detail::IsCustomTunable<T, I...>
  void Publish(std::string_view name, wpi::Tunable<T, I...>& tunable);

  void Publish(std::string_view name, wpi::ComplexTunable* tunable,
               std::unique_ptr<wpi::detail::TunableMemberBase> member);

  template <typename T, std::derived_from<wpi::ComplexTunable> Class, typename... I>
  void Publish(std::string_view name, Class* tunable, T Class::*member, I&&... info);

  template <typename T, std::derived_from<wpi::ComplexTunable> Class, typename... I>
  void Publish(std::string_view name, Class* tunable, T Class::*member,
               const wpi::TunableConfig& config, I&&... info);

  void Remove(std::string_view name);
};
```

Unlike Java, there are no `PublishDouble()` convenience methods; publish typed tunables directly.

### `wpi::TunableRegistry` and `wpi::TunableBackend`

`wpi::TunableRegistry` owns the mapping from normalized paths to registered backends and tracks tunables by UID so moved C++ objects continue to point at the right tunable.

```cpp
class TunableRegistry final {
 public:
  static void RegisterBackend(std::string_view prefix,
                              std::shared_ptr<wpi::TunableBackend> backend);
  static std::shared_ptr<wpi::TunableBackend> GetBackend(std::string_view path);
  static std::string_view NormalizeName(std::string_view path, std::string& buf);
  static void Publish(std::string_view path, wpi::detail::TunableBase& tunable);
  static void Publish(std::string_view path, wpi::ComplexTunable* tunable,
                      std::unique_ptr<wpi::detail::TunableMemberBase> member);
  static void Remove(std::string_view path);
  static void Update();
  static wpi::util::mutex& GetUpdateMutex();
  static void Reset();
};

class TunableBackend {
 public:
  struct PublishedTunable {
    std::string path;
    uint32_t uid;
  };

  virtual void Publish(std::string_view path, uint32_t uid,
                       wpi::detail::TunableBase& tunable,
                       const wpi::TunableConfig* config,
                       wpi::detail::TunableTypeValue type) = 0;
  virtual void Remove(std::string_view path) = 0;
  virtual std::vector<PublishedTunable> RemovePrefix(
      std::string_view prefix) = 0;
  virtual void UnregisterTunable(uint32_t uid) = 0;
  virtual void Update() = 0;
};
```

`RegisterBackend()` takes a `std::shared_ptr<TunableBackend>` and uses longest-prefix matching.  `GetBackend()` throws if no backend matches.  If a backend is registered after tunables have already been published, existing tunables whose longest matching prefix now resolves to the new backend are removed from their previous backend and republished to the new backend.  `RobotBase` calls `TunableRegistry::Update()` from the main loop, but backend registration is separate.

### `wpi::ComplexTunable`

Implement this for composite tunables:

```cpp
class ComplexTunable : public wpi::detail::TunableBase {
 public:
  virtual std::string_view GetTunableType() const { return {}; }
  virtual void PublishTunable(wpi::TunableTable& table) = 0;
  virtual void UpdateTunable() const {}

 protected:
  virtual ~ComplexTunable() = default;
};
```

### `wpi::Selectable<T>`

Chooser-style complex tunable:

```cpp
template <class T>
class Selectable final : public wpi::detail::SelectableBase {
 public:
  void Add(std::string_view name, T object);
  void AddDefault(std::string_view name, T object);
  void SetDefault(std::string_view name);
  void Clear();
  CopyType GetSelected() const;
  void OnChange(std::function<void(T)> listener);
};
```

`Selectable<T>` requires `T` to be copy-constructible and default-initializable.  If `T` is `std::shared_ptr<U>`, `GetSelected()` returns `std::weak_ptr<U>`.  If no selected or default option exists, it returns a value-initialized `CopyType`.

## C++ Usage Examples

### Simple tunable constants

```cpp
class DriveSubsystem {
 public:
  wpi::TunableDouble kP = wpi::Tunables::Add<double>("drive/kP", 0.05);
  wpi::TunableDouble kI = wpi::Tunables::Add<double>("drive/kI", 0.0);
  wpi::TunableDouble kD = wpi::Tunables::Add<double>("drive/kD", 0.001);

  void Periodic() {
    pid.SetP(kP);
    pid.SetI(kI);
    pid.SetD(kD);
  }
};
```

### Configured tunable

```cpp
wpi::TunableConfig config;
config.robust = true;
config.properties["min"] = 0;
config.properties["max"] = 1;

wpi::TunableDouble tolerance{0.02, config};
wpi::Tunables::Publish("shooter/tolerance", tolerance);
```

### Struct-serializable type

```cpp
wpi::Tunable<wpi::Pose2d> targetPose{wpi::Pose2d{}};
wpi::Tunables::Publish("drive/targetPose", targetPose);
```

### Complex tunable with `Tunable` member variables

If the member is already a `Tunable` object, publish it by reference.

```cpp
class TunablePIDController : public wpi::ComplexTunable {
 public:
  TunablePIDController(double p, double i, double d)
      : m_kP{p}, m_kI{i}, m_kD{d} {}

  void PublishTunable(wpi::TunableTable& table) override {
    table.Publish("kP", m_kP);
    table.Publish("kI", m_kI);
    table.Publish("kD", m_kD);
  }

 private:
  wpi::TunableDouble m_kP;
  wpi::TunableDouble m_kI;
  wpi::TunableDouble m_kD;
};

TunablePIDController armPID{1.0, 0.0, 0.1};
wpi::Tunables::Publish("arm/pid", armPID);
```

### Complex tunable with plain member variables

If the member is a plain field rather than a `Tunable`, publish it using the pointer-to-member overload.

```cpp
class TunablePIDController : public wpi::ComplexTunable {
 public:
  TunablePIDController(double p, double i, double d)
      : m_kP{p}, m_kI{i}, m_kD{d} {}

  void PublishTunable(wpi::TunableTable& table) override {
    table.Publish("kP", this, &TunablePIDController::m_kP);
    table.Publish("kI", this, &TunablePIDController::m_kI);
    table.Publish("kD", this, &TunablePIDController::m_kD);
  }

 private:
  double m_kP;
  double m_kI;
  double m_kD;
};

TunablePIDController armPID{1.0, 0.0, 0.1};
wpi::Tunables::Publish("arm/pid", armPID);
```

### Hierarchical tables

```cpp
wpi::TunableTable arm = wpi::Tunables::GetTable("arm");
wpi::TunableTable pivot = arm.GetTable("pivot");

wpi::TunableDouble pivotSpeed{2.0};
wpi::TunableDouble pivotAccel{8.0};

pivot.Publish("maxSpeed", pivotSpeed);
pivot.Publish("maxAccel", pivotAccel);
```

## C++ Unit Testing with `wpi::MockTunableBackend`

```cpp
auto backend = std::make_shared<wpi::MockTunableBackend>();
wpi::TunableRegistry::Reset();
wpi::TunableRegistry::RegisterBackend("", backend);

DriveSubsystem drive;
backend->SetDouble("/drive/kP", 0.1);
wpi::TunableRegistry::Update();

EXPECT_NEAR(drive.GetPID().GetP(), 0.1, 1e-9);
```

`Set*()` methods queue remote writes and apply during `TunableRegistry::Update()`.  The mock backend methods use C++ type names such as `SetBool()`, `SetInt32()`, `SetInt64()`, `SetFloat()`, `SetDouble()`, `SetString()`, `SetRaw()`, and corresponding vector/Struct/Protobuf setters.

## C++ Migration Notes

- Replace direct NT entry/topic boilerplate with `wpi::Tunable<T>` values and `wpi::Tunables` publishing once an appropriate backend is registered.
- For chooser use cases, replace `SendableChooser` patterns with `wpi::Selectable<T>`.
- For composite objects, implement `wpi::ComplexTunable` and make member values `wpi::Tunabl<T>>` or publish non-tunable members with `table.Publish("name", this, &Class::member)`.
- Expect template-based diagnostics for unsupported custom types; provide `CustomTunable<T>` or serialization traits where needed.
