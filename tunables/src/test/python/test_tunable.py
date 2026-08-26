import dataclasses
import subprocess
import sys
import weakref

import pytest

import tunables
from wpiutil import wpistruct


@wpistruct.make_wpistruct(name="TunablePoint")
@dataclasses.dataclass
class TunablePoint:
    a: wpistruct.int16
    b: wpistruct.int16


@pytest.fixture
def backend():
    tunables.TunableRegistry.reset()
    backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("", backend)
    try:
        yield backend
    finally:
        tunables.TunableRegistry.set_report_warning(None)
        tunables.TunableRegistry.reset()


def test_tunable_get_set():
    value = tunables.Tunable(1)

    assert value.get() == 1
    value.set(2)
    assert value.get() == 2


def test_tunable_type_selectors_use_python_types():
    integer = tunables.Tunable(1, value_type=int)
    strings = tunables.Tunable([], element_type=str)

    assert integer.get() == 1
    assert strings.get() == []

    with pytest.raises(TypeError, match="value_type must be a Python type"):
        tunables.Tunable(1, value_type="integer")

    with pytest.raises(TypeError, match="element_type must be a Python type"):
        tunables.Tunable([], element_type="string")

    with pytest.raises(TypeError, match="use element_type for sequences"):
        tunables.Tunable([], value_type=str)


def test_backend_updates_tunables(backend):
    value = tunables.add("value", 1)

    backend.set_int64("/value", 3)
    tunables.TunableRegistry.update()

    assert value.get() == 3
    tunables.remove("value")


def test_report_warning_allows_reentry(backend):
    warnings = []

    def report_warning(msg: str) -> None:
        warnings.append(msg)
        if msg == "outer warning":
            tunables.TunableRegistry.report_warning("nested warning")

    tunables.TunableRegistry.set_report_warning(report_warning)

    tunables.TunableRegistry.report_warning("outer warning")

    assert warnings == ["outer warning", "nested warning"]


def test_update_mutex_waits_do_not_hold_gil():
    code = """
import threading
import time

import tunables


def run_waiting_call(waiting_call):
    entered = threading.Event()
    done = threading.Event()

    def holder():
        def hold():
            entered.set()
            time.sleep(0.2)

        tunables.TunableRegistry.with_update_mutex(hold)

    def waiter():
        waiting_call()
        done.set()

    holder_thread = threading.Thread(target=holder)
    waiter_thread = threading.Thread(target=waiter)

    holder_thread.start()
    assert entered.wait(1.0)

    waiter_thread.start()
    holder_thread.join(2.0)
    waiter_thread.join(2.0)
    assert done.is_set()


run_waiting_call(lambda: tunables.TunableRegistry.with_update_mutex(lambda: None))
run_waiting_call(tunables.TunableRegistry.update)
"""
    subprocess.run([sys.executable, "-c", code], check=True, timeout=5)


def test_primitive_and_array_tunables_update_from_backend(backend):
    boolean = tunables.add("boolean", True)
    integer = tunables.add("integer", 1)
    double = tunables.add("double", 2.0)
    string = tunables.add("string", "start")
    raw = tunables.add("raw", b"abc")
    booleans = tunables.add("booleans", [True, False])
    integers = tunables.add("integers", [1, 2])
    doubles = tunables.add("doubles", [1.0, 2])
    strings = tunables.add("strings", ["a", "b"])

    backend.set_bool("/boolean", False)
    backend.set_int64("/integer", 10)
    backend.set_double("/double", 20.0)
    backend.set_string("/string", "remote")
    backend.set_raw("/raw", bytearray(b"xyz"))
    backend.set_bool_vector("/booleans", [False, True])
    backend.set_int64_vector("/integers", [3, 4])
    backend.set_double_vector("/doubles", [3.0, 4.5])
    backend.set_string_vector("/strings", ["c", "d"])
    tunables.TunableRegistry.update()

    assert boolean.get() is False
    assert integer.get() == 10
    assert double.get() == pytest.approx(20.0)
    assert string.get() == "remote"
    assert raw.get() == b"xyz"
    assert booleans.get() == [False, True]
    assert integers.get() == [3, 4]
    assert doubles.get() == [3.0, 4.5]
    assert strings.get() == ["c", "d"]


