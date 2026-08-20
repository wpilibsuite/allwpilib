# Tunables Python API

This file describes the Python tunables API exposed by the `tunables` package. It follows the same registry/backend model as C++, but presents a Python-owned `Tunable` wrapper, snake_case methods, and Python `ComplexTunable` / `Selectable` classes.

- For the shared Tunable design and Java-facing API, see [Tunable design](tunables.md).
- For C++-specific behavior, see [C++ API](cpp.md).

## Python Overview

The primary entry points are:

- `tunables.Tunable`, a Python value wrapper with `get()` and `set()`
- root-level functions such as `tunables.add()` and `tunables.publish()`
- `tunables.TunableTable`, a hierarchical publisher
- `tunables.ComplexTunable`, a Python ABC for objects that publish child tunables
- `tunables.Selectable`, a pure-Python chooser-style complex tunable
- `tunables.TunableRegistry`, for backend registration, updates, warning hooks, and tests

```py
import tunables

k_p = tunables.add_double("drive/kP", 0.05)
k_p.set(0.07)
value = k_p.get()

drive = tunables.get_table("drive")
max_speed = drive.add_double("maxSpeed", 3.0)
```

Python-created tunables are backed by native C++ tunables and retained by normalized publish path. `remove()` releases that retained storage for the removed publish path; complex tunable removal also releases retained child objects.

## `tunables.Tunable`

`Tunable` can infer common Python value types. Use `value_type` to select a scalar Python type explicitly, or `element_type` to select the element type for arrays.

```py
class Tunable:
    def __init__(self, value, *, getter=None, setter=None, on_tune=None,
                 robust=False, mutable=True, value_type=None,
                 element_type=None, properties=None, type_string="",
                 always_get=False): ...
    def get(self): ...
    def set(self, value): ...
    def mutate(self): ...
```

Supported value kinds are:

- `bool`
- `int`, stored as a 64-bit integer tunable
- `float`, stored as a double tunable
- `str`
- `bytes`, `bytearray`, or `memoryview`, stored as raw bytes
- homogeneous sequences of bools, ints, numbers, or strings
- `WPIStruct` objects created with `wpiutil.wpistruct`
- non-empty sequences of a single `WPIStruct` type

Explicit type selectors are Python type objects, not string names. `value_type` is for scalar values and accepts `bool`, `int`, `float`, `str`, `bytes`, `bytearray`, or a WPIStruct class. `element_type` is for sequences and accepts `bool`, `int`, `float`, `str`, or a WPIStruct class. `value_type` and `element_type` are mutually exclusive.

Empty sequences require `element_type` because Python lists do not carry an element type. Existing array tunables can later be set to an empty list.

```py
enabled = tunables.Tunable(False, value_type=bool)
names = tunables.Tunable([], element_type=str, mutable=False)
points = tunables.Tunable([], element_type=TunablePoint)  # a WPIStruct class
```

`properties` is converted from normal Python JSON-like values (`None`, bools, numbers, strings, lists, and dicts) into the backend property JSON. `type_string` overrides the backend type string. `mutable=False` prevents remote writes from calling the setter. `robust=True` requests the robust backend publication form. `on_tune`, when supplied, is called with the tuned value after a backend applies a remote write during `TunableRegistry.update()`. Getter, setter, and `on_tune` callables must not raise.

Use `mutate()` when changing a mutable value in place, such as a WPIStruct object or a WPIStruct object contained in a struct array. Directly mutating the object returned by `get()` can bypass `set()` and will not mark the tunable changed.

## Python Tables and Root Facade

`TunableTable` exposes:

