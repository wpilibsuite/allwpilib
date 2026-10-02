# Summary

This document describes a standardized approach for robot program values to be observed and modified from dashboards or debug tools at runtime, without recompilation of the robot program.

A "tunable" is a value that can be read and/or written from an external tool such as a dashboard.

Language-specific API details are split into separate files:

- [C++ API](cpp.md)
- [Python API](python.md)

# Motivation

The ability to monitor and adjust robot program values from a dashboard is a very common need across FTC and FRC teams. Common use cases include:

- Tuning control loop gains (PID constants, feedforward coefficients) without stopping the robot or recompiling code

- Toggling feature flags or operational modes during testing

- Providing a selection control (e.g. autonomous strategy selection, drive mode selection) driven by dashboard input

WPILib 2026's primary mechanism for this was the `SmartDashboard` API, which provided a simplified interface to get and set values. Complex objects implemented the `Sendable` interface that worked with `SmartDashboard.putData()` to refresh data each periodic loop.

Alternatively teams can use the NetworkTables API directly, which requires teams to manually create `NetworkTableEntry` or publisher/subscriber objects at specific NT paths, and to manually call `getDouble()`/`setDouble()` (or equivalent) inside the robot loop. This is flexible but verbose, makes the intent unclear at the declaration site, and tightly couples robot code to the NT API.

# Background (WPILib 2026)

WPILib 2026 shipped `SmartDashboard` and `Shuffleboard`, both of which are thin wrappers around NetworkTables.

There were two options in `SmartDashboard` for tuning values. One option is for teams to call e.g. `SmartDashboard.putNumber("kP", kP)` every loop to publish a value and `SmartDashboard.getNumber("kP", 0)` to read it back. This works but has several drawbacks:

- The pattern conflates "publish a value for observation" with "allow a value to be changed remotely"; there is no explicit mutability flag.
- There is no support for complex objects; Struct/Protobuf objects require manual serialization boilerplate.

`SmartDashboard.putData()` allowed background publishing with use of the `Sendable` interface implemented by the published object. This provided a one-shot approach to users for certain objects, but is not friendly to tuning single values. The `Sendable` interface also did not expose NT configuration; it's necessary to use the `NTSendable` interface for that.

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

- A `Tunable<T>` object wraps a single value. It can be backed by an internal stored value or delegated to a getter/setter pair from existing robot code.

- In Java, primitive-specialized subclasses (`TunableBoolean`, `TunableInt`, `TunableLong`, `TunableFloat`, `TunableDouble`) avoid boxing for the common numeric types.

- `ComplexTunable` is an interface for objects that own multiple related tunables. Implementing classes call `TunableTable` methods inside `publishTunable()` to describe their structure; the backend receives a subtable and populates it accordingly.

- `TunableTable` represents a hierarchical namespace node. Tables can be nested arbitrarily. Tunables are published to a table by name.

- `Tunables` is a static utility class providing convenience factory methods that publish to the root table. It is the primary entry point for simple use cases.

- `TunableConfig` / `TunableOption` carry optional metadata: mutability, robustness, type string, JSON-valued properties, polling behavior (`DEFAULT`, `ALWAYS_GET`, or `GET_ON_CHANGE`), and an `onTune` callback.

- `Selectable<V>` is a `ComplexTunable` that presents a drop-down list of named options and calls an optional listener when the selection changes.

Robot code normally uses the frontend API only. Publishing a tunable registers the value with the backend layer; subsequent calls to `TunableRegistry.update()` allow the backend to apply remote writes and run callbacks.

## Core APIs

### TunableBase

`TunableBase` is the abstract base class for all tunable objects. It stores an optional `TunableConfig`, a changed flag, and whether the tunable can notify backends when `set()` marks it changed. Subclasses that support notification call `markChanged()`; backends clear the flag by calling `resetChanged()` or `TunableRegistry.resetChangedAfterUpdate()`.

```java
public abstract class TunableBase {
  // returns true if the value has changed since the flag was last reset
  public boolean hasChanged() {...}

  // clears the changed flag; generally only called by backends
  public void resetChanged() {...}

  // returns true if markChanged() notifies backends when the value changes
  public boolean supportsChangeNotification() {...}

  // returns the configuration for this tunable
  public TunableConfig getConfig() {...}

  // returns true if this tunable is configured as robust (echoes value back after setting)
  public boolean isRobust() {...}

  // returns the properties JSON string
  public String getProperties() {...}
}
```

### Tunable\<T\>

`Tunable<T>` is a generic tunable for a simple value type. It extends `TunableBase` and implements both `Supplier<T>` and `Consumer<T>`.