def test_mutate_updates_stored_primitive_array_tunables(backend):
    raw = tunables.add("raw", b"abc")
    booleans = tunables.add("booleans", [True, False])
    integers = tunables.add("integers", [1, 2])
    doubles = tunables.add("doubles", [1.0, 2.0])
    strings = tunables.add("strings", ["a", "b"])

    raw_values = raw.mutate()
    raw_values[0] = ord("z")
    booleans.mutate()[1] = True
    integers.mutate()[0] += 2
    doubles.mutate().append(3.5)
    string_values = strings.mutate()
    string_values[1] = "c"
    string_values += ["d"]

    assert raw.get() == b"zbc"
    assert booleans.get() == [True, True]
    assert integers.get() == [3, 2]
    assert doubles.get() == [1.0, 2.0, 3.5]
    assert strings.get() == ["a", "c", "d"]

    tunables.TunableRegistry.update()

    assert backend.get_value("/raw") == b"zbc"
    assert backend.get_value("/booleans") == [True, True]
    assert backend.get_value("/integers") == [3, 2]
    assert backend.get_value("/doubles") == [1.0, 2.0, 3.5]
    assert backend.get_value("/strings") == ["a", "c", "d"]


def test_publish_value_uses_getter(backend):
    value = [1]

    published = tunables.get_table().publish_int(
        "getter", lambda: value[0], lambda tuned: value.__setitem__(0, tuned)
    )

    value[0] = 4
    assert published.get() == 4

    published.set(5)
    assert value[0] == 5
    assert published.get() == 5
    assert backend.get_value("/getter") == 5

    tunables.TunableRegistry.update()
    assert published.get() == 5

    backend.set_int64("/getter", 6)
    tunables.TunableRegistry.update()
    assert value[0] == 6
    tunables.remove("getter")


def test_publish_value_remote_setter_updates_cached_value_before_echo(backend):
    value = [1]

    tunables.get_table().publish_int(
        "clamped",
        lambda: value[0],
        lambda tuned: value.__setitem__(0, min(tuned, 5)),
    )

    backend.set_int64("/clamped", 10)
    tunables.TunableRegistry.update()

    assert value[0] == 5
    assert backend.get_value("/clamped") == 5
    tunables.remove("clamped")


def test_publish_value_getter_can_mutate_top_level_storage_during_refresh(backend):
    state = {"armed": False, "calls": 0}

    def get_value() -> int:
        state["calls"] += 1
        if state["armed"]:
            for i in range(32):
                tunables.add_int(f"added{i}", i)
            tunables.remove("value")
        return state["calls"]

    tunables.get_table().publish_int("value", get_value, lambda _value: None)
    assert state["calls"] == 1

    state["armed"] = True
    tunables.TunableRegistry.update()

    assert state["calls"] == 2
    assert backend.get_uid("/value") is None
    assert backend.get_value("/added31") == 31

    tunables.TunableRegistry.update()

    assert state["calls"] == 2


def test_table_remove_cleans_published_value_storage(backend):
    calls = []
    value = [1]
    table = tunables.get_table("child")

    table.publish_int(
        "getter",
        lambda: calls.append(value[0]) or value[0],
        lambda tuned: value.__setitem__(0, tuned),
    )
    assert calls == [1]

    table.remove("getter")
    value[0] = 2
    tunables.TunableRegistry.update()

    assert calls == [1]
    assert backend.get_uid("/child/getter") is None


def test_table_remove_cleans_normalized_published_value_storage(backend):
    class GetterBackedValue:
        def __init__(self) -> None:
            self.value = 1
            self.calls = 0

        def get(self) -> int:
            self.calls += 1
            return self.value

        def set(self, value: int) -> None:
            self.value = value

    table = tunables.get_table("child")
    value = GetterBackedValue()
    ref = weakref.ref(value)

    table.publish_int("/getter", value.get, value.set)
    assert value.calls == 1

    table.remove("/getter")
    value.value = 2
    tunables.TunableRegistry.update()

    assert value.calls == 1
    assert backend.get_uid("/child/getter") is None

    del value
    assert ref() is None