```py
table.path
table.get_table(name)
table.publish(name, tunable_or_complex) -> bool
table.add(name, value, *, value_type=None, element_type=None,
          robust=False, mutable=True, on_tune=None, properties=None,
          type_string="")
table.add_boolean(name, value, ...)
table.add_int(name, value, ...)
table.add_long(name, value, ...)
table.add_float(name, value, ...)
table.add_double(name, value, ...)
table.publish_value(name, getter, setter, *, value_type=None,
                    element_type=None, robust=False, mutable=True,
                    properties=None, type_string="")
table.publish_boolean(name, getter, setter, ...)
table.publish_int(name, getter, setter, ...)
table.publish_long(name, getter, setter, ...)
table.publish_float(name, getter, setter, ...)
table.publish_double(name, getter, setter, ...)
table.publish_string(name, getter, setter, ...)
table.publish_boolean_array(name, getter, setter, ...)
table.publish_integer_array(name, getter, setter, ...)
table.publish_double_array(name, getter, setter, ...)
table.publish_string_array(name, getter, setter, ...)
table.publish_raw(name, getter, setter, ...)
table.remove(name)
```

The typed `publish_*` helpers accept the same `robust`, `mutable`, `properties`, and `type_string` keyword options as `publish_value()`.

`table.publish()` and `tunables.publish()` return `True` when the backend accepts the publication and `False` when it rejects it, such as for a duplicate path.

Getter-backed `publish_*()` methods call the getter once when published, refresh the cached native tunable before each top-level `TunableRegistry.update()`, and call the setter when a remote write is applied. These getter and setter callables must not raise. If the setter clamps or canonicalizes the request, the getter is read again before a robust backend echoes the tuned value in that same update. Refreshes compare the getter result against the cached native value and only mark it changed when the value differs, so unchanged values use the backend's normal dirty-tracking path instead of `ALWAYS_GET` polling.

The root-level functions mirror the root-level subset:

```py
tunables.get_table(name="")
tunables.publish(name, value) -> bool
tunables.add(name, value, *, value_type=None, element_type=None,
                     robust=False, mutable=True, on_tune=None,
                     properties=None, type_string="")
tunables.add_boolean(name, value)
tunables.add_int(name, value)
tunables.add_long(name, value)
tunables.add_float(name, value)
tunables.add_double(name, value)
tunables.remove(name)
tunables.TunableRegistry.remove(name_or_value)
```

Use `tunables.get_table().publish_*()` for root-level getter/setter-backed values.

`TunableRegistry.remove()` accepts a path string or a Python tunable/complex object. When an object
is passed, every root-level publication retained for that object is removed.

## Python Complex Tunables

Python complex tunables subclass `tunables.ComplexTunable` or provide a `publish_tunables(table)` method. `get_tunable_type()` may return a non-empty string, and `update_tunables()` is optional. `publish_tunables()` and `update_tunables()` must not raise; if they do, registry and backend state is not guaranteed to be restored.

```py
class TunablePID(tunables.ComplexTunable):
    def __init__(self) -> None:
        self.k_p = tunables.Tunable(0.05)
        self.k_i = tunables.Tunable(0.0)
        self.k_d = tunables.Tunable(0.001)

    def get_tunable_type(self) -> str:
        return "PIDController"

    def publish_tunables(self, table: tunables.TunableTable) -> None:
        table.publish("kP", self.k_p)
        table.publish("kI", self.k_i)
        table.publish("kD", self.k_d)

tunables.publish("drive/pid", TunablePID())
```

The Python adapter retains child `Tunable` and complex objects published from `publish_tunables()` so they stay alive after publication. Removing a complex parent removes retained child values and descendant backend entries.

`update_tunables()` runs during registry updates and can push local changes into child tunables:

```py
class UpdatingComplex(tunables.ComplexTunable):
    def __init__(self) -> None:
        self.value = tunables.Tunable(0)

    def publish_tunables(self, table: tunables.TunableTable) -> None:
        table.publish("value", self.value)

    def update_tunables(self) -> None:
        self.value.set(self.value.get() + 1)
```

## Python Selectable

`tunables.Selectable` is a pure-Python `ComplexTunable`. It publishes the same `default`, `options`, and `selected` child entries as Java and C++ and uses the tunable type string `"Selectable"`.

```py
chooser = tunables.Selectable[int]()
chooser.add("one", 1)
chooser.add_default("two", 2)
chooser.on_change(lambda value: print(value))
tunables.publish("auto", chooser)
```

