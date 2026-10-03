import array
import dataclasses
import gc
import inspect
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


def test_mock_backend_registry_publication_uses_native_implementation():
    class Backend(tunables.MockTunableBackend):
        def publish(self, *args):
            return False

    tunables.TunableRegistry.reset()
    backend = Backend()
    try:
        tunables.TunableRegistry.register_backend("", backend)
        calls = []
        value = tunables.Tunable(1, on_tune=calls.append)

        assert tunables.publish("value", value) is True
        assert backend.get_int64("/value") == 1

        backend.set_int64("/value", 3)
        tunables.TunableRegistry.update()
        assert value.get() == 3
        assert calls == [3]

        tunables.remove("value")
        assert backend.get_uid("/value") is None
    finally:
        tunables.TunableRegistry.reset()


def test_mock_backend_exposes_non_protobuf_typed_and_struct_getters(backend):
    tunables.add("boolean", True)
    tunables.add("integer", 1)
    tunables.add_int("int32", 3)
    tunables.add("double", 2.5)
    tunables.add_float("float", 4.5)
    tunables.add("string", "initial")
    tunables.add("raw", b"\x01\x02")
    tunables.add("booleans", [True, False])
    tunables.add("integers", [1, 2])
    tunables.add("doubles", [1.5, 2.5])
    tunables.add("strings", ["one", "two"])
    tunables.add("point", TunablePoint(1, 2))
    tunables.add("points", [TunablePoint(3, 4), TunablePoint(5, 6)])

    assert backend.get_bool("/boolean") is True
    assert type(backend.get_int64("/integer")) is int
    assert backend.get_int32("/int32") == 3
    assert type(backend.get_double("/double")) is float
    assert backend.get_float("/float") == pytest.approx(4.5)
    assert type(backend.get_string("/string")) is str
    assert backend.get_raw("/raw") == b"\x01\x02"
    assert backend.get_bool_vector("/booleans") == [True, False]
    assert backend.get_int64_vector("/integers") == [1, 2]
    assert backend.get_double_vector("/doubles") == [1.5, 2.5]
    assert backend.get_string_vector("/strings") == ["one", "two"]

    with pytest.raises(ValueError):
        backend.get_int32("/integer")
    with pytest.raises(ValueError):
        backend.get_float("/double")
    with pytest.raises(ValueError):
        backend.get_int32_vector("/integers")
    with pytest.raises(ValueError):
        backend.get_float_vector("/doubles")

    assert backend.get_struct_type_name("/point") == "TunablePoint"
    assert backend.get_struct_data("/point") == wpistruct.pack(TunablePoint(1, 2))
    point = backend.get_struct("/point", TunablePoint)
    points = backend.get_struct_vector("/points", TunablePoint)
    assert type(point) is TunablePoint
    assert point == TunablePoint(1, 2)
    assert type(points) is list
    assert all(type(value) is TunablePoint for value in points)
    assert points == [TunablePoint(3, 4), TunablePoint(5, 6)]
    with pytest.raises(TypeError, match="not struct serializable"):
        backend.get_struct("/point", int)
    with pytest.raises(TypeError, match="not struct serializable"):
        backend.get_struct_vector("/points", int)

    backend.set_bool("/boolean", False)
    backend.set_int64("/integer", 7)
    backend.set_double("/double", 8.5)
    backend.set_string("/string", "updated")
    backend.set_raw("/raw", b"\x03\x04")
    backend.set_bool_vector("/booleans", [False, True])
    backend.set_int64_vector("/integers", [7, 8])
    backend.set_double_vector("/doubles", [7.5, 8.5])
    backend.set_string_vector("/strings", ["three", "four"])
    backend.set_struct("/point", TunablePoint(7, 8))
    backend.set_struct_vector("/points", [TunablePoint(9, 10)])
    tunables.TunableRegistry.update()

    assert backend.get_bool("/boolean") is False
    assert backend.get_int64("/integer") == 7
    assert backend.get_double("/double") == pytest.approx(8.5)
    assert backend.get_string("/string") == "updated"
    assert backend.get_raw("/raw") == b"\x03\x04"
    assert backend.get_bool_vector("/booleans") == [False, True]
    assert backend.get_int64_vector("/integers") == [7, 8]
    assert backend.get_double_vector("/doubles") == [7.5, 8.5]
    assert backend.get_string_vector("/strings") == ["three", "four"]
    assert backend.get_struct("/point", TunablePoint) == TunablePoint(7, 8)
    assert backend.get_struct_vector("/points", TunablePoint) == [TunablePoint(9, 10)]

    for protobuf_name in (
        "get_protobuf_type_string",
        "get_protobuf_data",
        "get_protobuf",
        "set_protobuf",
    ):
        assert not hasattr(backend, protobuf_name)