def test_duplicate_publication_preserves_retained_original(backend):
    warnings = []
    tunables.TunableRegistry.set_report_warning(warnings.append)

    tunables.add("duplicate", 1)
    original_uid = backend.get_uid("/duplicate")
    assert original_uid is not None

    tunables.add("duplicate", 2)
    assert backend.get_uid("/duplicate") == original_uid
    assert backend.get_value("/duplicate") == 1

    assert tunables.publish("published", tunables.Tunable(3)) is True
    assert backend.get_value("/published") == 3

    assert tunables.publish("duplicate", tunables.Tunable(4)) is False
    assert backend.get_uid("/duplicate") == original_uid
    assert backend.get_value("/duplicate") == 1

    table = tunables.get_table("child")
    assert table.publish("value", tunables.Tunable(5)) is True
    assert table.publish("value", tunables.Tunable(6)) is False
    assert backend.get_value("/child/value") == 5
    assert warnings.count("Tunable already exists: /duplicate") == 2
    assert warnings.count("Tunable already exists: /child/value") == 1


def test_config_immutable_and_on_tune(backend):
    calls = []
    mutable = tunables.add("mutable", 0, on_tune=lambda value: calls.append(value))
    immutable = tunables.add(
        "immutable",
        5,
        mutable=False,
        robust=True,
        properties={"min": 0},
        type_string="UnitTestWidget",
        on_tune=lambda value: calls.append(value),
    )

    backend.set_int64("/mutable", 1)
    backend.set_int64("/immutable", 42)
    tunables.TunableRegistry.update()

    assert mutable.get() == 1
    assert immutable.get() == 5
    assert calls == [1]


def test_table_paths_route_migrate_and_remove(backend):
    child_backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("/child", child_backend)

    assert tunables.get_table().path == "/"
    assert tunables.get_table("drive").path == "/drive/"
    assert tunables.get_table("drive").get_table("left").path == ("/drive/left/")
    assert tunables.TunableRegistry.normalize_name("///drive//left") == "/drive/left"

    root = tunables.add("root", 1.0)
    child = tunables.add("child/value", 2.0)

    assert backend.get_value("/root") == pytest.approx(1.0)
    assert child_backend.get_value("/child/value") == pytest.approx(2.0)
    assert backend.get_uid("/child/value") is None

    backend.set_double("/root", 3.0)
    child_backend.set_double("/child/value", 4.0)
    tunables.TunableRegistry.update()

    assert root.get() == pytest.approx(3.0)
    assert child.get() == pytest.approx(4.0)

    tunables.remove("child/value")
    assert child_backend.get_uid("/child/value") is None


def test_get_backend_normalizes_path(backend):
    child_backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("/child", child_backend)

    assert tunables.TunableRegistry.get_backend("child/value") is child_backend
    assert tunables.TunableRegistry.get_backend("//child//value") is child_backend

    tunables.TunableRegistry.reset()
    child_only_backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("/child", child_only_backend)

    assert tunables.TunableRegistry.get_backend("child/value") is child_only_backend


def test_register_backend_migrates_existing_tunables(backend):
    root = tunables.add("root", 1.0)
    child = tunables.add("child/value", 2.0)

    child_backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("/child", child_backend)

    assert backend.get_uid("/root") is not None
    assert backend.get_uid("/child/value") is None
    assert child_backend.get_uid("/child/value") is not None

    backend.set_double("/root", 3.0)
    child_backend.set_double("/child/value", 4.0)
    tunables.TunableRegistry.update()

    assert root.get() == pytest.approx(3.0)
    assert child.get() == pytest.approx(4.0)


def test_register_backend_replacement_migrates_existing_tunables(backend):
    value = tunables.add("value", 1.0)
    replacement_backend = tunables.MockTunableBackend()

    tunables.TunableRegistry.register_backend("", replacement_backend)

    assert backend.get_uid("/value") is None
    assert replacement_backend.get_uid("/value") is not None

    replacement_backend.set_double("/value", 3.0)
    tunables.TunableRegistry.update()

    assert value.get() == pytest.approx(3.0)


