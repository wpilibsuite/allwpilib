# Tunable Python API

This file describes the Python tunable API exposed by the `tunable` package. It follows the same registry/backend model as C++, but presents a Python-owned `Tunable` wrapper, snake_case methods, and Python `ComplexTunable` / `Selectable` classes.

- For the shared Tunable design and Java-facing API, see [Tunable design](tunable.md).
- For C++-specific behavior, see [C++ API](cpp.md).

## Python Overview

The primary entry points are:

- `tunable.Tunable`, a Python value wrapper with `get()` and `set()`
- `tunable.Tunables`, the root table facade
- `tunable.TunableTable`, a hierarchical publisher
- `tunable.ComplexTunable`, a Python ABC for objects that publish child tunables
- `tunable.Selectable`, a pure-Python chooser-style complex tunable
- `tunable.TunableRegistry`, for backend registration, updates, warning hooks, and tests

```py
import tunable

k_p = tunable.Tunables.add_double("drive/kP", 0.05)
k_p.set(0.07)
value = k_p.get()

drive = tunable.Tunables.get_table("drive")
max_speed = drive.add_double("maxSpeed", 3.0)
```

Python-created tunables are backed by native C++ tunables and retained by normalized publish path. `remove()` releases that retained storage for the removed publish path; complex tunable removal also releases retained child objects.

## `tunable.Tunable`

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
enabled = tunable.Tunable(False, value_type=bool)
names = tunable.Tunable([], element_type=str, mutable=False)
points = tunable.Tunable([], element_type=TunablePoint)  # a WPIStruct class
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

`table.publish()` and `tunable.Tunables.publish()` return `True` when the backend accepts the publication and `False` when it rejects it, such as for a duplicate path.

Getter-backed `publish_*()` methods call the getter once when published, refresh the cached native tunable before each top-level `TunableRegistry.update()`, and call the setter when a remote write is applied. These getter and setter callables must not raise. If the setter clamps or canonicalizes the request, the getter is read again before a robust backend echoes the tuned value in that same update. Refreshes compare the getter result against the cached native value and only mark it changed when the value differs, so unchanged values use the backend's normal dirty-tracking path instead of `ALWAYS_GET` polling.

`Tunables` mirrors the root-level subset:

```py
tunable.Tunables.get_table(name="")
tunable.Tunables.publish(name, value) -> bool
tunable.Tunables.add(name, value, *, value_type=None, element_type=None,
                     robust=False, mutable=True, on_tune=None,
                     properties=None, type_string="")
tunable.Tunables.add_boolean(name, value)
tunable.Tunables.add_int(name, value)
tunable.Tunables.add_long(name, value)
tunable.Tunables.add_float(name, value)
tunable.Tunables.add_double(name, value)
tunable.Tunables.remove(name)
tunable.TunableRegistry.remove(name_or_value)
```

Use `Tunables.get_table().publish_*()` for root-level getter/setter-backed values.

`TunableRegistry.remove()` accepts a path string or a Python tunable/complex object. When an object
is passed, every root-level publication retained for that object is removed.

## Python Complex Tunables

Python complex tunables subclass `tunable.ComplexTunable` or provide a `publish_tunable(table)` method. `get_tunable_type()` may return a non-empty string, and `update_tunable()` is optional. `publish_tunable()` and `update_tunable()` must not raise; if they do, registry and backend state is not guaranteed to be restored.

```py
class TunablePID(tunable.ComplexTunable):
    def __init__(self) -> None:
        self.k_p = tunable.Tunable(0.05)
        self.k_i = tunable.Tunable(0.0)
        self.k_d = tunable.Tunable(0.001)

    def get_tunable_type(self) -> str:
        return "PIDController"

    def publish_tunable(self, table: tunable.TunableTable) -> None:
        table.publish("kP", self.k_p)
        table.publish("kI", self.k_i)
        table.publish("kD", self.k_d)

tunable.Tunables.publish("drive/pid", TunablePID())
```

The Python adapter retains child `Tunable` and complex objects published from `publish_tunable()` so they stay alive after publication. Removing a complex parent removes retained child values and descendant backend entries.

`update_tunable()` runs during registry updates and can push local changes into child tunables:

```py
class UpdatingComplex(tunable.ComplexTunable):
    def __init__(self) -> None:
        self.value = tunable.Tunable(0)

    def publish_tunable(self, table: tunable.TunableTable) -> None:
        table.publish("value", self.value)

    def update_tunable(self) -> None:
        self.value.set(self.value.get() + 1)
```

## Python Selectable

`tunable.Selectable` is a pure-Python `ComplexTunable`. It publishes the same `default`, `options`, and `selected` child entries as Java and C++ and uses the tunable type string `"Selectable"`.

```py
chooser = tunable.Selectable[int]()
chooser.add("one", 1)
chooser.add_default("two", 2)
chooser.on_change(lambda value: print(value))
tunable.Tunables.publish("auto", chooser)
```

`get_selected()` returns the selected value, falling back to the default, or `None` if neither name maps to a current option. Removing an option clears the default only if that option was the default; it does not change the selected option name.

## Python Backend and Test APIs

Python exposes `TunableRegistry.set_report_warning(func_or_none)`, `report_warning(msg)`, `register_backend(prefix, backend)`, `get_backend(path)`, `get_table(path)`, `normalize_name(path)`, `update()`, `with_update_mutex(func)`, and `reset()`. Custom warning callbacks must not raise.

`TunableRegistry.reset()` also clears Python-retained tunables and reinstalls the pre-update refresh callback used by getter-backed Python values.

`MockTunableBackend` exposes remote-write helpers named after the C++ backend methods:

```py
backend = tunable.MockTunableBackend()
tunable.TunableRegistry.reset()
tunable.TunableRegistry.register_backend("", backend)

gain = tunable.Tunables.add_double("drive/kP", 0.05)
backend.set_double("/drive/kP", 0.1)
tunable.TunableRegistry.update()
assert gain.get() == 0.1
```

Available mock setters include `set_bool()`, `set_int32()`, `set_int64()`, `set_float()`, `set_double()`, `set_string()`, `set_raw()`, `set_struct()`, `set_struct_vector()` / `set_struct_array()`, and corresponding vector setters. `get_uid(path)` returns the backend's registered UID for a path or `None`, and `get_value(path)` reads the current published native value, returning raw and struct values as bytes.
