# validated: 2024-01-20 DS 92149efa11fa button/CommandGenericHID.java
import threading
from typing import ClassVar, Optional, Union, final

from wpilib import DriverStation, EventLoop, GenericHID

from .. import Commands
from ..commandscheduler import CommandScheduler
from ..subsystembase import SubsystemBase
from .trigger import Trigger


@final
class CommandGenericHID:
    """
    A version of :class:`wpilib.GenericHID` with :class:`.Trigger` factories for command-based.
    """

    _hids: ClassVar[dict[int, "CommandGenericHID"]] = {}
    _hids_lock = threading.Lock()

    def __init__(self, hid: Union[int, GenericHID]):
        """
        Construct an instance of a device.

        :param hid: The port on the Driver Station that the device is plugged into,
                    or the GenericHID object to use for this command HID.
        """
        if isinstance(hid, int):
            self._hid = DriverStation.get_generic_hid(hid)
        else:
            self._hid = hid

        # Rumble mutexes
        self._left_rumble = SubsystemBase()
        self._right_rumble = SubsystemBase()
        self._left_trigger_rumble = SubsystemBase()
        self._right_trigger_rumble = SubsystemBase()

        # LED mutex
        self._leds = SubsystemBase()

    @classmethod
    def get_command_generic_hid(cls, port: int) -> "CommandGenericHID":
        """
        Gets the CommandGenericHID object for the given port.
        """
        with cls._hids_lock:
            hid = cls._hids.get(port)
            if hid is None:
                hid = cls(port)
                cls._hids[port] = hid
            return hid

    def get_hid(self) -> GenericHID:
        """
        Get the underlying GenericHID object.
        """
        return self._hid

    def button(self, button: int, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around this button's digital signal.

        :param button: The button index
        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: A trigger instance attached to the event loop
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_raw_button(button))

    def pov(
        self, angle: int, *, pov: int = 0, loop: Optional[EventLoop] = None
    ) -> Trigger:
        """
        Constructs a Trigger instance based around this angle of a POV on the HID.

        The POV angles start at 0 in the up direction, and increase clockwise (e.g. right is 90,
        upper-left is 315).

        :param angle: POV angle in degrees, or -1 for the center / not pressed.
        :param pov: index of the POV to read (starting at 0). Defaults to 0.
        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: a Trigger instance based around this angle of a POV on the HID.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_pov(pov) == angle)

    def pov_up(self) -> Trigger:
        """
        Constructs a Trigger instance based around the 0 degree angle (up) of the default (index 0) POV
        on the HID, attached to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: a Trigger instance based around the 0 degree angle of a POV on the HID.
        """
        return self.pov(0)

    def pov_up_right(self) -> Trigger:
        """
        Constructs a Trigger instance based around the 45 degree angle (right up) of the default (index
        0) POV on the HID, attached to :func:`commands2.CommandScheduler.get_default_button_loop`.

        :returns: a Trigger instance based around the 45 degree angle of a POV on the HID.
        """
        return self.pov(45)

    def pov_right(self) -> Trigger:
        """
        Constructs a Trigger instance based around the 90 degree angle (right) of the default (index 0)
        POV on the HID, attached to :func:`commands2.CommandScheduler.get_default_button_loop`.

        :returns: a Trigger instance based around the 90 degree angle of a POV on the HID.
        """
        return self.pov(90)

    def pov_down_right(self) -> Trigger:
        """
        Constructs a Trigger instance based around the 135 degree angle (right down) of the default
        (index 0) POV on the HID, attached to :func:`commands2.CommandScheduler.get_default_button_loop`.

        :returns: a Trigger instance based around the 135 degree angle of a POV on the HID.
        """
        return self.pov(135)

    def pov_down(self) -> Trigger:
        """
        Constructs a Trigger instance based around the 180 degree angle (down) of the default (index 0)
        POV on the HID, attached to :func:`commands2.CommandScheduler.get_default_button_loop`.

        :returns: a Trigger instance based around the 180 degree angle of a POV on the HID.
        """
        return self.pov(180)

    def pov_down_left(self) -> Trigger:
        """
        Constructs a Trigger instance based around the 225 degree angle (down left) of the default
        (index 0) POV on the HID, attached to :func:`commands2.CommandScheduler.get_default_button_loop`.

        :returns: a Trigger instance based around the 225 degree angle of a POV on the HID.
        """
        return self.pov(225)

    def pov_left(self) -> Trigger:
        """
        Constructs a Trigger instance based around the 270 degree angle (left) of the default (index 0)
        POV on the HID, attached to :func:`commands2.CommandScheduler.get_default_button_loop`.

        :returns: a Trigger instance based around the 270 degree angle of a POV on the HID.
        """
        return self.pov(270)

    def pov_up_left(self) -> Trigger:
        """
        Constructs a Trigger instance based around the 315 degree angle (left up) of the default (index
        0) POV on the HID, attached to :func:`commands2.CommandScheduler.get_default_button_loop`.

        :returns: a Trigger instance based around the 315 degree angle of a POV on the HID.
        """
        return self.pov(315)

    def pov_center(self) -> Trigger:
        """
        Constructs a Trigger instance based around the center (not pressed) position of the default
        (index 0) POV on the HID, attached to :func:`commands2.CommandScheduler.get_default_button_loop`.

        :returns: a Trigger instance based around the center position of a POV on the HID.
        """
        return self.pov(-1)

    def axis_less_than(
        self, axis: int, threshold: float, loop: Optional[EventLoop] = None
    ) -> Trigger:
        """
        Constructs a Trigger instance that is true when the axis value is less than ``threshold``,
        attached to the given loop.

        :param axis: The axis to read, starting at 0
        :param threshold: The value below which this trigger should return true.
        :param loop: the event loop instance to attach the trigger to

        :returns: a Trigger instance that is true when the axis value is less than the provided
                  threshold.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_raw_axis(axis) < threshold)

    def axis_greater_than(
        self, axis: int, threshold: float, loop: Optional[EventLoop] = None
    ) -> Trigger:
        """
        Constructs a Trigger instance that is true when the axis value is greater than
        ``threshold``, attached to the given loop.

        :param axis: The axis to read, starting at 0
        :param threshold: The value above which this trigger should return true.
        :param loop: the event loop instance to attach the trigger to.

        :returns: a Trigger instance that is true when the axis value is greater than the provided
                  threshold.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_raw_axis(axis) > threshold)

    def axis_magnitude_greater_than(
        self, axis: int, threshold: float, loop: Optional[EventLoop] = None
    ) -> Trigger:
        """
        Constructs a Trigger instance that is true when the axis magnitude is greater than
        ``threshold``, attached to the given loop.

        :param axis: The axis to read, starting at 0
        :param threshold: The value above which this trigger should return true.
        :param loop: the event loop instance to attach the trigger to.

        :returns: a Trigger instance that is true when the axis magnitude is greater than the provided
                  threshold.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: abs(self._hid.get_raw_axis(axis)) > threshold)

    def get_raw_axis(self, axis: int) -> float:
        """
        Get the value of the axis.

        :param axis: The axis to read, starting at 0.
        :returns: The value of the axis.
        """
        return self._hid.get_raw_axis(axis)

    def set_rumble(self, type: GenericHID.RumbleType, value: float):
        """
        Set the rumble output for the HID.
        The DS currently supports 2 rumble values, left rumble and right rumble.

        :param type: Which rumble value to set.
        :param value: The normalized value (0 to 1) to set the rumble to.
        """
        self._hid.set_rumble(type, value)

    def is_connected(self):
        """
        Get if the HID is connected.

        :returns: True if the HID is connected.
        """
        return self._hid.is_connected()

    def _rumble(self, subsystem: SubsystemBase, rumble_type: GenericHID.RumbleType, value: float):
        """
        Create a rumble command that manages rumble via a subsystem mutex.
        """
        return Commands.start_end(
            lambda: self.set_rumble(rumble_type, value),
            lambda: self.set_rumble(rumble_type, 0),
            {subsystem}
        )

    def rumble_left(self, value: float):
        """
        Run the left rumble motor. On most controllers, this is the low-frequency motor.

        :param value: The normalized value (0 to 1) to set the rumble to
        :returns: A command that will run the left rumble motor at the given value until interrupted.
        """
        return self._rumble(self._left_rumble, GenericHID.RumbleType.kLeftRumble, value)

    def rumble_right(self, value: float):
        """
        Run the right rumble motor. On most controllers, this is the high-frequency motor.

        :param value: The normalized value (0 to 1) to set the rumble to
        :returns: A command that will run the right rumble motor at the given value until interrupted.
        """
        return self._rumble(self._right_rumble, GenericHID.RumbleType.kRightRumble, value)

    def rumble_both(self, value: float):
        """
        Run both rumble motors.

        :param value: The normalized value (0 to 1) to set the rumble to
        :returns: A command that will run the rumble motors at the given value until interrupted.
        """
        return Commands.parallel(self.rumble_left(value), self.rumble_right(value)).with_name("Both Rumble")

    def rumble_left_trigger(self, value: float):
        """
        Run the left trigger rumble motor, on controllers that have one.

        :param value: The normalized value (0 to 1) to set the rumble to
        :returns: A command that will run the left trigger rumble motor at the given value until interrupted.
        """
        return self._rumble(self._left_trigger_rumble, GenericHID.RumbleType.kLeftTriggerRumble, value)

    def rumble_right_trigger(self, value: float):
        """
        Run the right trigger rumble motor, on controllers that have one.

        :param value: The normalized value (0 to 1) to set the rumble to
        :returns: A command that will run the right trigger rumble motor at the given value until interrupted.
        """
        return self._rumble(self._right_trigger_rumble, GenericHID.RumbleType.kRightTriggerRumble, value)

    def rumble_triggers(self, value: float):
        """
        Run both trigger rumble motors, on controllers that have them.

        :param value: The normalized value (0 to 1) to set the rumble to
        :returns: A command that will run both trigger rumble motors at the given value until interrupted.
        """
        return Commands.parallel(self.rumble_left_trigger(value), self.rumble_right_trigger(value)).with_name("Both Trigger Rumble")

    def set_leds(self, r: int, g: int, b: int):
        """
        Set the LEDs, on controllers that have them.

        :param r: The red value (0-255)
        :param g: The green value (0-255)
        :param b: The blue value (0-255)
        :returns: A command that will set the LEDs to the given values until interrupted.
        """
        return Commands.start_end(
            lambda: self._hid.set_leds(r, g, b),
            lambda: self._hid.set_leds(0, 0, 0),
            {self._leds}
        ).with_name(f"Set LEDs ({r}, {g}, {b})")


def _reset_command_generic_hid_data() -> None:
    with CommandGenericHID._hids_lock:
        CommandGenericHID._hids.clear()