The preferred creation patterns are:

- **From a non-null initial value** – the value is stored internally. For types that implement `StructSerializable` or `ProtobufSerializable`, the appropriate serializer is detected automatically via reflection. Arrays whose component type implements `StructSerializable` are stored as struct arrays.
- **From a getter/setter pair** – the tunable delegates to existing state. The getter and setter must not throw. If the config is omitted or leaves polling as `DEFAULT`, `ALWAYS_GET` is set automatically so the backend reads the current value every update cycle. Use `GET_ON_CHANGE` when the owner marks the tunable changed after local writes.
- **Null initial value** – useful when a value is not yet known at construction time; use `createNull()` or an explicit Struct/Protobuf serializer.

Struct and Protobuf serializers can also be supplied explicitly to override automatic detection.
Basic generic tunables are limited to the simple backend-supported types (`Boolean`, `Integer`, `Long`, `Float`, `Double`, `String`, `byte[]`, `boolean[]`, `int[]`, `long[]`, `float[]`, `double[]`, and `String[]`). Other object types should use Struct/Protobuf serialization or a registered type handler that exposes a supported inner tunable; otherwise, construction reports a tunable warning and backends may skip publication.

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

  // marks changed and returns the stored/current value for in-place mutation
  public T mutate() {...}

  // Consumer<T> delegation
  @Override
  public void accept(T value) { set(value); }

  public abstract Class<T> getTypeClass() {...}
}
```

Use `mutate()` when changing a mutable value in place, such as a protobuf object or a mutable object inside a struct array. Directly mutating the object returned by `get()` can bypass `set()` and will not mark the tunable changed.

### Primitive Tunable Subclasses (Java only)

In Java, to avoid boxing, primitive-specialized subclasses are provided. Each follows the same factory pattern as `Tunable<T>`: `create()` variants for internal storage or getter/setter delegation, a no-argument `create()` using the type's zero/false default, and `createConfig()` variants that accept a `TunableConfig`.

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

`ComplexTunable` is an interface for objects whose tunable state is composed of multiple sub-values. Implementing classes receive a `TunableTable` and are responsible for publishing their current sub-entries in `publishTunable()`. An optional `getTunableType()` can be overridden to indicate to dashboards what type of widget to display. `updateTunable()` is called during backend updates and may be overridden to push new computed values into the published entries. `publishTunable()` and `updateTunable()` must not throw; if they do, registry and backend state is not guaranteed to be restored.

Complex tunables may also update their published child set after the initial publish. `publishChildTunable()` publishes a new child under every current path for the complex object, `removeChildTunable()` removes that child from every current path, and `setChildTunableChanged()` marks a plain-member child changed when the plain member was mutated outside of its wrapper. `publishTunable()` should still publish the complete current child set so backend migration and full republish operations can recreate it.

```java
public interface ComplexTunable {
  // optional type hint for dashboards; null by default
  default String getTunableType() { return null; }

  // called once when the object is published; implementations should
  // call table.publish*() to register their sub-entries
  void publishTunable(TunableTable table);

  // called every update cycle; may be overridden to push new values
  default void updateTunable() {}

  default void publishChildTunable(String name, TunableBase tunable) {...}
  default void publishChildTunable(String name, ComplexTunable tunable) {...}
  default void removeChildTunable(String name) {...}
  default void setChildTunableChanged(String name) {...}
}
```

### TunableTable

`TunableTable` represents a node in the hierarchical tunable namespace. Tables are obtained via `Tunables.getTable(name)` or by calling `getTable(name)` on an existing table. `TunableTable` objects are cached by path in `TunableRegistry` so the same path always returns the same instance.

```java
public final class TunableTable {
  // returns the path of this table (with trailing "/")
  public String getPath() {...}

  // gets a child table
  public TunableTable getTable(String name) {...}

  // publishes a TunableBase under this table; returns true if accepted
  public boolean publish(String name, TunableBase tunable) {...}

  // publishes a ComplexTunable under this table; returns true if accepted
  public boolean publish(String name, ComplexTunable tunable) {...}

