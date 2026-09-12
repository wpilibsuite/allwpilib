import pytest
from wpiutil_test import module
import array
import sys
import weakref


@pytest.mark.parametrize("values", [[], [1, 2, 3], [1, 2, 3, 4], list(range(40))])
def test_span_load_int(values):
    assert module.load_span_int(values) == values


@pytest.mark.parametrize("fixed", [False, True])
@pytest.mark.parametrize("values", ["123", None, [1, object(), 3]])
def test_span_rejects_invalid_sequence(fixed, values):
    function = module.load_span_fixed_double if fixed else module.load_span_int
    with pytest.raises(TypeError):
        function(values)


@pytest.mark.parametrize("fixed", [False, True])
def test_span_load_tuple(fixed):
    function = module.load_span_fixed_double if fixed else module.load_span_int
    assert list(function((1, 2, 3))) == [1, 2, 3]


def test_span_load_bool():
    assert module.load_span_bool([True, False, True]) == [True, False, True]


def test_span_load_string():
    assert module.load_span_string(["a", "b", "c"]) == ["a", "b", "c"]


def test_span_load_string_const():
    assert module.load_span_string_const(["a", "b", "c"]) == ["a", "b", "c"]


def test_span_load_stringview():
    assert module.load_span_string_view(["a", "b", "c"]) == ["a", "b", "c"]


def test_span_load_vector():
    assert module.load_span_vector([["a"], ["b"], ["c"]]) == [["a"], ["b"], ["c"]]


@pytest.mark.parametrize("values", [b"", b"abc", memoryview(b"abcdef")[1:4]])
def test_span_load_buffer_bytes(values):
    assert module.load_span_bytes(values) == bytes(values)


def test_span_modify_buffer_bytes():
    b = b"abc"
    with pytest.raises(BufferError):
        module.modify_span_buffer(b)


def test_span_load_buffer_bytearray():
    assert module.load_span_bytes(bytearray([1, 2, 3])) == b"\x01\x02\x03"


def test_span_modify_buffer_bytearray():
    b = bytearray([1, 2, 3])
    module.modify_span_buffer(b)
    assert b == bytearray([4, 2, 3])


def test_span_load_buffer_array():
    a = array.array("l")
    a.append(1)
    a2 = array.array("l")
    a2.frombytes(module.load_span_bytes(a))
    assert len(a2) == 1
    assert a2[0] == 1


def test_span_modify_multibyte_buffer():
    values = array.array("l", [123456])
    expected = bytearray(values.tobytes())
    expected[0] = 4
    module.modify_span_buffer(values)
    assert values.tobytes() == bytes(expected)


@pytest.mark.parametrize("writable", [False, True])
@pytest.mark.parametrize("stride", [2, -1])
def test_span_rejects_strided_buffers(writable, stride):
    # Leave enough allocation after the view's starting pointer that even the
    # broken contiguous interpretation cannot access outside the allocation.
    data = bytearray(b"abcdef")
    view = memoryview(data)[::2] if stride == 2 else memoryview(data)[2::-1]
    function = module.modify_span_buffer if writable else module.load_span_bytes
    with pytest.raises(TypeError):
        function(view)
    assert data == b"abcdef"


@pytest.mark.parametrize("writable", [False, True])
def test_span_rejects_multidimensional_buffers(writable):
    view = memoryview(bytearray(b"abcdef")).cast("B", shape=[2, 3])
    function = module.modify_span_buffer if writable else module.load_span_bytes
    with pytest.raises(TypeError):
        function(view)


@pytest.mark.skipif(sys.version_info < (3, 12), reason="Python buffer protocol")
@pytest.mark.parametrize("writable", [False, True])
def test_span_keeps_buffer_export_alive(writable):
    class Exporter:
        def __init__(self):
            self.data = bytearray(b"abc")
            self.released = None

        def __buffer__(self, flags):
            return memoryview(self.data)

        def __release_buffer__(self, view):
            self.released = bytes(view)
            self.data[:] = b"xyz"

    exporter = Exporter()
    if writable:
        module.modify_span_buffer(exporter)
        assert exporter.released == b"\x04bc"
    else:
        assert module.load_span_bytes(exporter) == b"abc"
        assert exporter.released == b"abc"
    assert exporter.data == b"xyz"


def test_span_cast():
    assert module.cast_span() == [1, 2, 3]


def test_string_span():
    assert module.cast_string_span() == ["hi", "there"]


def test_fixed_double_span():
    assert module.load_span_fixed_double([1, 2, 3]) == (1, 2, 3)


@pytest.mark.parametrize("values", [[], [1, 2], [1, 2, 3, 4]])
def test_fixed_double_span_rejects_wrong_length(values):
    with pytest.raises(TypeError):
        module.load_span_fixed_double(values)


def test_span_zero_extent():
    assert module.span_zero([]) == ()
    assert module.span_zero(()) == ()
    with pytest.raises(TypeError):
        module.span_zero([1])


def test_fixed_span_checks_snapshot_length():
    class ChangingSequence:
        def __init__(self):
            self.length_calls = 0

        def __len__(self):
            self.length_calls += 1
            return 3 if self.length_calls == 1 else 2

        def __getitem__(self, index):
            if index >= 2:
                raise IndexError
            return index + 1

    with pytest.raises(TypeError):
        module.load_span_fixed_double(ChangingSequence())