def test_tunable_get_set():
    value = tunables.Tunable(1)

    assert value.get() == 1
    value.set(2)
    assert value.get() == 2


@pytest.fixture(params=["constructor", "add", "table_add", "publish_value"])
def create_tunable(backend, request):
    count = 0

    def create(value, **kwargs):
        nonlocal count
        count += 1
        name = f"value{count}"
        if request.param == "constructor":
            return tunables.Tunable(value, **kwargs)
        if request.param == "add":
            return tunables.add(name, value, **kwargs)
        table = tunables.get_table("table")
        if request.param == "table_add":
            return table.add(name, value, **kwargs)
        return table.publish_value(name, lambda: value, lambda value: None, **kwargs)

    return create


def test_tunable_type_selectors_use_python_types(create_tunable):
    integer = create_tunable(1, value_type=int)
    strings = create_tunable([], element_type=str)

    assert integer.get() == 1
    assert strings.get() == []

    with pytest.raises(TypeError, match="value_type must be a Python type"):
        create_tunable(1, value_type="integer")

    with pytest.raises(TypeError, match="element_type must be a Python type"):
        create_tunable([], element_type="string")

    with pytest.raises(TypeError, match="use element_type for sequences"):
        create_tunable([], value_type=str)


@pytest.mark.parametrize("initial", [1, [1, 2], TunablePoint(1, 2)])
def test_none_type_selectors_infer_tunable_type(create_tunable, initial):
    value = create_tunable(initial, value_type=None, element_type=None)

    assert value.get() == initial


def test_tunable_type_selectors_are_mutually_exclusive(create_tunable):
    with pytest.raises(
        TypeError, match="value_type and element_type are mutually exclusive"
    ):
        create_tunable([1], value_type=int, element_type=int)


def test_tunable_type_selector_annotations():
    for method in (
        tunables.Tunable.__init__,
        tunables.add,
        tunables.TunableTable.add,
        tunables.TunableTable.publish_value,
    ):
        assert "value_type: type[object] | None = None" in method.__doc__
        assert "element_type: type[object] | None = None" in method.__doc__


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


def test_update_mutex_context_is_fresh_and_returns_self():
    first = tunables.TunableRegistry.with_update_mutex()
    second = tunables.TunableRegistry.with_update_mutex()

    assert first is not second
    assert first.__enter__() is first
    first.__exit__(None, None, None)
    with first as entered:
        assert entered is first


def test_update_mutex_contexts_nest_on_same_thread():
    with tunables.TunableRegistry.with_update_mutex():
        with tunables.TunableRegistry.with_update_mutex():
            tunables.TunableRegistry.update()


def test_update_mutex_context_rejects_reentering_same_object():
    context = tunables.TunableRegistry.with_update_mutex()

    with context:
        with pytest.raises(RuntimeError, match="already entered"):
            context.__enter__()


def test_update_mutex_context_rejects_cross_thread_enter_without_blocking():
    code = """
import threading

import tunables


context = tunables.TunableRegistry.with_update_mutex()
owner_entered = threading.Event()
allow_owner_exit = threading.Event()
errors = []


def owner():
    try:
        with context:
            owner_entered.set()
            assert allow_owner_exit.wait(1.0), "owner was not allowed to exit"
    except BaseException as exc:
        errors.append(exc)


owner_thread = threading.Thread(target=owner)
owner_thread.start()
assert owner_entered.wait(1.0), "owner did not enter"
try:
    context.__enter__()
except RuntimeError as exc:
    assert "already entered" in str(exc)
else:
    raise AssertionError("cross-thread enter unexpectedly succeeded")
finally:
    allow_owner_exit.set()

owner_thread.join(2.0)
assert not owner_thread.is_alive(), "owner thread hung"
assert errors == []
with tunables.TunableRegistry.with_update_mutex():
    pass
"""
    subprocess.run([sys.executable, "-c", code], check=True, timeout=5)


