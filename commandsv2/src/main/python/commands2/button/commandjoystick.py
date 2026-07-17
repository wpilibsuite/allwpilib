# validated: 2024-01-20 DS 92aecab2ef05 button/CommandJoystick.java
from typing import Optional

from wpilib import EventLoop, Joystick

from ..commandscheduler import CommandScheduler
from .commandgenerichid import CommandGenericHID
from .trigger import Trigger


class CommandJoystick:
    """
    A version of :class:`wpilib.Joystick` with :class:`.Trigger` factories for command-based.
    """

    _hid: CommandGenericHID
    _joystick: Joystick

    def __init__(self, port: int):
        """
        Construct an instance of a controller.

        :param port: The port index on the Driver Station that the controller is plugged into.
        """

        self._hid = CommandGenericHID.get_command_generic_hid(port)
        self._joystick = Joystick(self._hid.get_hid())

    def __getattr__(self, name: str):
        return getattr(self._hid, name)

    def get_hid(self) -> CommandGenericHID:
        """
        Get the underlying CommandGenericHID object.

        :returns: the wrapped CommandGenericHID object
        """
        return self._hid

    def get_joystick(self) -> Joystick:
        """
        Get the underlying Joystick object.

        :returns: the wrapped Joystick object
        """
        return self._joystick

    def trigger(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the trigger button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the trigger button's digital signal attached to the
            given loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._joystick.get_trigger())

    def top(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the top button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the top button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._joystick.get_top())

    def set_x_channel(self, channel: int):
        """
        Set the channel associated with the X axis.

        :param channel: The channel to set the axis to.
        """
        self._joystick.set_x_channel(channel)

    def set_y_channel(self, channel: int):
        """
        Set the channel associated with the Y axis.

        :param channel: The channel to set the axis to.
        """
        self._joystick.set_y_channel(channel)

    def set_z_channel(self, channel: int):
        """
        Set the channel associated with the Z axis.

        :param channel: The channel to set the axis to.
        """
        self._joystick.set_z_channel(channel)

    def set_throttle_channel(self, channel: int):
        """
        Set the channel associated with the throttle axis.

        :param channel: The channel to set the axis to.
        """
        self._joystick.set_throttle_channel(channel)

    def set_twist_channel(self, channel: int):
        """
        Set the channel associated with the twist axis.

        :param channel: The channel to set the axis to.
        """
        self._joystick.set_twist_channel(channel)

    def get_x_channel(self) -> int:
        """
        Get the channel currently associated with the X axis.

        :returns: The channel for the axis.
        """
        return self._joystick.get_x_channel()

    def get_y_channel(self) -> int:
        """
        Get the channel currently associated with the Y axis.

        :returns: The channel for the axis.
        """
        return self._joystick.get_y_channel()

    def get_z_channel(self) -> int:
        """
        Get the channel currently associated with the Z axis.

        :returns: The channel for the axis.
        """
        return self._joystick.get_z_channel()

    def get_twist_channel(self) -> int:
        """
        Get the channel currently associated with the twist axis.

        :returns: The channel for the axis.
        """
        return self._joystick.get_twist_channel()

    def get_throttle_channel(self) -> int:
        """
        Get the channel currently associated with the throttle axis.

        :returns: The channel for the axis.
        """
        return self._joystick.get_throttle_channel()

    def get_x(self) -> float:
        """
        Get the x position of the HID.

        This depends on the mapping of the joystick connected to the current port. On most
        joysticks, positive is to the right.

        :returns: the x position
        """
        return self._joystick.get_x()

    def get_y(self) -> float:
        """
        Get the y position of the HID.

        This depends on the mapping of the joystick connected to the current port. On most
        joysticks, positive is to the back.

        :returns: the y position
        """
        return self._joystick.get_y()

    def get_z(self) -> float:
        """
        Get the z position of the HID.

        :returns: the z position
        """
        return self._joystick.get_z()

    def get_twist(self) -> float:
        """
        Get the twist value of the current joystick. This depends on the mapping of the joystick
        connected to the current port.

        :returns: The Twist value of the joystick.
        """
        return self._joystick.get_twist()

    def get_throttle(self) -> float:
        """
        Get the throttle value of the current joystick. This depends on the mapping of the joystick
        connected to the current port.

        :returns: The Throttle value of the joystick.
        """
        return self._joystick.get_throttle()

    def get_magnitude(self) -> float:
        """
        Get the magnitude of the vector formed by the joystick's current position relative to its
        origin.

        :returns: The magnitude of the direction vector
        """
        return self._joystick.get_magnitude()

    def get_direction_radians(self) -> float:
        """
        Get the direction of the vector formed by the joystick and its origin in radians. 0 is forward
        and clockwise is positive. (Straight right is π/2.)

        :returns: The direction of the vector in radians
        """
        # https://docs.wpilib.org/en/stable/docs/software/basic-programming/coordinate-system.html#joystick-and-controller-coordinate-system
        # A positive rotation around the X axis moves the joystick right, and a
        # positive rotation around the Y axis moves the joystick backward. When
        # treating them as translations, 0 radians is measured from the right
        # direction, and angle increases clockwise.
        #
        # It's rotated 90 degrees CCW (y is negated and the arguments are reversed)
        # so that 0 radians is forward.
        return self._joystick.get_direction_radians()

    def get_direction_degrees(self) -> float:
        """
        Get the direction of the vector formed by the joystick and its origin in degrees. 0 is forward
        and clockwise is positive. (Straight right is 90.)

        :returns: The direction of the vector in degrees
        """
        return self._joystick.get_direction_degrees()