@pytest.mark.parametrize("fixed", [False, True])
def test_span_retains_generated_strings(fixed):
    class TrackedString(str):
        pass

    class GeneratedSequence:
        def __init__(self):
            self.refs = []

        def __len__(self):
            return 3

        def __getitem__(self, index):
            if index >= 3:
                raise IndexError
            value = TrackedString(f"generated string {index}")
            self.refs.append(weakref.ref(value))
            return value

    values = GeneratedSequence()

    def check_lifetimes():
        # Check before native code reads the views: fail safely, without a UAF.
        assert len(values.refs) == 3
        assert all(ref() is not None for ref in values.refs)

    function = (
        module.fixed_span_string_views_with_callback
        if fixed
        else module.span_string_views_with_callback
    )
    assert list(function(values, check_lifetimes)) == [
        "generated string 0",
        "generated string 1",
        "generated string 2",
    ]
    assert all(ref() is None for ref in values.refs)


@pytest.mark.parametrize("fixed", [False, True])
def test_span_snapshot_survives_mutation_during_conversion(fixed):
    class MutatingNumber:
        def __float__(self):
            values.clear()
            return 1.0

    values = [MutatingNumber(), 2, 3]
    function = (
        module.load_span_fixed_double if fixed else module.load_span_dynamic_double
    )
    assert list(function(values)) == [1.0, 2.0, 3.0]
    assert values == []


@pytest.mark.parametrize("fixed", [False, True])
def test_span_snapshot_retains_removed_strings(fixed):
    class TrackedString(str):
        pass

    values = [TrackedString(word * 20) for word in ["first ", "second ", "third "]]
    refs = [weakref.ref(value) for value in values]

    def remove_strings():
        values.clear()
        assert all(ref() is not None for ref in refs)

    function = (
        module.fixed_span_string_views_with_callback
        if fixed
        else module.span_string_views_with_callback
    )
    assert list(function(values, remove_strings)) == [
        "first " * 20,
        "second " * 20,
        "third " * 20,
    ]
    assert values == []


@pytest.mark.parametrize("fixed", [False, True])
def test_span_retains_element_casters(fixed):
    function = (
        module.fixed_span_caster_owned_views
        if fixed
        else module.span_caster_owned_views
    )
    assert list(function(["one", "two", "three"])) == ["one", "two", "three"]


def test_span_retains_heap_backed_element_casters():
    assert module.span_caster_owned_views(["owned"] * 40) == ["owned"] * 40


@pytest.mark.parametrize("writable", [False, True])
@pytest.mark.parametrize("values", [bytearray(), bytearray(b"abc")])
def test_span_buffer_return_is_bytes_copy(writable, values):
    values = values.copy()
    function = (
        module.writable_span_bytes_with_callback
        if writable
        else module.span_bytes_with_callback
    )
    result = function(values, lambda: None)
    assert isinstance(result, bytes)
    assert result == bytes(values)
    values[:] = b"changed"
    assert result != bytes(values)


@pytest.mark.parametrize("writable", [False, True])
@pytest.mark.parametrize("selection", [slice(1, 2, 4), slice(0, 0, 2)])
def test_span_accepts_contiguous_degenerate_buffer(writable, selection):
    # Empty and single-element views are contiguous regardless of their stride.
    data = bytearray(b"abcdef")
    view = memoryview(data)[selection]
    function = (
        module.writable_span_bytes_with_callback
        if writable
        else module.span_bytes_with_callback
    )
    assert function(view, lambda: None) == view.tobytes()


@pytest.mark.parametrize("writable", [False, True])
def test_span_buffer_export_prevents_resize_during_call(writable):
    values = bytearray(b"abc")

    def try_resize():
        with pytest.raises(BufferError):
            values.extend(b"def")

    function = (
        module.writable_span_bytes_with_callback
        if writable
        else module.span_bytes_with_callback
    )
    assert function(values, try_resize) == b"abc"
    values.extend(b"def")
    assert values == b"abcdef"


@pytest.mark.parametrize("writable", [False, True])
def test_span_releases_export_after_exception(writable):
    values = bytearray(b"abc")

    def fail():
        raise ValueError("callback failed")

    function = (
        module.writable_span_bytes_with_callback
        if writable
        else module.span_bytes_with_callback
    )
    with pytest.raises(ValueError, match="callback failed"):
        function(values, fail)
    values.extend(b"def")
    assert values == b"abcdef"


@pytest.mark.parametrize("fixed", [False, True])
@pytest.mark.parametrize("const", [False, True])
@pytest.mark.parametrize("policy", ["", "_copy", "_reference", "_reference_internal"])
def test_span_return_does_not_move_from_owner(fixed, const, policy):
    owner = module.SpanOwner()
    name = ("const_" if const else "") + ("fixed_span" if fixed else "span") + policy
    result = getattr(owner, name)()
    assert [item.value for item in result] == [11, 22]
    assert owner.values() == (11, 22)

    result[0].value = 99
    if policy in ("_reference", "_reference_internal"):
        assert owner.values() == (99, 22)
        owner_ref = weakref.ref(owner)
        del owner
        if policy == "_reference_internal":
            assert owner_ref() is not None
        else:
            assert owner_ref() is None
        del result
        assert owner_ref() is None
    else:
        assert owner.values() == (11, 22)


@pytest.mark.parametrize("fixed", [False, True])
@pytest.mark.parametrize("const", [False, True])
def test_span_return_honors_explicit_move(fixed, const):
    owner = module.SpanOwner()
    name = ("const_" if const else "") + ("fixed_span" if fixed else "span") + "_move"
    result = getattr(owner, name)()
    assert [item.value for item in result] == [11, 22]
    assert owner.values() == (-1, -1)


@pytest.mark.parametrize("fixed", [False, True])
def test_direct_span_cast_honors_explicit_copy(fixed):
    owner = module.SpanOwner()
    result = owner.cast_fixed_span_copy() if fixed else owner.cast_span_copy()
    assert [item.value for item in result] == [11, 22]
    assert owner.values() == (11, 22)
    result[0].value = 99
    assert owner.values() == (11, 22)