def test_update_mutex_context_rejects_enter_while_same_object_is_acquiring():
    code = """
import sys
import threading

import tunables


sys.setswitchinterval(10.0)
blocker = tunables.TunableRegistry.with_update_mutex()
blocker.__enter__()
context = tunables.TunableRegistry.with_update_mutex()
owner_attempting = threading.Event()
owner_acquired = threading.Event()
errors = []


def owner():
    try:
        owner_attempting.set()
        context.__enter__()
        owner_acquired.set()
        context.__exit__(None, None, None)
    except BaseException as exc:
        errors.append(exc)


owner_thread = threading.Thread(target=owner)
owner_thread.start()
assert owner_attempting.wait(1.0), "owner did not attempt entry"
assert not owner_acquired.is_set(), "owner acquired while blocker held the mutex"
try:
    context.__enter__()
except RuntimeError as exc:
    assert "already entered" in str(exc)
else:
    raise AssertionError("concurrent enter unexpectedly succeeded")

blocker.__exit__(None, None, None)
owner_thread.join(2.0)
assert not owner_thread.is_alive(), "owner thread hung"
assert owner_acquired.is_set()
assert errors == []
with context:
    pass
"""
    subprocess.run([sys.executable, "-c", code], check=True, timeout=5)


def test_update_mutex_context_wrong_thread_exit_preserves_owner_recovery():
    code = """
import threading

import tunables


context = tunables.TunableRegistry.with_update_mutex()
owner_entered = threading.Event()
wrong_exit_finished = threading.Event()
errors = []


def owner():
    try:
        context.__enter__()
        owner_entered.set()
        assert wrong_exit_finished.wait(1.0), "wrong-thread exit did not finish"
        context.__exit__(None, None, None)
    except BaseException as exc:
        errors.append(exc)


owner_thread = threading.Thread(target=owner)
owner_thread.start()
assert owner_entered.wait(1.0), "owner did not enter"
try:
    context.__exit__(None, None, None)
except RuntimeError as exc:
    assert "owning thread" in str(exc)
else:
    raise AssertionError("wrong-thread exit unexpectedly succeeded")
finally:
    wrong_exit_finished.set()

owner_thread.join(2.0)
assert not owner_thread.is_alive(), "owner thread hung"
assert errors == []
with tunables.TunableRegistry.with_update_mutex():
    pass
"""
    subprocess.run([sys.executable, "-c", code], check=True, timeout=5)


def test_update_mutex_context_propagates_exception_and_unlocks():
    code = """
import threading

import tunables


class ExpectedError(Exception):
    pass


context = tunables.TunableRegistry.with_update_mutex()
try:
    with context:
        raise ExpectedError
except ExpectedError:
    pass
else:
    raise AssertionError("with statement suppressed the exception")

acquired = threading.Event()


def acquire():
    with tunables.TunableRegistry.with_update_mutex():
        acquired.set()


thread = threading.Thread(target=acquire, daemon=True)
thread.start()
thread.join(2.0)
assert not thread.is_alive(), "mutex remained locked after exceptional exit"
assert acquired.is_set()
"""
    subprocess.run([sys.executable, "-c", code], check=True, timeout=5)


def test_update_mutex_rejects_obsolete_callback_form():
    with pytest.raises(TypeError):
        tunables.TunableRegistry.with_update_mutex(lambda: None)