def test_publish_retains_complex_tunables(backend):
    value = tunables.Selectable()
    value.add_default("option", True)
    ref = weakref.ref(value)

    tunables.publish("selectable", value)
    del value

    assert bool(ref) is True
    tunables.remove("selectable")


def test_complex_tunable_publishes_members_and_updates(backend):
    class UpdatingComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.value = tunables.Tunable(0)
            self.update_count = 0

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish("value", self.value)

        def update_tunables(self) -> None:
            self.update_count += 1
            self.value.set(self.value.get() + 1)

        def get_tunable_type(self) -> str:
            return "UpdatingComplex"

    value = UpdatingComplex()
    tunables.publish("complex", value)

    assert backend.get_value("/complex/value") == 0

    tunables.TunableRegistry.update()
    tunables.TunableRegistry.update()

    assert value.update_count == 2
    assert value.value.get() == 2
    assert backend.get_value("/complex/value") == 2


def test_remove_complex_tunable_removes_members(backend):
    class RemovedComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.value = tunables.Tunable(1)
            self.update_count = 0

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish("value", self.value)

        def update_tunables(self) -> None:
            self.update_count += 1

    value = RemovedComplex()
    tunables.publish("complex", value)
    tunables.remove("complex")
    tunables.TunableRegistry.update()

    assert value.value.get() == 1
    assert value.update_count == 0
    assert backend.get_uid("/complex") is None
    assert backend.get_uid("/complex/value") is None


def test_registry_remove_complex_tunable_by_object(backend):
    class RemovedComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.value = tunables.Tunable(1)
            self.update_count = 0

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish("value", self.value)

        def update_tunables(self) -> None:
            self.update_count += 1

    value = RemovedComplex()
    tunables.publish("first", value)
    tunables.publish("second", value)

    tunables.TunableRegistry.remove(value)
    tunables.TunableRegistry.update()

    assert value.update_count == 0
    assert backend.get_uid("/first") is None
    assert backend.get_uid("/first/value") is None
    assert backend.get_uid("/second") is None
    assert backend.get_uid("/second/value") is None


def test_registry_remove_path_string(backend):
    tunables.add("value", 1)

    tunables.TunableRegistry.remove("value")

    assert backend.get_uid("/value") is None


def test_table_remove_releases_complex_tunables(backend):
    class RemovedComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.value = tunables.Tunable(1)

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish("value", self.value)

    table = tunables.get_table("child")
    value = RemovedComplex()
    ref = weakref.ref(value)

    table.publish("complex", value)
    del value

    assert ref() is not None
    table.remove("complex")

    assert ref() is None
    assert backend.get_uid("/child/complex") is None
    assert backend.get_uid("/child/complex/value") is None


def test_remove_normalized_complex_tunable_releases_storage(backend):
    class RemovedComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.value = 1
            self.calls = 0

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish_int("/value", self._get_value, self._set_value)

        def _get_value(self) -> int:
            self.calls += 1
            return self.value

        def _set_value(self, value: int) -> None:
            self.value = value

    value = RemovedComplex()
    ref = weakref.ref(value)

    tunables.publish("child//complex", value)
    assert value.calls == 1

    tunables.remove("child/complex")
    value.value = 2
    tunables.TunableRegistry.update()

    assert value.calls == 1
    assert backend.get_uid("/child/complex") is None
    assert backend.get_uid("/child/complex/value") is None

    del value
    assert ref() is None


def test_complex_table_remove_releases_published_value_child(backend):
    calls = []

    class ChildValue:
        def __init__(self) -> None:
            self.value = 1

        def get(self) -> int:
            calls.append(self.value)
            return self.value

        def set(self, value: int) -> None:
            self.value = value

    class RemovingComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.child = ChildValue()
            self.table: tunables.TunableTable | None = None

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            self.table = table
            table.publish_int("value", self.child.get, self.child.set)

    value = RemovingComplex()
    child = value.child
    ref = weakref.ref(child)

    tunables.publish("complex", value)
    assert calls == [1]

    assert value.table is not None
    value.table.remove("value")
    child.value = 2
    value.child = None
    del child
    tunables.TunableRegistry.update()

    assert calls == [1]
    assert ref() is None
    assert backend.get_uid("/complex/value") is None