  // getter/setter publishing methods
  public <T> Tunable<T> publishValue(String name, Supplier<T> getter, Consumer<T> setter, Class<T> cls) {...}
  public <T> Tunable<T> publishValue(String name, Supplier<T> getter, Consumer<T> setter,
                                     Class<T> cls, TunableConfig config) {...}
  public TunableBoolean publishBoolean(String name, BooleanSupplier getter, BooleanConsumer setter) {...}
  public TunableBoolean publishBoolean(String name, BooleanSupplier getter,
                                       BooleanConsumer setter, TunableConfig config) {...}
  public TunableInt     publishInt(String name, IntSupplier getter, IntConsumer setter) {...}
  public TunableInt     publishInt(String name, IntSupplier getter, IntConsumer setter,
                                   TunableConfig config) {...}
  public TunableLong    publishLong(String name, LongSupplier getter, LongConsumer setter) {...}
  public TunableLong    publishLong(String name, LongSupplier getter, LongConsumer setter,
                                    TunableConfig config) {...}
  public TunableFloat   publishFloat(String name, FloatSupplier getter, FloatConsumer setter) {...}
  public TunableFloat   publishFloat(String name, FloatSupplier getter, FloatConsumer setter,
                                     TunableConfig config) {...}
  public TunableDouble  publishDouble(String name, DoubleSupplier getter, DoubleConsumer setter) {...}
  public TunableDouble  publishDouble(String name, DoubleSupplier getter, DoubleConsumer setter,
                                      TunableConfig config) {...}

  // removes a tunable from this table
  public void remove(String name) {...}
}
```

Direct `publish()` methods return `true` when the backend accepts the publication and `false` when it rejects it, such as for a duplicate path.

### Tunables

`Tunables` is a static utility class that operates on the root table (`"/"`). It is the primary entry point for simple use cases where hierarchy is not needed.

```java
public final class Tunables {
  // returns the root TunableTable
  public static TunableTable getTable() {...}

  // returns a named child of the root table
  public static TunableTable getTable(String name) {...}

  // creates, publishes, and returns a Tunable<T> at the root
  public static <T> Tunable<T> addValue(String name, T initialValue) {...}

  // primitive-specific variants
  public static TunableBoolean addBoolean(String name, boolean initialValue) {...}
  public static TunableInt     addInt(String name, int initialValue) {...}
  public static TunableLong    addLong(String name, long initialValue) {...}
  public static TunableFloat   addFloat(String name, float initialValue) {...}
  public static TunableDouble  addDouble(String name, double initialValue) {...}

  // lower-level publish at the root table (does not create the tunable)
  public static boolean publish(String name, TunableBase tunable) {...}
  public static boolean publish(String name, ComplexTunable tunable) {...}

  // getter/setter-backed publishing at the root table
  public static <T> Tunable<T> publishValue(String name, Supplier<T> getter,
                                            Consumer<T> setter, Class<T> cls) {...}
  public static <T> Tunable<T> publishValue(String name, Supplier<T> getter,
                                            Consumer<T> setter, Class<T> cls,
                                            TunableConfig config) {...}
  public static TunableBoolean publishBoolean(String name, BooleanSupplier getter, BooleanConsumer setter) {...}
  public static TunableBoolean publishBoolean(String name, BooleanSupplier getter,
                                              BooleanConsumer setter, TunableConfig config) {...}
  public static TunableInt     publishInt(String name, IntSupplier getter, IntConsumer setter) {...}
  public static TunableInt     publishInt(String name, IntSupplier getter, IntConsumer setter,
                                          TunableConfig config) {...}
  public static TunableLong    publishLong(String name, LongSupplier getter, LongConsumer setter) {...}
  public static TunableLong    publishLong(String name, LongSupplier getter, LongConsumer setter,
                                           TunableConfig config) {...}
  public static TunableFloat   publishFloat(String name, FloatSupplier getter, FloatConsumer setter) {...}
  public static TunableFloat   publishFloat(String name, FloatSupplier getter, FloatConsumer setter,
                                            TunableConfig config) {...}
  public static TunableDouble  publishDouble(String name, DoubleSupplier getter, DoubleConsumer setter) {...}
  public static TunableDouble  publishDouble(String name, DoubleSupplier getter,
                                             DoubleConsumer setter, TunableConfig config) {...}