def test_update_mutex_is_held_through_with_body():
    code = """
import threading

import tunables


holder_entered = threading.Event()
waiter_attempted = threading.Event()
waiter_entered = threading.Event()
errors = []


def holder():
    try:
        with tunables.TunableRegistry.with_update_mutex():
            holder_entered.set()
            assert waiter_attempted.wait(1.0), "waiter did not attempt acquisition"
            assert not waiter_entered.wait(0.1), "waiter entered before holder exited"
    except BaseException as exc:
        errors.append(exc)


def waiter():
    try:
        waiter_attempted.set()
        with tunables.TunableRegistry.with_update_mutex():
            waiter_entered.set()
    except BaseException as exc:
        errors.append(exc)


holder_thread = threading.Thread(target=holder)
holder_thread.start()
assert holder_entered.wait(1.0), "holder did not acquire mutex"
waiter_thread = threading.Thread(target=waiter)
waiter_thread.start()
holder_thread.join(2.0)
waiter_thread.join(2.0)
assert not holder_thread.is_alive(), "holder thread hung"
assert not waiter_thread.is_alive(), "waiter thread hung"
assert waiter_entered.is_set(), "waiter did not acquire after holder exited"
assert errors == []
"""
    subprocess.run([sys.executable, "-c", code], check=True, timeout=5)


def test_update_mutex_waits_do_not_hold_gil():
    code = """
import threading
import time

import tunables


def acquire_context():
    with tunables.TunableRegistry.with_update_mutex():
        pass


def run_waiting_call(waiting_call):
    entered = threading.Event()
    done = threading.Event()

    def holder():
        with tunables.TunableRegistry.with_update_mutex():
            entered.set()
            time.sleep(0.2)

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
    assert not holder_thread.is_alive(), "holder thread hung"
    assert not waiter_thread.is_alive(), "waiter thread hung"
    assert done.is_set()


run_waiting_call(acquire_context)
run_waiting_call(tunables.TunableRegistry.update)
"""
    subprocess.run([sys.executable, "-c", code], check=True, timeout=5)


def test_update_mutex_context_retains_self_until_owner_thread_exit():
    code = """
import gc
import threading
import weakref

import tunables


context = tunables.TunableRegistry.with_update_mutex()
context_ref = weakref.ref(context)
shared = [context]
del context
owner_entered = threading.Event()
external_reference_dropped = threading.Event()
errors = []


def owner():
    try:
        current = shared[0]
        current.__enter__()
        owner_entered.set()
        del current
        assert external_reference_dropped.wait(1.0), "external reference was not dropped"
        retained = context_ref()
        assert retained is not None, "entered context was destroyed on the nonowner thread"
        retained.__exit__(None, None, None)
    except BaseException as exc:
        errors.append(exc)


owner_thread = threading.Thread(target=owner)
owner_thread.start()
assert owner_entered.wait(1.0), "owner did not enter"
shared.clear()
gc.collect()
external_reference_dropped.set()

owner_thread.join(2.0)
assert not owner_thread.is_alive(), "owner thread hung"
assert errors == []
gc.collect()
assert context_ref() is None, "normal exit did not break self-retention"
with tunables.TunableRegistry.with_update_mutex():
    pass
"""
    subprocess.run([sys.executable, "-c", code], check=True, timeout=5)