def test_complex_table_remove_releases_nested_complex_child(backend):
    calls = []

    class ChildValue:
        def __init__(self) -> None:
            self.value = 1

        def get(self) -> int:
            calls.append(self.value)
            return self.value

        def set(self, value: int) -> None:
            self.value = value

    class NestedComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.value = ChildValue()

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish_int("value", self.value.get, self.value.set)

    class RemovingComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.child = NestedComplex()
            self.table: tunables.TunableTable | None = None

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            self.table = table
            table.publish("child", self.child)

    value = RemovingComplex()
    child = value.child
    ref = weakref.ref(child)

    tunables.publish("complex", value)
    assert calls == [1]

    assert value.table is not None
    value.table.remove("child")
    value.child = None
    del child
    tunables.TunableRegistry.update()

    assert calls == [1]
    assert ref() is None
    assert backend.get_uid("/complex/child") is None
    assert backend.get_uid("/complex/child/value") is None


def test_register_backend_migrates_complex_tunables(backend):
    class MigratedComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.value = tunables.Tunable(2)
            self.update_count = 0

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish("value", self.value)

        def update_tunables(self) -> None:
            self.update_count += 1

    value = MigratedComplex()
    tunables.publish("child/complex", value)

    child_backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("/child", child_backend)

    assert backend.get_uid("/child/complex") is None
    assert backend.get_uid("/child/complex/value") is None
    assert child_backend.get_uid("/child/complex") is not None
    assert child_backend.get_uid("/child/complex/value") is not None

    child_backend.set_int64("/child/complex/value", 4)
    tunables.TunableRegistry.update()

    assert value.value.get() == 4
    assert value.update_count == 1


def test_migrated_complex_publish_value_refreshes_once(backend):
    class MigratedComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.value = 2
            self.getter_calls = 0

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish_int("value", self._get_value, self._set_value)

        def _get_value(self) -> int:
            self.getter_calls += 1
            return self.value

        def _set_value(self, value: int) -> None:
            self.value = value

    value = MigratedComplex()
    tunables.publish("child/complex", value)

    child_backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("/child", child_backend)
    complex_backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("/child/complex", complex_backend)

    value.getter_calls = 0
    value.value = 5
    tunables.TunableRegistry.update()

    assert value.getter_calls == 1
    assert complex_backend.get_value("/child/complex/value") == 5


def test_more_specific_child_backend_keeps_migrated_complex_child(backend):
    class MigratedComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.value = tunables.Tunable(2)

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish("value", self.value)

    leaf_backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("/child/complex/value", leaf_backend)

    value = MigratedComplex()
    tunables.publish("child/complex", value)

    child_backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("/child", child_backend)

    assert child_backend.get_uid("/child/complex") is not None
    assert child_backend.get_uid("/child/complex/value") is None
    assert leaf_backend.get_uid("/child/complex/value") is not None

    leaf_backend.set_int64("/child/complex/value", 7)
    tunables.TunableRegistry.update()

    assert value.value.get() == 7


def test_complex_tunable_publish_descriptor_is_reused_for_initial_publish(backend):
    class DescriptorComplex:
        def __init__(self) -> None:
            self.value = tunables.Tunable(1)
            self.lookup_count = 0
            self.publish_count = 0

        @property
        def publish_tunables(self):
            self.lookup_count += 1

            def publish(table: tunables.TunableTable) -> None:
                self.publish_count += 1
                table.publish("value", self.value)

            return publish

    value = DescriptorComplex()

    tunables.publish("descriptor", value)

    assert value.lookup_count == 1
    assert value.publish_count == 1
    assert backend.get_uid("/descriptor/value") is not None


