import threading
import weakref

from typing import Callable, Dict, Optional, Sequence

from ._ntcore import NetworkTableInstance, NetworkTableEntry, NetworkTableType, Value

__all__ = ["ntproperty", "ChooserControl"]


class _NtProperty:
    """
    Don't use this directly, use @ntproperty instead
    """

    entry: NetworkTableEntry
    default_value: Value

    _instlock = threading.Lock()
    _instances: Dict[int, weakref.WeakSet] = {}

    @classmethod
    def attach(cls, self: "_NtProperty", inst: NetworkTableInstance):
        with cls._instlock:
            handle = inst._get_handle()
            props = cls._instances.get(handle)
            if not props:
                props = weakref.WeakSet()
                cls._instances[handle] = props
            props.add(self)

    @classmethod
    def on_instance_post_reset(cls, inst: NetworkTableInstance):
        with cls._instlock:
            props = cls._instances.get(inst._get_handle(), [])
            for prop in props:
                prop.reset()

    @classmethod
    def on_instance_destroy(cls, inst: NetworkTableInstance):
        with cls._instlock:
            cls._instances.pop(inst._get_handle(), None)

    def __init__(
        self,
        key: str,
        default_value,
        write_default: bool,
        persistent: bool,
        type: Optional[NetworkTableType],
        inst: NetworkTableInstance,
    ) -> None:
        # Autodetect the type if not provided, and store the default
        # value as that specific type
        if type is None:
            self.default_value = Value.make_value(default_value)
            self.mkv = Value.get_factory_by_type(self.default_value.type())
        else:
            self.mkv = Value.get_factory_by_type(type)
            self.default_value = self.mkv(default_value)

        self.key = key
        self.write_default = write_default
        self.persistent = persistent
        # never overwrite persistent values with defaults
        if persistent:
            self.write_default = False
        self.inst = inst
        _NtProperty.attach(self, inst)

        # Set it up
        self.reset()

    def reset(self):
        self.entry = self.inst.get_entry(self.key)
        if self.write_default:
            print("set v", self.default_value)
            self.entry.set_value(self.default_value)
        else:
            print("set default", self.default_value)
            self.entry.set_default_value(self.default_value)

        if self.persistent:
            self.entry.set_persistent()

    def get(self, _):
        return self.entry.value

    def set(self, _, value):
        self.entry.set_value(self.mkv(value))


def ntproperty(
    key: str,
    default_value,
    *,
    write_default: bool = True,
    doc: str = None,
    persistent: bool = False,
    type: Optional[NetworkTableType] = None,
    inst: Optional[NetworkTableInstance] = None
) -> property:
    """
    A property that you can add to your classes to access NetworkTables
    variables like a normal variable.

    :param key: A full NetworkTables key (eg ``/SmartDashboard/foo``)
    :param default_value: Default value to use if not in the table
    :type  default_value: any
    :param write_default: If True, put the default value to the table,
                          overwriting existing values
    :param doc: If given, will be the docstring of the property.
    :param persistent: If True, persist set values across restarts.
                       *write_default* is ignored if this is True.
    :param type: Specify the type of this entry. If not specified,
                 will autodetect the type from the default value
    :param inst: The NetworkTables instance to use.

    Example usage::

        class Foo(object):

            something = ntproperty('/SmartDashboard/something', True)

            ...

            def do_thing(self):
                if self.something:    # reads from value
                    ...

                    self.something = False # writes value

    .. note:: When using empty lists/tuples, you must explicitly specify
              the type.

    .. warning::

       This function assumes that the value's type
       never changes. If it does, you'll get really strange
       errors... so don't do that.
    """
    if inst is None:
        inst = NetworkTableInstance.get_default()

    ntprop = _NtProperty(key, default_value, write_default, persistent, type, inst)
    return property(fget=ntprop.get, fset=ntprop.set, doc=doc)


class ChooserControl:
    """
    Interacts with a :class:`wpilib.SendableChooser`
    object over NetworkTables.
    """

    def __init__(
        self,
        key: str,
        on_choices: Optional[Callable[[Sequence[str]], None]] = None,
        on_selected: Optional[Callable[[str], None]] = None,
        *,
        inst: Optional[NetworkTableInstance] = None
    ) -> None:
        """
        :param key: NetworkTables key
        :param on_choices: A function that will be called when the
                           choices change.
        :param on_selection: A function that will be called when the
                             selection changes.
        :param inst: The NetworkTables instance to use.
        """

        if inst is None:
            inst = NetworkTableInstance.get_default()

        self.subtable = inst.get_table("SmartDashboard").get_sub_table(key)

        self.on_choices = on_choices
        self.on_selected = on_selected

        if on_choices or on_selected:
            self.subtable.add_table_listener(self._on_change, True)

    def close(self) -> None:
        """Stops listening for changes to the ``SendableChooser``"""
        if self.on_choices or self.on_selected:
            self.subtable.remove_table_listener(self._on_change)

    def get_choices(self) -> Sequence[str]:
        """
        Returns the current choices. If the chooser doesn't exist, this
        will return an empty tuple.
        """
        return self.subtable.get_string_array("options", [])

    def get_selected(self) -> Optional[str]:
        """
        Returns the current selection or None
        """
        selected = self.subtable.get_string("selected", None)
        if selected is None:
            selected = self.subtable.get_string("default", None)
        return selected

    def set_selected(self, selection: str) -> None:
        """
        Sets the active selection on the chooser

        :param selection: Active selection name
        """
        self.subtable.put_string("selected", selection)

    def _on_change(self, table, key, value, is_new):
        if key == "options":
            if self.on_choices is not None:
                self.on_choices(value)
        elif key == "selected":
            if self.on_selected is not None:
                self.on_selected(value)
        elif key == "default":
            if (
                self.on_selected is not None
                and self.subtable.get_string("selected", None) is None
            ):
                self.on_selected(value)