  // removes a tunable from the root table
  public static void remove(String name) {...}
}
```

### TunableConfig and TunableOption

`TunableConfig` carries optional metadata for a tunables. It is immutable; wither-style methods return new instances. The most common way to create one is `TunableConfig.of(TunableOption...)`.

`TunableOption` is a sealed interface whose records encode each possible option. `Property` values are strings containing valid JSON values, such as `"0"`, `"true"`, or `"\"label\""`.

| Option                                           | Meaning                                                                                                                                                                                                                                                                                                                     |
| ------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `ROBUST` / `NOT_ROBUST`                          | When robust, the backend echoes the value back after a remote set, confirming delivery.                                                                                                                                                                                                                                     |
| `MUTABLE` / `IMMUTABLE`                          | When immutable, the backend never calls `set()` in response to remote writes; the value is read-only from the dashboard's perspective.                                                                                                                                                                                      |
| `Polling.DEFAULT`, `ALWAYS_GET`, `GET_ON_CHANGE` | Controls when the backend reads `get()`. `DEFAULT` lets the framework choose; `ALWAYS_GET` polls every update cycle; `GET_ON_CHANGE` only polls when the tunable is marked changed. `ALWAYS_GET` is automatically applied to getter/setter-backed tunables when the supplied config is null or leaves polling as `DEFAULT`. |
| `Property(key, value)`                           | JSON-valued property passed to the backend (e.g. min/max hints for a slider widget).                                                                                                                                                                                                                                        |
| `TypeString(typeString)`                         | Overrides the type string used by the backend for this entry.                                                                                                                                                                                                                                                               |
| `OnTune(callback)`                               | A non-throwing `Runnable` invoked by the backend after a remote write is applied.                                                                                                                                                                                                                                           |

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

`Selectable<V>` is a `ComplexTunable` that provides the equivalent of `SendableChooser` from WPILib 2026. It maintains a map of `String` → `V` and publishes three sub-entries to its table:

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

  // removes an option; clears the default if the removed option was the default
  public void remove(String name) {...}

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
autoChooser.addDefault("Drive Straight", new DriveStraightCommand(robot));
autoChooser.add("Score Preload", new ScorePreloadCommand(robot));
autoChooser.onChange(command -> System.out.println("Auto selected: " + command.getName()));
Tunables.publish("auto", autoChooser);
```

The selected value is read from robot code at the point of use:

```java
public Command getAutonomousCommand() {
  return autoChooser.getSelected();
}
```

## Thread Safety and Secondary Threads

Tunable values, `ComplexTunable` implementations, and `Selectable` are not internally thread-safe. The normal usage model is to access them only from the main robot thread. `RobotBase` calls `TunableRegistry.update()` once per loop; that call applies remote writes, refreshes complex or getter-backed values, updates backends, resets change flags, and runs `onTune` callbacks while holding the registry's recursive update mutex.

The mutex is held only for `update()` and registry operations. It is not held around the rest of the robot loop, including user `periodic()` methods. Consequently, wrapping only a worker-thread access does not make a tunable safe if main-loop code accesses the same object without synchronization.

For occasional direct access from a secondary thread, use the registry update mutex. When choosing this model, every competing access must use the mutex. This includes reads, writes, in-place mutation, `Selectable` changes, and access to state used by a getter, setter, `updateTunable()`, or callback unless that state provides its own safe handoff. Batch related operations in one short critical section:

```java
final double[] result = new double[1];

TunableRegistry.withUpdateMutex(
    () -> {
      driveGain.set(0.08);
      result[0] = driveGain.get();
    });
```

Main-loop code that reads `driveGain` must use `withUpdateMutex()` as well. The method returns `void`, so a value that must leave the critical section should be copied into caller-owned storage as shown above. Do not allow a mutable object returned by `get()` or `mutate()` to escape the critical section and then modify it without synchronization.

The update mutex is global and is held while backend work, getters, setters, complex updates, and callbacks run. Code under the mutex should not wait for I/O, join another thread, or do other lengthy work. Be careful with application locks: if `update()` calls a getter or callback that waits for an application lock while another thread holds that lock and waits for the update mutex, the program deadlocks. Either always acquire the update mutex first or use a handoff design that does not nest the locks.

For values exchanged frequently with a worker thread, prefer thread-safe application state instead of taking the global update mutex in the worker hot path:

- Store a scalar in an atomic, or store a compound configuration as one immutable snapshot in an atomic reference. A getter/setter-backed tunable can exchange that snapshot during `update()`.

- Queue worker-originated changes for the main loop to apply, and publish main-loop changes back to workers through an atomic or immutable snapshot. This keeps callbacks on the main thread and introduces at most one loop of handoff latency.

For example, Java can back a published value with an atomic immutable snapshot and have worker code use the snapshot rather than the `TunableDouble` itself:

```java
record DriveConfig(double kP) {}

AtomicReference<DriveConfig> driveConfig =
    new AtomicReference<>(new DriveConfig(0.05));
Tunables.publishDouble(
    "drive/kP",
    () -> driveConfig.get().kP(),
    value -> driveConfig.set(new DriveConfig(value)));
```