def test_complex_tunable_publish_lookup_error_is_reported(backend):
    class BrokenLookup:
        def __getattr__(self, name: str):
            if name == "publish_tunables":
                raise RuntimeError("broken descriptor")
            raise AttributeError(name)

    with pytest.raises(RuntimeError, match="broken descriptor"):
        tunables.publish("brokenLookup", BrokenLookup())

    assert backend.get_uid("/brokenLookup") is None


def test_struct_tunable_and_struct_array_update_from_backend(backend):
    point = tunables.add("point", TunablePoint(1, 2))
    points = tunables.add("points", [TunablePoint(1, 2), TunablePoint(3, 4)])

    assert backend.get_value("/point") == wpistruct.pack(TunablePoint(1, 2))
    assert backend.get_value("/points") == wpistruct.pack_array(
        [TunablePoint(1, 2), TunablePoint(3, 4)]
    )

    backend.set_struct("/point", TunablePoint(5, 6))
    backend.set_struct_vector("/points", [TunablePoint(7, 8), TunablePoint(9, 10)])
    tunables.TunableRegistry.update()

    assert point.get() == TunablePoint(5, 6)
    assert points.get() == [TunablePoint(7, 8), TunablePoint(9, 10)]


def test_mutate_marks_struct_tunables_changed(backend):
    point = tunables.add("point", TunablePoint(1, 2))
    points = tunables.add("points", [TunablePoint(3, 4)])

    point.mutate().a = 5
    points.mutate()[0].a = 6
    tunables.TunableRegistry.update()

    assert backend.get_value("/point") == wpistruct.pack(TunablePoint(5, 2))
    assert backend.get_value("/points") == wpistruct.pack_array([TunablePoint(6, 4)])


def test_empty_tunable_array_requires_element_type(backend):
    with pytest.raises(TypeError, match="empty tunable sequences require element_type"):
        tunables.add("untyped", [])

    doubles = tunables.add("doubles", [], element_type=float)
    points = tunables.add("points", [], element_type=TunablePoint)

    assert doubles.get() == []
    assert backend.get_value("/doubles") == []
    assert points.get() == []
    assert backend.get_value("/points") == b""


def test_struct_array_tunable_can_be_cleared(backend):
    points = tunables.add("points", [TunablePoint(1, 2)])

    points.set([])

    assert points.get() == []
    assert backend.get_value("/points") == b""


def test_struct_array_publish_value_can_refresh_to_empty_sequence(backend):
    points = [[TunablePoint(1, 2)]]

    tunables.get_table().publish_value(
        "points", lambda: points[0], lambda tuned: points.__setitem__(0, tuned)
    )
    points[0] = []

    tunables.TunableRegistry.update()

    assert backend.get_value("/points") == b""


def test_struct_publish_value_refreshes_in_place_mutation(backend):
    point = TunablePoint(1, 2)

    tunables.get_table().publish_value("point", lambda: point, lambda _: None)
    point.a = 3

    tunables.TunableRegistry.update()

    assert backend.get_value("/point") == wpistruct.pack(TunablePoint(3, 2))


def test_complex_tunable_direct_struct_publish_value(backend):
    class DirectStructComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.point = TunablePoint(1, 2)

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish_value("point", lambda: self.point, self._set_point)

        def _set_point(self, value: TunablePoint) -> None:
            self.point = value

    value = DirectStructComplex()
    tunables.publish("directStruct", value)

    backend.set_struct("/directStruct/point", TunablePoint(3, 4))
    tunables.TunableRegistry.update()

    assert value.point == TunablePoint(3, 4)


def test_complex_tunable_direct_publish_value_refreshes_before_update(backend):
    class DirectGetterComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.value = 1

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish_int("value", lambda: self.value, self._set_value)

        def _set_value(self, value: int) -> None:
            self.value = value

    value = DirectGetterComplex()
    tunables.publish("directGetter", value)

    value.value = 4
    tunables.TunableRegistry.update()

    assert backend.get_value("/directGetter/value") == 4