def test_concurrent_complex_rejection_preserves_accepted_retention():
    code = """
import gc
import threading
import weakref

import tunables
from tunables import _tunables


tunables.TunableRegistry.reset()
backend = tunables.MockTunableBackend()
tunables.TunableRegistry.register_backend("", backend)

first_callback_entered = threading.Event()
second_descriptor_seen = threading.Event()
rejection_warning_entered = threading.Event()
allow_rejection = threading.Event()
errors = []
results = {}


class First(_tunables.ComplexTunable):
    def __init__(self):
        super().__init__()
        self.update_count = 0

    def publish_tunable(self, table):
        table.add_int("value", 1)
        first_callback_entered.set()
        if not second_descriptor_seen.wait(1.0):
            errors.append("second publisher did not enter the helper")

    def update_tunable(self):
        self.update_count += 1


class Second:
    def get_tunable_type(self):
        second_descriptor_seen.set()
        return "Second"

    def publish_tunables(self, table):
        raise AssertionError("rejected publisher callback must not run")


def report_warning(message):
    if message == "Tunable already exists: /same":
        rejection_warning_entered.set()
        if not allow_rejection.wait(1.0):
            errors.append("accepted publisher did not return")


tunables.TunableRegistry.set_report_warning(report_warning)
first = First()
first_ref = weakref.ref(first)


def publish_first():
    results["first"] = tunables.publish("same", first)
    allow_rejection.set()


def publish_second():
    if not first_callback_entered.wait(1.0):
        errors.append("first publisher callback did not run")
        return
    results["second"] = tunables.publish("same", Second())


first_thread = threading.Thread(target=publish_first)
second_thread = threading.Thread(target=publish_second)
first_thread.start()
second_thread.start()
first_thread.join(2.0)
second_thread.join(2.0)
tunables.TunableRegistry.set_report_warning(None)

assert not first_thread.is_alive(), "accepted publisher thread hung"
assert not second_thread.is_alive(), "rejected publisher thread hung"
assert rejection_warning_entered.is_set(), "rejected publisher never reached backend"
assert errors == []
assert results == {"first": True, "second": False}

del first
gc.collect()
retained = first_ref()
assert retained is not None, "accepted first publication was not retained"
assert backend.get_uid("/same") is not None
assert backend.get_value("/same/value") == 1

tunables.TunableRegistry.update()
assert retained.update_count == 1

tunables.TunableRegistry.reset()
"""
    subprocess.run([sys.executable, "-c", code], check=True, timeout=5)


@pytest.mark.parametrize(
    "value, expected",
    [
        (b"a\x00\xff", b"a\x00\xff"),
        (bytearray(b"a\x00\xff"), b"a\x00\xff"),
        (memoryview(b"abcdef"), b"abcdef"),
        (memoryview(b"abcdef")[::2], b"ace"),
        (memoryview(b"abcdef")[::-1], b"fedcba"),
        (memoryview(b"abcdef").cast("B", shape=(2, 3)), b"abcdef"),
        (memoryview(array.array("H", [0x0101, 0xFEFE])), b"\x01\x01\xfe\xfe"),
        (b"", b""),
        (memoryview(array.array("B")), b""),
    ],
)
@pytest.mark.parametrize("operation", ["add", "set", "backend"])
def test_raw_tunable_buffer(backend, value, expected, operation):
    if operation == "add":
        raw = tunables.add("raw", value)
    else:
        raw = tunables.add("raw", b"initial")
        if operation == "set":
            raw.set(value)
        else:
            backend.set_raw("/raw", value)
            tunables.TunableRegistry.update()

    assert raw.get() == expected
    assert backend.get_raw("/raw") == expected


@pytest.mark.parametrize("layout", ["c", "fortran", "strided", "scalar", "empty"])
@pytest.mark.parametrize("operation", ["add", "set", "backend"])
def test_raw_tunable_numpy_memoryview(backend, layout, operation):
    np = pytest.importorskip("numpy")
    value = np.array(
        [[1, 2], [3, 4]], dtype="<u2", order="F" if layout == "fortran" else "C"
    )
    expected = b"\x01\x00\x02\x00\x03\x00\x04\x00"
    if layout == "strided":
        value = value[::-1, ::-1]
        expected = b"\x04\x00\x03\x00\x02\x00\x01\x00"
    elif layout == "scalar":
        value = np.array(258, dtype="<u2")
        expected = b"\x02\x01"
    elif layout == "empty":
        value = value[:0]
        expected = b""

    view = memoryview(value)
    if operation == "add":
        raw = tunables.add("raw", view)
    else:
        raw = tunables.add("raw", b"initial")
        if operation == "set":
            raw.set(view)
        else:
            backend.set_raw("/raw", view)
            tunables.TunableRegistry.update()

    value[...] = 0
    assert raw.get() == expected
    assert backend.get_raw("/raw") == expected


@pytest.mark.parametrize("operation", ["add", "set", "backend"])
def test_raw_tunable_indirect_memoryview(backend, operation):
    tb = pytest.importorskip("_testbuffer")
    value = memoryview(tb.ndarray(list(b"abcdef"), shape=[2, 3], flags=tb.ND_PIL))
    if operation == "add":
        raw = tunables.add("raw", value)
    else:
        raw = tunables.add("raw", b"initial")
        if operation == "set":
            raw.set(value)
        else:
            backend.set_raw("/raw", value)
            tunables.TunableRegistry.update()

    assert raw.get() == b"abcdef"
    assert backend.get_raw("/raw") == b"abcdef"