The getter and setter are invoked during `update()` while the update mutex is held, but any state they access must still provide its own safe handoff, as `AtomicReference` does here. A worker reads all fields from one `driveConfig.get()` result. `Polling.ALWAYS_GET` only controls when a backend reads a value, and `isMutable=false` only rejects remote writes; neither option provides thread safety.

Callbacks run on the thread that calls `TunableRegistry.update()`, normally the main robot thread. Calling `update()` from a worker is not a substitute for synchronizing tunable access: concurrent updates serialize, but callbacks then run on whichever thread performed that update.

## Backend Overview / Key Features

Backends implement `TunableBackend`. Publishing a frontend tunable normalizes its path and registers it with the backend selected by `TunableRegistry`. `TunableRegistry.update()` calls each registered backend so remote writes can be applied, local values can be published, changed flags can be reset, complex tunables can update, and `onTune` callbacks can run after the update cycle has finished applying writes.

`TunableRegistry.registerBackend(String prefix, TunableBackend backend)` associates a backend with a path prefix. When a tunable is published, the registry selects the backend with the longest matching prefix. This allows different backends to be used for different subtrees, such as a test backend for `/test/` and an NT backend for `/`. If a backend is registered after tunables have already been published, existing tunables whose longest matching prefix now resolves to a different backend are removed from their previous backend with `removePrefix()` and republished to the newly selected backend. Java complex tunables are republished parent-first, and child entries recreated as a side effect of `publishComplex()` are suppressed from the remaining migration list so they are not published twice.

Robot programs normally get a default `NetworkTablesTunableBackend` from `RobotBase`, registered with an empty prefix and a NetworkTables path prefix of `/Tunables`. Unit tests can use `MockTunableBackend`.

`TunableRegistry` also owns type handlers used by the frontend when `Tunable.createConfig(T initialValue, TunableConfig config)` sees a type that is not automatically handled by Struct or Protobuf serialization.

### TunableRegistry

`TunableRegistry` is the central backend-facing registry. It is not normally used by robot code directly except in tests or when registering custom backends or type handlers.

```java
public final class TunableRegistry {
  // registers a backend for the given path prefix
  public static void registerBackend(String prefix, TunableBackend backend) {...}

  // registers a type handler for the given class
  public static <T> void registerTypeHandler(Class<T> cls, TypeHandler<T> handler) {...}

  // publishes a tunable (routes to the appropriate backend)
  public static boolean publish(String path, TunableBase tunable) {...}
  public static boolean publish(String path, ComplexTunable tunable) {...}

  // removes a tunable
  public static void remove(String path) {...}

  // removes every tracked publication for a complex tunable
  public static void remove(ComplexTunable tunable) {...}

  // calls update() on all registered backends
  public static void update() {...}

  // backend helpers for update cycle ordering
  public static void resetChangedAfterUpdate(TunableBase tunable) {...}
  public static void runAfterUpdate(Runnable callback) {...}
  public static void updateComplexIfNeeded(ComplexTunable tunable) {...}

  // runs a function while holding the same mutex used for update()
  public static void withUpdateMutex(Runnable func) {...}

  // clears type handlers and closes/removes all backends; intended for tests
  public static void reset() {...}

  // gets or creates a TunableTable for the given path
  public static TunableTable getTable(String path) {...}

  // warning reporting (e.g. for type mismatches detected at publish time)
  public static void setReportWarning(Consumer<String> func) {...}
  public static Consumer<String> getReportWarning() {...}
  public static void reportWarning(String msg) {...}
}
```

### TunableBackend

`TunableBackend` is the interface that production and test backends implement. Backend publication, removal, dirty marking, and update methods must not throw; recoverable failures should be reported as tunable warnings and skipped so the robot loop and registry state can continue updating.

```java
public interface TunableBackend extends AutoCloseable {
  record PublishedTunable(String path, TunableBase tunable, ComplexTunable complex) {}

  boolean publish(String path, TunableBase tunable);
  boolean publishComplex(String path, ComplexTunable tunable);
  void remove(String path);
  List<PublishedTunable> removePrefix(String prefix);
  default void markDirty(TunableBase tunable) {}

  // called periodically; reads changed values, calls onTune callbacks, etc.
  void update();
}
```

The standard production backend is `NetworkTablesTunableBackend`, registered by `RobotBase` with the NT prefix `/Tunables`. It publishes non-robust tunables to `/Tunables/<path>` and robust tunables as separate `/Tunables/<path>/value` and `/Tunables/<path>/tune` topics. The backend calls `tunables.get()` to publish the current value and `tunables.set(value)` to apply a remotely-written value, subject to the mutability flag.

