import weakref

import pytest

import tunable


@pytest.fixture
def backend():
    tunable.TunableRegistry.reset()
    backend = tunable.MockTunableBackend()
    tunable.TunableRegistry.register_backend("", backend)
    try:
        yield backend
    finally:
        tunable.TunableRegistry.reset()


def test_tunable_get_set():
    value = tunable.Tunable(1)

    assert value.get() == 1
    value.set(2)
    assert value.get() == 2


def test_backend_updates_tunable(backend):
    value = tunable.Tunables.add("value", 1)

    backend.set_int64("/value", 3)
    tunable.TunableRegistry.update()

    assert value.get() == 3
    tunable.Tunables.remove("value")


def test_publish_value_uses_getter(backend):
    value = [1]

    published = tunable.Tunables.get_table().publish_int(
        "getter", lambda: value[0], lambda tuned: value.__setitem__(0, tuned)
    )

    value[0] = 4
    tunable.TunableRegistry.update()
    assert published.get() == 4

    backend.set_int64("/getter", 6)
    tunable.TunableRegistry.update()
    assert value[0] == 6
    tunable.Tunables.remove("getter")


def test_publish_retains_complex_tunable(backend):
    value = tunable.Selectable()
    value.add_default("option", True)
    ref = weakref.ref(value)

    tunable.Tunables.publish("selectable", value)
    del value

    assert bool(ref) is True
    tunable.Tunables.remove("selectable")


@pytest.fixture
def chooser() -> tunable.Selectable[int]:
    chooser = tunable.Selectable()
    for i in range(1, 4):
        chooser.add(str(i), i)
    return chooser


@pytest.mark.parametrize("value", [0, 1, 2, 3])
def test_selectable_returns_selected(
    backend: tunable.MockTunableBackend,
    chooser: tunable.Selectable[int],
    value: int,
):
    chooser.add_default("0", 0)
    name = f"ReturnsSelectedChooser{value}"

    tunable.Tunables.publish(name, chooser)
    backend.set_string(f"/{name}/selected", str(value))
    tunable.TunableRegistry.update()

    assert value == chooser.get_selected()
    tunable.Tunables.remove(name)


def test_selectable_default_is_returned_on_no_select(
    chooser: tunable.Selectable[int],
):
    chooser.add_default("4", 4)
    assert 4 == chooser.get_selected()


def test_selectable_default_constructable_is_returned_on_no_select_and_no_default(
    chooser: tunable.Selectable[int],
):
    assert chooser.get_selected() is None


def test_selectable_change_listener(
    backend: tunable.MockTunableBackend,
    chooser: tunable.Selectable[int],
):
    current_val = [0]

    def on_change(val):
        current_val[0] = val

    chooser.on_change(on_change)
    tunable.Tunables.publish("ChangeListenerChooser", chooser)
    backend.set_string("/ChangeListenerChooser/selected", "3")
    tunable.TunableRegistry.update()

    assert 3 == current_val[0]
    tunable.Tunables.remove("ChangeListenerChooser")