def test_raw_tunable_buffer_with_explicit_value_type():
    raw = tunables.Tunable(memoryview(array.array("H", [0x0101])), value_type=bytes)
    assert raw.get() == b"\x01\x01"


def test_buffer_rejects_tunable_sequence_element_type():
    with pytest.raises(TypeError, match="element_type.*sequences"):
        tunables.Tunable(memoryview(array.array("B", [1, 2])), element_type=int)


@pytest.mark.parametrize("explicit", [False, True])
def test_numpy_array_keeps_tunable_sequence_dispatch(explicit):
    np = pytest.importorskip("numpy")
    value = np.array([1.5, 2.5], dtype=np.float64)
    options = {"element_type": float} if explicit else {}
    raw = tunables.Tunable(value, **options)
    assert raw.get() == [1.5, 2.5]


@pytest.mark.parametrize("operation", ["add", "set", "backend"])
def test_array_exporter_keeps_tunable_sequence_dispatch(backend, operation):
    value = array.array("H", [1, 255])
    if operation == "add":
        tunable = tunables.add("value", value)
        assert tunable.get() == [1, 255]
    else:
        raw = tunables.add("raw", b"initial")
        if operation == "set":
            raw.set(value)
        else:
            backend.set_raw("/raw", value)
            tunables.TunableRegistry.update()
        assert raw.get() == b"\x01\xff"


def test_raw_backend_still_accepts_integer_iterables(backend):
    raw = tunables.add("raw", b"initial")
    backend.set_raw("/raw", iter([0, 255, 1]))
    tunables.TunableRegistry.update()
    assert raw.get() == b"\x00\xff\x01"
    with pytest.raises(ValueError, match="range 0-255"):
        backend.set_raw("/raw", [256])


def test_raw_tunable_buffer_export_released_after_copy():
    value = bytearray(b"abc")
    raw = tunables.Tunable(value)
    value.extend(b"d")
    assert raw.get() == b"abc"


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


@pytest.mark.parametrize(
    "initial, options",
    [
        (True, {}),
        (1, {}),
        (1.5, {}),
        ("hello", {}),
        (b"abc", {}),
        ([True, False], {}),
        ([1, 2], {}),
        ([1.0, 2.0], {}),
        (["a", "b"], {}),
        ([TunablePoint(1, 2)], {}),
        ([], {"element_type": int}),
        ([], {"element_type": TunablePoint}),
    ],
)
@pytest.mark.parametrize("getter_backed", [False, True])
def test_mutate_rejects_non_struct_values(initial, options, getter_backed):
    getter_calls = []

    def getter():
        getter_calls.append(None)
        return initial

    value = tunables.Tunable(
        initial, getter=getter if getter_backed else None, **options
    )
    getter_calls.clear()
    with pytest.raises(TypeError, match="mutate.*struct"):
        value.mutate()
    assert getter_calls == []
    assert value.get() == initial


def test_publish_int_getter_setter_lifecycle_uses_int32(backend):
    state = {"value": 1, "set_values": []}

    def setter(value: int) -> None:
        state["set_values"].append(value)
        state["value"] = value

    published = tunables.get_table().publish_int(
        "intGetter", lambda: state["value"], setter, robust=True
    )

    assert backend.get_int32("/intGetter") == 1

    backend.set_int32("/intGetter", 3)
    tunables.TunableRegistry.update()

    assert state == {"value": 3, "set_values": [3]}
    assert backend.get_int32("/intGetter") == 3

    del published
    gc.collect()

    state["value"] = 5
    tunables.TunableRegistry.update()

    assert backend.get_int32("/intGetter") == 5


