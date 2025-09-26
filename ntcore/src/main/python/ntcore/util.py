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
    defaultValue: Value

    _instlock = threading.Lock()
    _instances: Dict[int, weakref.WeakSet] = {}

    @classmethod
    def attach(cls, self: "_NtProperty", inst: NetworkTableInstance):
        with cls._instlock:
            handle = inst._getHandle()
            props = cls._instances.get(handle)
            if not props:
                props = weakref.WeakSet()
                cls._instances[handle] = props
            props.add(self)

    @classmethod
    def onInstancePostReset(cls, inst: NetworkTableInstance):
        with cls._instlock:
            props = cls._instances.get(inst._getHandle(), [])
            for prop in props:
                prop.reset()

    @classmethod
    def onInstanceDestroy(cls, inst: NetworkTableInstance):
        with cls._instlock:
            cls._instances.pop(inst._getHandle(), None)

    def __init__(
        self,
        key: str,
        defaultValue,
        writeDefault: bool,
        persistent: bool,
        type: Optional[NetworkTableType],
        inst: NetworkTableInstance,
    ) -> None:
        # Autodetect the type if not provided, and store the default
        # value as that specific type
        if type is None:
            self.defaultValue = Value.makeValue(defaultValue)
            self.mkv = Value.getFactoryByType(self.defaultValue.type())
        else:
            self.mkv = Value.getFactoryByType(type)
            self.defaultValue = self.mkv(defaultValue)

        self.key = key
        self.writeDefault = writeDefault
        self.persistent = persistent
        # never overwrite persistent values with defaults
        if persistent:
            self.writeDefault = False
        self.inst = inst
        _NtProperty.attach(self, inst)

        # Set it up
        self.reset()

    def reset(self):
        self.entry = self.inst.getEntry(self.key)
        if self.writeDefault:
            print("set v", self.defaultValue)
            self.entry.setValue(self.defaultValue)
        else:
            print("set default", self.defaultValue)
            self.entry.setDefaultValue(self.defaultValue)

        if self.persistent:
            self.entry.setPersistent()

    def get(self, _):
        return self.entry.value

    def set(self, _, value):
        self.entry.setValue(self.mkv(value))


def ntproperty(
    key: str,
    defaultValue,
    *,
    writeDefault: bool = True,
    doc: str = None,
    persistent: bool = False,
    type: Optional[NetworkTableType] = None,
    inst: Optional[NetworkTableInstance] = None
) -> property:
    """
    A property that you can add to your classes to access NetworkTables
    variables like a normal variable.

    :param key: A full NetworkTables key (eg ``/SmartDashboard/foo``)
    :param defaultValue: Default value to use if not in the table
    :type  defaultValue: any
    :param writeDefault: If True, put the default value to the table,
                         overwriting existing values
    :param doc: If given, will be the docstring of the property.
    :param persistent: If True, persist set values across restarts.
                       *writeDefault* is ignored if this is True.
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
        inst = NetworkTableInstance.getDefault()

    ntprop = _NtProperty(key, defaultValue, writeDefault, persistent, type, inst)
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
            inst = NetworkTableInstance.getDefault()

        self.subtable = inst.getTable("SmartDashboard").getSubTable(key)

        self.on_choices = on_choices
        self.on_selected = on_selected

        if on_choices or on_selected:
            self.subtable.addTableListener(self._on_change, True)

    def close(self) -> None:
        """Stops listening for changes to the ``SendableChooser``"""
        if self.on_choices or self.on_selected:
            self.subtable.removeTableListener(self._on_change)

    def getChoices(self) -> Sequence[str]:
        """
        Returns the current choices. If the chooser doesn't exist, this
        will return an empty tuple.
        """
        return self.subtable.getStringArray("options", [])

    def getSelected(self) -> Optional[str]:
        """
        Returns the current selection or None
        """
        selected = self.subtable.getString("selected", None)
        if selected is None:
            selected = self.subtable.getString("default", None)
        return selected

    def setSelected(self, selection: str) -> None:
        """
        Sets the active selection on the chooser

        :param selection: Active selection name
        """
        self.subtable.putString("selected", selection)

    def _on_change(self, table, key, value, isNew):
        if key == "options":
            if self.on_choices is not None:
                self.on_choices(value)
        elif key == "selected":
            if self.on_selected is not None:
                self.on_selected(value)
        elif key == "default":
            if (
                self.on_selected is not None
                and self.subtable.getString("selected", None) is None
            ):
                self.on_selected(value)