`get_selected()` returns the selected value, falling back to the default, or `None` if neither name maps to a current option. Removing an option clears the default only if that option was the default; it does not change the selected option name.

## Thread Safety and Secondary Threads

Python `Tunable`, `ComplexTunable`, and `Selectable` objects are not internally thread-safe. `TunableRegistry.update()` releases the GIL while it waits for and enters the native update, and reacquires it when Python getters, setters, complex updates, and callbacks run. The GIL is therefore not a replacement for tunable synchronization.

Use `TunableRegistry.with_update_mutex(func)` for occasional direct access from a secondary thread. When choosing this model, every competing access, including access from the main robot loop, must use it:

```py
gain_result: list[float] = []

def update_gain() -> None:
    drive_gain.set(0.08)
    gain_result.append(drive_gain.get())

tunables.TunableRegistry.with_update_mutex(update_gain)
gain_copy = gain_result[0]
```

`with_update_mutex()` returns `None`, so copy results into caller-owned state inside the function. A mutable object returned by `get()` or `mutate()` must not be modified after leaving the critical section unless all competing access is still synchronized.

The binding releases the GIL while waiting for the update mutex, which avoids making the GIL itself part of that wait. The supplied function runs with both the GIL and update mutex held. Keep it short, and avoid waiting for I/O or another thread. Python `Lock` objects and other application locks can still deadlock if `update()` holds the update mutex and waits for an application lock while a worker holds that application lock and waits for `with_update_mutex()`.

For frequent cross-thread exchange, use `queue.SimpleQueue`, a lock-protected immutable snapshot, or another thread-safe application handoff and have the worker avoid direct access to the tunables. Getter/setter-backed `publish_*()` methods can bridge such state during the main-loop update. Their getter and setter run under the update mutex, but the backing state must still be safe for the worker. `always_get=True` and `mutable=False` affect polling and remote writes, not thread safety.

Callbacks run on the thread that calls `TunableRegistry.update()`, normally the main robot thread. Concurrent update calls serialize, but calling `update()` from a worker causes Python callbacks to run there and is not a substitute for synchronization.

## Python Migration from WPILib 2026

Key differences from 2026:

- `wpilib.SmartDashboard.putNumber("key", value)` / `wpilib.SmartDashboard.getNumber("key", default)` called every loop is replaced with a single `tunables.add_double("key", initial_value)` declaration that returns a `tunables.Tunable`. Read it with `get()` and write it with `set()`.
- Direct NetworkTables entry/topic boilerplate is replaced by the same `tunables.Tunable` pattern; the backend handles the underlying NT entry lifecycle.
- `wpilib.SendableChooser` is replaced by `tunables.Selectable`. The API is similar: `add(name, object)`, `add_default(name, object)`, `get_selected()`.
- The `Sendable` interface and `wpilib.SmartDashboard.putData()` are not part of the Tunable API; objects that previously implemented `Sendable` should implement `tunables.ComplexTunable` or use a WPILib object that already does, then register via `tunables.publish()`.

### SmartDashboard Tuning to Tunable

Use Tunable when the dashboard is allowed to change the value and robot code reads that value back.

**Was (WPILib 2026):**

```py
def robotInit(self) -> None:
    self.intake_speed = 0.65

def robotPeriodic(self) -> None:
    wpilib.SmartDashboard.putNumber("Intake/speed", self.intake_speed)
    self.intake_speed = wpilib.SmartDashboard.getNumber(
        "Intake/speed", self.intake_speed
    )
    self.intake_motor.set(self.intake_speed)
```

**Is (Tunable):**

```py
def robotInit(self) -> None:
    self.intake_speed = tunables.add_double("Intake/speed", 0.65)

def robotPeriodic(self) -> None:
    self.intake_motor.set(self.intake_speed.get())
```

Getter/setter-backed tunables are a closer match when existing robot code already owns the value or validates assignments:

**Was (WPILib 2026):**

```py
def robotInit(self) -> None:
    wpilib.SmartDashboard.putNumber(
        "Drive/maxOutput", self.drive.get_max_output()
    )

def robotPeriodic(self) -> None:
    max_output = wpilib.SmartDashboard.getNumber(
        "Drive/maxOutput", self.drive.get_max_output()
    )
    self.drive.set_max_output(max_output)
```

