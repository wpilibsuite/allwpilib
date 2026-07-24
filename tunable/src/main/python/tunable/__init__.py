from . import _init__tunable

from abc import ABC, abstractmethod
from collections.abc import Callable
from typing import Generic, TypeVar

from ._tunable import (
    MockTunableBackend,
    Tunable,
    TunableBackend,
    TunableRegistry,
    TunableTable,
    Tunables,
)


class ComplexTunable(ABC):
    """An object composed of one or more tunable values."""

    @abstractmethod
    def publish_tunable(self, table: TunableTable) -> None:
        """Publishes this object's tunable values to ``table``."""

    def get_tunable_type(self) -> str:
        """Returns this object's stable tunable type, or an empty string."""
        return ""

    def update_tunable(self) -> None:
        """Updates callback-backed values before they are sent to the backend."""


T = TypeVar("T")


class Selectable(ComplexTunable, Generic[T]):
    """Presents a named collection of arbitrary Python objects as a tunable."""

    def __init__(self) -> None:
        self._values: dict[str, T] = {}
        self._listener: Callable[[T], None] | None = None
        self._default = Tunable("", mutable=False)
        self._options = Tunable([], mutable=False, value_type="string[]")
        self._selected = Tunable("", on_tune=self._changed, robust=True)

    def add(self, name: str, value: T) -> None:
        """Adds or replaces an option."""
        self._values[name] = value
        self._options.set(list(self._values))

    def add_default(self, name: str, value: T) -> None:
        """Adds an option and makes it the default."""
        self.add(name, value)
        self.set_default(name)

    def set_default(self, name: str) -> None:
        """Sets the default option by name."""
        self._default.set(name)

    def clear(self) -> None:
        """Clears all options and the default selection."""
        self._values.clear()
        self._options.set([])
        self._default.set("")

    def get_selected(self) -> T | None:
        """Returns the selected option, falling back to the default."""
        selected = self._selected.get() or self._default.get()
        return self._values.get(selected)

    def on_change(self, listener: Callable[[T], None]) -> None:
        """Sets the listener called when a valid option is selected."""
        self._listener = listener

    def publish_tunable(self, table: TunableTable) -> None:
        table.publish("default", self._default)
        table.publish("options", self._options)
        table.publish("selected", self._selected)

    def get_tunable_type(self) -> str:
        return "Selectable"

    def _changed(self, _name: str) -> None:
        selected = self.get_selected()
        if selected is not None and self._listener is not None:
            self._listener(selected)

__all__ = [
    "ComplexTunable",
    "MockTunableBackend",
    "Selectable",
    "Tunable",
    "TunableBackend",
    "TunableRegistry",
    "TunableTable",
    "Tunables",
]

try:
    from .version import version as __version__
except ImportError:
    __version__ = "master"

del _init__tunable