Custom backend implementations can use `org.wpilib.tunable.util.PathUtil` for path normalization and descendant checks. `normalizePrefix()` preserves a trailing slash for `removePrefix()` descendant-only matching; registry backend registration uses its own normalization so `/foo` and `/foo/` register the same backend prefix.

`markDirty()` is called when a tunable with change-notification support is modified locally; backends can use this to avoid polling unchanged `GET_ON_CHANGE` values. Backends should call `resetChangedAfterUpdate()` after publishing a changed value so all registered backends can observe the changed flag during the same registry update, and should use `runAfterUpdate()` for non-throwing callbacks that react to tuned values.

Warning handlers installed with `setReportWarning()` must not throw.

`MockTunableBackend` is provided for unit testing. It stores all published tunables in a `HashMap` and exposes typed getters (e.g. `getBoolean(path)`, `getInteger(path)`, `getDouble(path)`, `getValue(path, cls)`) so tests can verify that the correct values were published. External writes can be queued by calling `setBoolean(path, value)`, `setInt(path, value)`, `setArray(path, value)`, `setValue(path, value)`, etc., then applied by calling `update()`.

## Java Robot Code Examples

### Simple tunable constants

The most common use case: declaring tunable constants at the point of use with no additional boilerplate.

```java
public class IntakeSubsystem {
  // Stored internally; dashboard can read and write these values
  private final TunableDouble intakeSpeed =
      Tunables.addDouble("intake/speed", 0.65);
  private final TunableDouble outtakeSpeed =
      Tunables.addDouble("intake/outtakeSpeed", -0.5);

  public void intake() {
    motor.set(intakeSpeed.get());
  }

  public void outtake() {
    motor.set(outtakeSpeed.get());
  }
}
```

### Tunable backed by an existing field (getter/setter)

When the value already lives in subsystem state or another object, use the getter/setter form. The `ALWAYS_GET` option is applied automatically. If the owning object marks the tunable changed after local writes, pass `GET_ON_CHANGE` to avoid polling every update cycle.

```java
private final TunableDouble maxOutputLink = Tunables.getTable("drive")
    .publishDouble("maxOutput", this::getMaxOutput, this::setMaxOutput);
```

This is useful when existing subsystem state should remain the source of truth, or when the setter should validate and clamp dashboard input:

```java
public final class DriveSubsystem extends SubsystemBase {
  private double m_maxOutput = 0.8;

  public DriveSubsystem() {
    Tunables.getTable("drive")
        .publishDouble("maxOutput", this::getMaxOutput, this::setMaxOutput);
  }

  public void arcadeDrive(double forward, double rotation) {
    m_drive.arcadeDrive(forward * m_maxOutput, rotation * m_maxOutput);
  }

  public double getMaxOutput() {
    return m_maxOutput;
  }

  public void setMaxOutput(double value) {
    m_maxOutput = MathUtil.clamp(value, 0.0, 1.0);
  }
}
```

### onTune callback

React immediately when a value is changed from the dashboard.

```java
private final TunableDouble deadband;

public DriveSubsystem() {
  deadband = TunableDouble.createConfig(
      0.02,
      TunableConfig.of(TunableOption.onTune(this::applyDeadband)));
  Tunables.publish("drive/deadband", deadband);
}

private void applyDeadband() {
  m_drive.setDeadband(deadband.get());
}
```

### Struct-serializable type

Types implementing `StructSerializable` are automatically detected; no extra code is needed.

```java
// Pose2d implements StructSerializable; struct serializer is found automatically
private final Tunable<Pose2d> targetPose = Tunables.addValue("drive/targetPose", new Pose2d());
```

### Tunable Field2d dashboard edits

`Field2d` implements `ComplexTunable`, so it can be published through `Tunables` when dashboard edits should flow back into robot code. Publish it once, then read edited object poses from the same `Field2d` instance during the robot loop.

```java
public final class Robot extends TimedRobot {
  private final Field2d m_field = new Field2d();
  private final FieldObject2d m_target = m_field.getObject("Target");
  private Pose2d m_driveTargetPose = Pose2d.ZERO;

  @Override
  public void robotInit() {
    Tunables.publish("Field", m_field);
  }

  @Override
  public void robotPeriodic() {
    m_field.setRobotPose(m_poseEstimator.getEstimatedPosition());

    m_driveTargetPose = m_target.getPose();
  }
}
```