def test_publish_float_getter_setter_lifecycle_uses_float(backend):
    state = {"value": 1.25, "set_values": []}

    def setter(value: float) -> None:
        state["set_values"].append(value)
        state["value"] = value

    published = tunables.get_table().publish_float(
        "floatGetter", lambda: state["value"], setter, robust=True
    )

    assert backend.get_float("/floatGetter") == pytest.approx(1.25)

    backend.set_float("/floatGetter", 3.5)
    tunables.TunableRegistry.update()

    assert state["value"] == pytest.approx(3.5)
    assert state["set_values"] == pytest.approx([3.5])
    assert backend.get_float("/floatGetter") == pytest.approx(3.5)

    del published
    gc.collect()

    state["value"] = 5.75
    tunables.TunableRegistry.update()

    assert backend.get_float("/floatGetter") == pytest.approx(5.75)


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

    backend.set_int32("/getter", 6)
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

    backend.set_int32("/clamped", 10)
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


def test_removed_nested_getter_is_not_refreshed(backend):
    calls = []
    table = tunables.get_table("outer")
    table.publish_int("child//value", lambda: calls.append(1) or 1, lambda _: None)
    assert calls == [1]

    table.remove("child")
    tunables.TunableRegistry.update()

    assert calls == [1]
    assert backend.get_uid("/outer/child/value") is None


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


def test_root_get_table_defaults_to_root():
    assert tunables.get_table().get_path() == "/"


def test_root_get_table_accepts_positional_name():
    assert tunables.get_table("drive").get_path() == "/drive/"


def test_table_paths_route_migrate_and_remove(backend):
    child_backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("/child", child_backend)

    assert tunables.get_table().get_path() == "/"
    assert tunables.get_table("drive").get_path() == "/drive/"
    assert tunables.get_table("drive").get_table("left").get_path() == ("/drive/left/")
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


def test_native_complex_tunable_receives_public_native_table(backend):
    from tunables import _tunables

    assert not hasattr(_tunables, "_NativeTunableTable")
    received: list[tunables.TunableTable] = []

    class NativeComplex(_tunables.ComplexTunable):
        def publish_tunable(self, table: tunables.TunableTable) -> None:
            received.append(table)
            table.add_int("value", 7)

    value = NativeComplex()
    assert tunables.publish("native", value) is True
    assert type(received[0]) is tunables.TunableTable
    assert received[0].get_path() == "/native/"
    assert backend.get_value("/native/value") == 7


def test_native_tunable_table_constructor_and_child(backend):
    table = tunables.TunableTable("/manual/")
    child = table.get_table("child")

    assert type(table) is tunables.TunableTable
    assert type(child) is tunables.TunableTable
    assert table.get_path() == "/manual/"
    assert child.get_path() == "/manual/child/"
    assert child.add_double("value", 1.5).get() == pytest.approx(1.5)
    assert backend.get_value("/manual/child/value") == pytest.approx(1.5)


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


def test_reentrant_global_complex_replacement_retains_newer_value(backend):
    replacement_refs = []

    class Replacement:
        def __init__(self) -> None:
            self.update_count = 0

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.add_int("value", 11)

        def update_tunables(self) -> None:
            self.update_count += 1

    class ReentrantComplex:
        def publish_tunables(self, table: tunables.TunableTable) -> None:
            replacement = Replacement()
            replacement_refs.append(weakref.ref(replacement))
            tunables.remove("same")
            assert tunables.publish("same", replacement) is True

    obsolete = ReentrantComplex()
    obsolete_ref = weakref.ref(obsolete)

    assert tunables.publish("same", obsolete) is True
    del obsolete

    assert obsolete_ref() is None
    replacement = replacement_refs[0]()
    assert replacement is not None
    assert backend.get_value("/same/value") == 11

    tunables.TunableRegistry.update()
    assert replacement.update_count == 1


