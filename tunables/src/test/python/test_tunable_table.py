import pytest

import tunables


@pytest.fixture
def backend():
    tunables.TunableRegistry.reset()
    backend = tunables.MockTunableBackend()
    tunables.TunableRegistry.register_backend("", backend)
    try:
        yield backend
    finally:
        tunables.TunableRegistry.reset()


@pytest.mark.parametrize("scope", ["root", "table"])
@pytest.mark.parametrize(
    (
        "helper_name",
        "initial",
        "remote",
        "backend_getter",
        "backend_setter",
        "python_type",
    ),
    [
        ("add_boolean", True, False, "get_bool", "set_bool", bool),
        ("add_int", 2_000_000_001, -2_000_000_001, "get_int32", "set_int32", int),
        (
            "add_long",
            5_000_000_001,
            -5_000_000_001,
            "get_int64",
            "set_int64",
            int,
        ),
        ("add_float", 1.25, -3.5, "get_float", "set_float", float),
        ("add_double", 1.25, -3.5, "get_double", "set_double", float),
    ],
)
def test_scalar_add_helpers_preserve_native_type_and_callback_values(
    backend,
    scope,
    helper_name,
    initial,
    remote,
    backend_getter,
    backend_setter,
    python_type,
):
    tuned_values = []
    if scope == "root":
        add = getattr(tunables, helper_name)
        name = f"root/{helper_name}"
        path = f"/{name}"
    else:
        add = getattr(tunables.get_table("table"), helper_name)
        name = helper_name
        path = f"/table/{name}"

    value = add(name, initial, on_tune=tuned_values.append)

    assert type(value.get()) is python_type
    assert getattr(backend, backend_getter)(path) == pytest.approx(initial)

    getattr(backend, backend_setter)(path, remote)
    tunables.TunableRegistry.update()

    assert tuned_values == pytest.approx([remote])
    assert type(tuned_values[0]) is python_type


@pytest.mark.parametrize(
    (
        "helper_name",
        "initial",
        "remote",
        "local",
        "backend_getter",
        "backend_setter",
        "python_type",
        "element_type",
    ),
    [
        (
            "publish_boolean",
            True,
            False,
            True,
            "get_bool",
            "set_bool",
            bool,
            None,
        ),
        (
            "publish_int",
            2_000_000_001,
            -2_000_000_001,
            123,
            "get_int32",
            "set_int32",
            int,
            None,
        ),
        (
            "publish_long",
            5_000_000_001,
            -5_000_000_001,
            456,
            "get_int64",
            "set_int64",
            int,
            None,
        ),
        (
            "publish_float",
            1.25,
            -3.5,
            5.75,
            "get_float",
            "set_float",
            float,
            None,
        ),
        (
            "publish_double",
            1.25,
            -3.5,
            5.75,
            "get_double",
            "set_double",
            float,
            None,
        ),
        (
            "publish_string",
            "initial",
            "remote",
            "local",
            "get_string",
            "set_string",
            str,
            None,
        ),
        (
            "publish_raw",
            b"initial",
            b"remote",
            b"local",
            "get_raw",
            "set_raw",
            bytes,
            None,
        ),
        (
            "publish_boolean_array",
            [True, False],
            [False, True],
            [True, True],
            "get_bool_vector",
            "set_bool_vector",
            list,
            bool,
        ),
        (
            "publish_integer_array",
            [5_000_000_001, -5_000_000_001],
            [-6_000_000_001, 6_000_000_001],
            [7, 8],
            "get_int64_vector",
            "set_int64_vector",
            list,
            int,
        ),
        (
            "publish_double_array",
            [1.25, -3.5],
            [-5.75, 7.0],
            [8.25, 9.5],
            "get_double_vector",
            "set_double_vector",
            list,
            float,
        ),
        (
            "publish_string_array",
            ["initial", "values"],
            ["remote", "values"],
            ["local", "values"],
            "get_string_vector",
            "set_string_vector",
            list,
            str,
        ),
    ],
)
def test_typed_publish_helpers_preserve_getter_and_setter_values(
    backend,
    helper_name,
    initial,
    remote,
    local,
    backend_getter,
    backend_setter,
    python_type,
    element_type,
):
    state = {"value": initial}
    set_values = []

    def setter(value):
        set_values.append(value)
        state["value"] = value

    table = tunables.get_table("published")
    publish = getattr(table, helper_name)
    published = publish(helper_name, lambda: state["value"], setter, robust=True)
    path = f"/published/{helper_name}"

    assert published.get() == initial
    assert getattr(backend, backend_getter)(path) == initial

    getattr(backend, backend_setter)(path, remote)
    tunables.TunableRegistry.update()

    assert set_values == [remote]
    assert type(set_values[0]) is python_type
    if element_type is not None:
        assert all(type(value) is element_type for value in set_values[0])

    state["value"] = local
    tunables.TunableRegistry.update()

    assert published.get() == local
    assert getattr(backend, backend_getter)(path) == local


@pytest.mark.parametrize("scope", ["root", "table"])
@pytest.mark.parametrize(
    ("helper_name", "minimum", "maximum"),
    [
        ("add_int", -(2**31), 2**31 - 1),
        ("add_long", -(2**63), 2**63 - 1),
    ],
)
def test_integer_add_helpers_enforce_native_range(
    backend, scope, helper_name, minimum, maximum
):
    owner = tunables if scope == "root" else tunables.get_table("table")
    add = getattr(owner, helper_name)

    assert add(f"{helper_name}Minimum", minimum).get() == minimum
    assert add(f"{helper_name}Maximum", maximum).get() == maximum

    with pytest.raises((TypeError, RuntimeError)):
        add(f"{helper_name}Underflow", minimum - 1)
    with pytest.raises((TypeError, RuntimeError)):
        add(f"{helper_name}Overflow", maximum + 1)