Use `Tunables.publish("Field", m_field)` when the dashboard should be able to move field objects and robot code should observe those edits.

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

  public double calculate(double measurement, double setpoint) {
    return m_pid.calculate(measurement, setpoint);
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
public final class ArmSubsystem extends SubsystemBase {
  private final TunablePIDController m_armPid =
      new TunablePIDController(1.0, 0.0, 0.1);

  public ArmSubsystem() {
    Tunables.publish("arm/pid", m_armPid);
  }

  @Override
  public void periodic() {
    double output =
        m_armPid.calculate(m_encoder.getDistance(), m_goalRadians);
    m_motor.setVoltage(output);
  }
}
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

### Autonomous command selection

Use `Selectable<Command>` for the same autonomous-chooser workflow teams previously used with `SendableChooser`.

```java
public final class RobotContainer {
  private final DriveSubsystem m_drive = new DriveSubsystem();
  private final Selectable<Command> m_autoChooser = new Selectable<>();
  private String m_selectedAutoName = "Leave Community";

  public RobotContainer() {
    m_autoChooser.addDefault("Leave Community", new DriveDistance(m_drive, 3.0));
    m_autoChooser.add("Score And Leave", new ScoreAndLeave(m_drive));
    m_autoChooser.add("Do Nothing", Commands.none());
    m_autoChooser.onChange(command -> m_selectedAutoName = command.getName());

    Tunables.publish("Auto", m_autoChooser);
  }

  public Command getAutonomousCommand() {
    return m_autoChooser.getSelected();
  }
}
```

The selectable publishes `/Auto/default`, `/Auto/options`, and `/Auto/selected` under the Tunable backend's prefix. A dashboard changes only the selected option name; robot code still owns the actual command objects.

### Mode or configuration selection

`Selectable` is not limited to commands. It can hold enums, records, subsystem modes, or immutable configuration objects.

```java
enum DriveMode {
  FIELD_RELATIVE,
  ROBOT_RELATIVE,
  PRECISION
}

public final class DriveSubsystem extends SubsystemBase {
  private final Selectable<DriveMode> m_driveMode = new Selectable<>();
  private DriveMode m_currentMode = DriveMode.FIELD_RELATIVE;

  public DriveSubsystem() {
    m_driveMode.addDefault("Field Relative", DriveMode.FIELD_RELATIVE);
    m_driveMode.add("Robot Relative", DriveMode.ROBOT_RELATIVE);
    m_driveMode.add("Precision", DriveMode.PRECISION);
    m_driveMode.onChange(this::setDriveMode);

    Tunables.publish("Drive/mode", m_driveMode);
  }

  @Override
  public void periodic() {
    setDriveMode(m_driveMode.getSelected());
  }

  private void setDriveMode(DriveMode mode) {
    m_currentMode = mode;
  }
}
```

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
void testMaxOutputPublished() {
  new DriveSubsystem();
  assertEquals(0.8, backend.getDouble("/drive/maxOutput"), 1e-9);
}

@Test
void testMaxOutputTunable() {
  var drive = new DriveSubsystem();
  backend.setDouble("/drive/maxOutput", 0.5);
  TunableRegistry.update();
  assertEquals(0.5, drive.getMaxOutput(), 1e-9);
}
```

# Migration from WPILib 2026

Key differences from 2026:

- `SmartDashboard.putNumber("key", value)` / `SmartDashboard.getNumber("key", default)` called every loop is replaced with a single `Tunables.addDouble("key", initialValue)` declaration that returns a `TunableDouble`. Read it with `tunables.get()` and write it with `tunables.set(value)` (or let the dashboard write it).
- `NetworkTableEntry` / `DoublePublisher` / `DoubleSubscriber` boilerplate is replaced by the same `TunableDouble` pattern; the NT backend handles the underlying NT entry lifecycle.
- `SendableChooser<T>` is replaced by `Selectable<T>`. The API is similar: `add(name, object)`, `addDefault(name, object)`, `getSelected()`.
- The `Sendable` interface and `SmartDashboard.putData()` are not part of the Tunable API; subsystems and mechanisms that previously implemented `Sendable` should implement `ComplexTunable` and register via `Tunables.publish()`.

## SmartDashboard Tuning to Tunable

Use Tunable when the dashboard is allowed to change the value and robot code reads that value back.

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

Getter/setter-backed tunables are a closer match when existing robot code already owns the value or validates assignments:

**Was (WPILib 2026):**

```java
public void robotInit() {
  SmartDashboard.putNumber(
      "Drive/maxOutput", m_drive.getMaxOutput());
}

public void robotPeriodic() {
  double maxOutput = SmartDashboard.getNumber(
      "Drive/maxOutput", m_drive.getMaxOutput());
  m_drive.setMaxOutput(maxOutput);
}
```

**Is (Tunable getter/setter):**

```java
public void robotInit() {
  Tunables.publishDouble(
      "Drive/maxOutput",
      m_drive::getMaxOutput,
      m_drive::setMaxOutput);
}
```

## Editable Field2d to Tunable

**Was (WPILib 2026):**

```java
private final Field2d m_field = new Field2d();
private final FieldObject2d m_target = m_field.getObject("Target");
private Pose2d m_driveTargetPose = Pose2d.ZERO;

public void robotInit() {
  SmartDashboard.putData("Field", m_field);
}

public void robotPeriodic() {
  m_field.setRobotPose(m_poseEstimator.getEstimatedPosition());
  m_driveTargetPose = m_target.getPose();
}
```

**Is (Tunable):**

```java
private final Field2d m_field = new Field2d();
private final FieldObject2d m_target = m_field.getObject("Target");
private Pose2d m_driveTargetPose = Pose2d.ZERO;

public void robotInit() {
  Tunables.publish("Field", m_field);
}

public void robotPeriodic() {
  m_field.setRobotPose(m_poseEstimator.getEstimatedPosition());
  m_driveTargetPose = m_target.getPose();
}
```

## SendableChooser to Selectable

`Selectable` publishes the chooser data through the Tunable backend and returns the selected robot-owned object from `getSelected()`.

**Was (WPILib 2026):**

```java
private final SendableChooser<Command> m_autoChooser =
    new SendableChooser<>();

public RobotContainer() {
  m_autoChooser.setDefaultOption(
      "Leave Community", new DriveDistance(m_drive, 3.0));
  m_autoChooser.addOption("Score And Leave", new ScoreAndLeave(m_drive));
  SmartDashboard.putData("Auto", m_autoChooser);
}

public Command getAutonomousCommand() {
  return m_autoChooser.getSelected();
}
```

**Is (Selectable):**

```java
private final Selectable<Command> m_autoChooser =
    new Selectable<>();

public RobotContainer() {
  m_autoChooser.addDefault(
      "Leave Community", new DriveDistance(m_drive, 3.0));
  m_autoChooser.add("Score And Leave", new ScoreAndLeave(m_drive));
  Tunables.publish("Auto", m_autoChooser);
}

public Command getAutonomousCommand() {
  return m_autoChooser.getSelected();
}
```

The same pattern works for non-command options:

**Was (WPILib 2026):**

```java
private final SendableChooser<String> m_driveModeChooser =
    new SendableChooser<>();

public RobotContainer() {
  m_driveModeChooser.setDefaultOption("Field Relative", "field");
  m_driveModeChooser.addOption("Robot Relative", "robot");
  SmartDashboard.putData("Drive Mode", m_driveModeChooser);
}

public void teleopPeriodic() {
  m_drive.setMode(m_driveModeChooser.getSelected());
}
```

**Is (Selectable):**

```java
private final Selectable<DriveMode> m_driveMode =
    new Selectable<>();

public RobotContainer() {
  m_driveMode.addDefault("Field Relative", DriveMode.FIELD_RELATIVE);
  m_driveMode.add("Robot Relative", DriveMode.ROBOT_RELATIVE);
  Tunables.publish("Drive/mode", m_driveMode);
}

public void teleopPeriodic() {
  m_drive.setMode(m_driveMode.getSelected());
}
```

# Drawbacks

- Adding a `Tunable` object per-field increases the per-instance memory footprint slightly compared to bare primitive fields. This is generally acceptable given the number of tunables in a typical robot program.

# Alternatives

- **Direct NetworkTables API** – flexible but verbose and transport-coupled. Teams already use this; the Tunable API is intended to be a simpler first-class alternative.

- **`Epilogue`-style annotation processor** – generates telemetry code at compile time. This covers the read-only (log) use case well but does not address the write-back (tune) use case without additional complexity.

- **A single `Sendable` replacement** – a single interface combining logging and tuning. This was considered but abandoned because the read-only and read-write cases have different semantics (mutability, robustness) that are cleaner to express as separate concerns.

# Trades

- Naming: `Tunable` vs `Adjustable` vs `Parameter`? `Tunable` was chosen because it clearly communicates that the value can be observed and tuned from external tools.

- Should `Selectable` be renamed `TunableChooser` for consistency with `SendableChooser`? `Selectable` was chosen to reduce coupling to the `Tunable` naming scheme (it is a `ComplexTunable` rather than a `TunableBase` subclass) and to reflect its broader applicability.