def test_reentrant_owner_scoped_complex_replacement_retains_newer_value(backend):
    obsolete_refs = []
    replacement_refs = []

    class Replacement:
        def __init__(self) -> None:
            self.update_count = 0

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            table.add_int("value", 12)

        def update_tunables(self) -> None:
            self.update_count += 1

    class ReentrantChild:
        def __init__(self, parent_table: tunables.TunableTable) -> None:
            self.parent_table = parent_table

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            replacement = Replacement()
            replacement_refs.append(weakref.ref(replacement))
            self.parent_table.remove("child")
            assert self.parent_table.publish("child", replacement) is True

    class Parent:
        def publish_tunables(self, table: tunables.TunableTable) -> None:
            obsolete = ReentrantChild(table)
            obsolete_refs.append(weakref.ref(obsolete))
            assert table.publish("child", obsolete) is True

    assert tunables.publish("parent", Parent()) is True

    assert obsolete_refs[0]() is None
    replacement = replacement_refs[0]()
    assert replacement is not None
    assert backend.get_value("/parent/child/value") == 12

    tunables.TunableRegistry.update()
    assert replacement.update_count == 1


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


def test_registry_remove_accepts_path_or_bound_value(backend):
    first = tunables.add("first", 1)
    second = tunables.add("second", 2)
    tunables.TunableRegistry.remove("first")
    tunables.TunableRegistry.remove(second)
    assert backend.get_uid("/first") is None
    assert backend.get_uid("/second") is None


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


def test_retained_duck_complex_table_rejects_remove_after_parent_removal(backend):
    class RetainingComplex:
        def __init__(self) -> None:
            self.table: tunables.TunableTable | None = None

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            self.table = table

    value = RetainingComplex()
    assert tunables.publish("complex", value) is True
    assert value.table is not None
    retained_table = value.table

    tunables.remove("complex")

    retained_child = retained_table.get_table("nested")
    assert retained_child.get_path() == "/complex/nested/"
    tunables.add_int("complex/nested/value", 7)
    replacement_uid = backend.get_uid("/complex/nested/value")
    assert replacement_uid is not None

    error = None
    try:
        retained_child.remove("value")
    except RuntimeError as exc:
        error = exc

    assert backend.get_uid("/complex/nested/value") == replacement_uid
    assert error is not None
    assert str(error) == "callback TunableTable owner is no longer valid"


def test_manual_table_in_weakref_callback_does_not_inherit_owner(backend):
    class RetainingComplex:
        def __init__(self) -> None:
            self.table: tunables.TunableTable | None = None

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            self.table = table

    owner = RetainingComplex()
    assert tunables.publish("owner", owner) is True
    assert owner.table is not None

    manual_tables = []

    def allocate_manual_table(_ref: weakref.ReferenceType) -> None:
        table = tunables.TunableTable("/manual/")
        table.add_int("value", 7)
        manual_tables.append(table)

    table_ref = weakref.ref(owner.table, allocate_manual_table)
    owner.table = None

    assert table_ref() is None
    assert len(manual_tables) == 1
    manual_uid = backend.get_uid("/manual/value")
    assert manual_uid is not None

    tunables.remove("owner")

    assert backend.get_uid("/manual/value") == manual_uid
    assert backend.get_value("/manual/value") == 7


def test_stale_duck_table_is_not_revived_by_path_reuse(backend):
    class RetainingComplex:
        def __init__(self) -> None:
            self.table: tunables.TunableTable | None = None

        def publish_tunables(self, table: tunables.TunableTable) -> None:
            self.table = table

    first = RetainingComplex()
    assert tunables.publish("same", first) is True
    stale = first.table
    assert stale is not None
    tunables.remove("same")

    second = RetainingComplex()
    assert tunables.publish("same", second) is True
    second_table = second.table
    assert second_table is not None
    second_table.add_int("live", 9)

    with pytest.raises(
        RuntimeError, match="callback TunableTable owner is no longer valid"
    ):
        stale.remove("live")

    assert backend.get_value("/same/live") == 9


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


@pytest.mark.parametrize("getter_backed", [False, True])
def test_mutate_marks_struct_tunables_changed(backend, getter_backed):
    original = TunablePoint(1, 2)
    point = tunables.Tunable(
        original, getter=(lambda: original) if getter_backed else None
    )
    tunables.publish("point", point)
    tunables.TunableRegistry.update()

    edited = point.mutate()
    assert edited is original
    edited.a = 5
    tunables.TunableRegistry.update()

    assert point.get() is original
    assert backend.get_value("/point") == wpistruct.pack(TunablePoint(5, 2))


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