def test_complex_tunable_getter_can_mutate_top_level_storage_during_refresh(
    backend,
):
    class EmptyComplex(tunables.ComplexTunable):
        def publish_tunables(self, table: tunables.TunableTable) -> None:
            pass

    class MutatingComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.armed = False
            self.calls = 0
            self.value = 1

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish_int("value", self._get_value, self._set_value)

        def _get_value(self) -> int:
            self.calls += 1
            if self.armed:
                tunables.publish("addedComplex", EmptyComplex())
                tunables.remove("complex")
            return self.value

        def _set_value(self, value: int) -> None:
            self.value = value

    value = MutatingComplex()
    tunables.publish("complex", value)
    assert value.calls == 1

    value.armed = True
    tunables.TunableRegistry.update()

    assert value.calls == 2
    assert backend.get_uid("/complex") is None
    assert backend.get_uid("/complex/value") is None
    assert backend.get_uid("/addedComplex") is not None

    tunables.TunableRegistry.update()

    assert value.calls == 2


def test_complex_tunable_wrapped_struct_member(backend):
    class WrappedStructComplex(tunables.ComplexTunable):
        def __init__(self) -> None:
            self.point = tunables.Tunable(TunablePoint(1, 2))

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.publish("point", self.point)

    value = WrappedStructComplex()
    tunables.publish("wrappedStruct", value)

    backend.set_struct("/wrappedStruct/point", TunablePoint(5, 6))
    tunables.TunableRegistry.update()

    assert value.point.get() == TunablePoint(5, 6)


@pytest.fixture
def chooser() -> tunables.Selectable[int]:
    chooser = tunables.Selectable()
    for i in range(1, 4):
        chooser.add(str(i), i)
    return chooser


@pytest.mark.parametrize("value", [0, 1, 2, 3])
def test_selectable_returns_selected(
    backend: tunables.MockTunableBackend,
    chooser: tunables.Selectable[int],
    value: int,
):
    chooser.add_default("0", 0)
    name = f"ReturnsSelectedChooser{value}"

    tunables.publish(name, chooser)
    backend.set_string(f"/{name}/selected", str(value))
    tunables.TunableRegistry.update()

    assert value == chooser.get_selected()
    tunables.remove(name)


def test_selectable_default_is_returned_on_no_select(
    chooser: tunables.Selectable[int],
):
    chooser.add_default("4", 4)
    assert 4 == chooser.get_selected()


def test_selectable_default_is_returned_on_unknown_select(
    backend: tunables.MockTunableBackend,
):
    chooser = tunables.Selectable()
    chooser.add_default("one", 1)
    chooser.add("two", 2)

    tunables.publish("UnknownDefaultChooser", chooser)
    backend.set_string("/UnknownDefaultChooser/selected", "missing")
    tunables.TunableRegistry.update()

    assert chooser.get_selected() == 1
    tunables.remove("UnknownDefaultChooser")


def test_selectable_default_constructable_is_returned_on_no_select_and_no_default(
    chooser: tunables.Selectable[int],
):
    assert chooser.get_selected() is None


def test_selectable_change_listener(
    backend: tunables.MockTunableBackend,
    chooser: tunables.Selectable[int],
):
    current_val = [0]

    def on_change(val):
        current_val[0] = val

    chooser.on_change(on_change)
    tunables.publish("ChangeListenerChooser", chooser)
    backend.set_string("/ChangeListenerChooser/selected", "3")
    tunables.TunableRegistry.update()

    assert 3 == current_val[0]
    tunables.remove("ChangeListenerChooser")


def test_selectable_change_listener_uses_default_when_selection_is_unknown(
    backend: tunables.MockTunableBackend,
):
    chooser = tunables.Selectable()
    chooser.add_default("one", 1)
    chooser.add("two", 2)
    current_val = [0]
    chooser.on_change(lambda value: current_val.__setitem__(0, value))

    tunables.publish("ChangeListenerUnknownDefaultChooser", chooser)
    backend.set_string("/ChangeListenerUnknownDefaultChooser/selected", "missing")
    tunables.TunableRegistry.update()

    assert current_val[0] == 1
    tunables.remove("ChangeListenerUnknownDefaultChooser")