**Is (Tunable getter/setter):**

```py
def robotInit(self) -> None:
    tunables.get_table().publish_double(
        "Drive/maxOutput",
        self.drive.get_max_output,
        self.drive.set_max_output,
    )
```

### Editable Field2d to Tunable

**Was (WPILib 2026):**

```py
def robotInit(self) -> None:
    self.field = wpilib.Field2d()
    self.target = self.field.getObject("Target")
    self.drive_target_pose = Pose2d()
    wpilib.SmartDashboard.putData("Field", self.field)

def robotPeriodic(self) -> None:
    self.field.setRobotPose(self.pose_estimator.getEstimatedPosition())
    self.drive_target_pose = self.target.getPose()
```

**Is (Tunable):**

```py
def robotInit(self) -> None:
    self.field = wpilib.Field2d()
    self.target = self.field.getObject("Target")
    self.drive_target_pose = Pose2d()
    tunables.publish("Field", self.field)

def robotPeriodic(self) -> None:
    self.field.setRobotPose(self.pose_estimator.getEstimatedPosition())
    self.drive_target_pose = self.target.getPose()
```

### SendableChooser to Selectable

`tunables.Selectable` publishes the chooser data through the Tunable backend and returns the selected robot-owned object from `get_selected()`.

**Was (WPILib 2026):**

```py
class DriveMode(Enum):
    FIELD_RELATIVE = 1
    ROBOT_RELATIVE = 2

def robotInit(self) -> None:
    self.drive_mode_chooser = wpilib.SendableChooser()
    self.drive_mode_chooser.setDefaultOption(
        "Field Relative", DriveMode.FIELD_RELATIVE
    )
    self.drive_mode_chooser.addOption(
        "Robot Relative", DriveMode.ROBOT_RELATIVE
    )
    wpilib.SmartDashboard.putData("Drive Mode", self.drive_mode_chooser)

def teleopPeriodic(self) -> None:
    self.drive.set_mode(self.drive_mode_chooser.getSelected())
```

**Is (Selectable):**

```py
class DriveMode(Enum):
    FIELD_RELATIVE = 1
    ROBOT_RELATIVE = 2

def robotInit(self) -> None:
    self.drive_mode = tunables.Selectable[DriveMode]()
    self.drive_mode.add_default("Field Relative", DriveMode.FIELD_RELATIVE)
    self.drive_mode.add("Robot Relative", DriveMode.ROBOT_RELATIVE)
    tunables.publish("Drive/mode", self.drive_mode)

def teleopPeriodic(self) -> None:
    self.drive.set_mode(self.drive_mode.get_selected())
```

## Python Backend and Test APIs

Python exposes `TunableRegistry.set_report_warning(func_or_none)`, `report_warning(msg)`, `register_backend(prefix, backend)`, `get_backend(path)`, `get_table(path)`, `normalize_name(path)`, `update()`, `with_update_mutex(func)`, and `reset()`. Custom warning callbacks must not raise.

`TunableRegistry.reset()` also clears Python-retained tunables and reinstalls the pre-update refresh callback used by getter-backed Python values.

`MockTunableBackend` exposes remote-write helpers named after the C++ backend methods:

```py
backend = tunables.MockTunableBackend()
tunables.TunableRegistry.reset()
tunables.TunableRegistry.register_backend("", backend)

gain = tunables.add_double("drive/kP", 0.05)
backend.set_double("/drive/kP", 0.1)
tunables.TunableRegistry.update()
assert gain.get() == 0.1
```

Available mock setters include `set_bool()`, `set_int32()`, `set_int64()`, `set_float()`, `set_double()`, `set_string()`, `set_raw()`, `set_struct()`, `set_struct_vector()` / `set_struct_array()`, and corresponding vector setters. `get_uid(path)` returns the backend's registered UID for a path or `None`, and `get_value(path)` reads the current published native value, returning raw and struct values as bytes.