def test_selectable_publishes_metadata_and_ignores_remote_metadata_writes(
    backend: tunables.MockTunableBackend,
):
    chooser = tunables.Selectable()
    chooser.add("one", 1)
    chooser.add_default("two", 2)

    assert chooser.get_tunable_type() == "Selectable"
    assert chooser.get_selected() == 2

    tunables.publish("MetadataChooser", chooser)

    assert backend.get_value("/MetadataChooser/default") == "two"
    assert backend.get_value("/MetadataChooser/options") == ["one", "two"]
    assert backend.get_value("/MetadataChooser/selected") == ""

    backend.set_string("/MetadataChooser/default", "one")
    backend.set_string_vector("/MetadataChooser/options", ["remote"])
    backend.set_string("/MetadataChooser/selected", "one")
    tunables.TunableRegistry.update()

    assert backend.get_value("/MetadataChooser/default") == "two"
    assert backend.get_value("/MetadataChooser/options") == ["one", "two"]
    assert chooser.get_selected() == 1
    tunables.remove("MetadataChooser")


def test_selectable_listener_is_not_called_for_unknown_selection(
    backend: tunables.MockTunableBackend,
):
    chooser = tunables.Selectable()
    chooser.add("one", 1)
    current_val = [0]
    chooser.on_change(lambda value: current_val.__setitem__(0, value))

    tunables.publish("UnknownSelectionChooser", chooser)
    backend.set_string("/UnknownSelectionChooser/selected", "missing")
    tunables.TunableRegistry.update()

    assert current_val[0] == 0
    assert chooser.get_selected() is None
    tunables.remove("UnknownSelectionChooser")


def test_selectable_listener_replacement_uses_latest_listener(
    backend: tunables.MockTunableBackend,
):
    chooser = tunables.Selectable()
    chooser.add("one", 1)
    first = [0]
    second = [0]
    chooser.on_change(lambda value: first.__setitem__(0, value))
    chooser.on_change(lambda value: second.__setitem__(0, value))

    tunables.publish("ListenerReplacementChooser", chooser)
    backend.set_string("/ListenerReplacementChooser/selected", "one")
    tunables.TunableRegistry.update()

    assert first[0] == 0
    assert second[0] == 1
    tunables.remove("ListenerReplacementChooser")


def test_selectable_duplicate_option_and_clear(
    backend: tunables.MockTunableBackend,
):
    chooser = tunables.Selectable()
    chooser.add("mode", 1)
    chooser.add("mode", 2)

    tunables.publish("DuplicateChooser", chooser)
    assert backend.get_value("/DuplicateChooser/options") == ["mode"]

    backend.set_string("/DuplicateChooser/selected", "mode")
    tunables.TunableRegistry.update()
    assert chooser.get_selected() == 2

    chooser.clear()
    assert backend.get_value("/DuplicateChooser/default") == ""
    assert backend.get_value("/DuplicateChooser/options") == []
    assert chooser.get_selected() is None

    chooser.add("mode", 22)
    assert chooser.get_selected() == 22
    tunables.remove("DuplicateChooser")


def test_selectable_remove_option(
    backend: tunables.MockTunableBackend,
):
    chooser = tunables.Selectable()
    chooser.add_default("one", 1)
    chooser.add("two", 2)
    chooser.add("three", 3)

    tunables.publish("RemoveChooser", chooser)
    backend.set_string("/RemoveChooser/selected", "two")
    tunables.TunableRegistry.update()

    chooser.remove("one")
    assert backend.get_value("/RemoveChooser/default") == ""
    assert backend.get_value("/RemoveChooser/options") == ["two", "three"]
    assert chooser.get_selected() == 2

    chooser.remove("two")
    assert backend.get_value("/RemoveChooser/options") == ["three"]
    assert chooser.get_selected() is None

    chooser.add("two", 22)
    assert backend.get_value("/RemoveChooser/options") == ["three", "two"]
    assert chooser.get_selected() == 22

    chooser.remove("missing")
    assert backend.get_value("/RemoveChooser/options") == ["three", "two"]
    assert chooser.get_selected() == 22
    tunables.remove("RemoveChooser")
