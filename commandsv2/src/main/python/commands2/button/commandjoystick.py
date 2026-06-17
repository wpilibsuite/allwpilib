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

        self._hid = CommandGenericHID.getCommandGenericHID(port)
        self._joystick = Joystick(self._hid.getHID())

    def __getattr__(self, name: str):
        return getattr(self._hid, name)

    def getHID(self) -> CommandGenericHID:
        """
        Get the underlying CommandGenericHID object.

        :returns: the wrapped CommandGenericHID object
        """
        return self._hid

    def getJoystick(self) -> Joystick:
        """
        Get the underlying Joystick object.

        :returns: the wrapped Joystick object
        """
        return self._joystick

    def trigger(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the trigger button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the trigger button's digital signal attached to the
            given loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._joystick.getTrigger())

    def top(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the top button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the top button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._joystick.getTop())

    def setXChannel(self, channel: int):
        """
        Set the channel associated with the X axis.

        :param channel: The channel to set the axis to.
        """
        self._joystick.setXChannel(channel)

    def setYChannel(self, channel: int):
        """
        Set the channel associated with the Y axis.

        :param channel: The channel to set the axis to.
        """
        self._joystick.setYChannel(channel)

    def setZChannel(self, channel: int):
        """
        Set the channel associated with the Z axis.

        :param channel: The channel to set the axis to.
        """
        self._joystick.setZChannel(channel)

    def setThrottleChannel(self, channel: int):
        """
        Set the channel associated with the throttle axis.

        :param channel: The channel to set the axis to.
        """
        self._joystick.setThrottleChannel(channel)

    def setTwistChannel(self, channel: int):
        """
        Set the channel associated with the twist axis.

        :param channel: The channel to set the axis to.
        """
        self._joystick.setTwistChannel(channel)

    def getXChannel(self) -> int:
        """
        Get the channel currently associated with the X axis.

        :returns: The channel for the axis.
        """
        return self._joystick.getXChannel()

    def getYChannel(self) -> int:
        """
        Get the channel currently associated with the Y axis.

        :returns: The channel for the axis.
        """
        return self._joystick.getYChannel()

    def getZChannel(self) -> int:
        """
        Get the channel currently associated with the Z axis.

        :returns: The channel for the axis.
        """
        return self._joystick.getZChannel()

    def getTwistChannel(self) -> int:
        """
        Get the channel currently associated with the twist axis.

        :returns: The channel for the axis.
        """
        return self._joystick.getTwistChannel()

    def getThrottleChannel(self) -> int:
        """
        Get the channel currently associated with the throttle axis.

        :returns: The channel for the axis.
        """
        return self._joystick.getThrottleChannel()

    def getX(self) -> float:
        """
        Get the x position of the HID.

        This depends on the mapping of the joystick connected to the current port. On most
        joysticks, positive is to the right.

        :returns: the x position
        """
        return self._joystick.getX()

    def getY(self) -> float:
        """
        Get the y position of the HID.

        This depends on the mapping of the joystick connected to the current port. On most
        joysticks, positive is to the back.

        :returns: the y position
        """
        return self._joystick.getY()

    def getZ(self) -> float:
        """
        Get the z position of the HID.

        :returns: the z position
        """
        return self._joystick.getZ()

    def getTwist(self) -> float:
        """
        Get the twist value of the current joystick. This depends on the mapping of the joystick
        connected to the current port.

        :returns: The Twist value of the joystick.
        """
        return self._joystick.getTwist()

    def getThrottle(self) -> float:
        """
        Get the throttle value of the current joystick. This depends on the mapping of the joystick
        connected to the current port.

        :returns: The Throttle value of the joystick.
        """
        return self._joystick.getThrottle()

    def getMagnitude(self) -> float:
        """
        Get the magnitude of the vector formed by the joystick's current position relative to its
        origin.

        :returns: The magnitude of the direction vector
        """
        return self._joystick.getMagnitude()

    def getDirectionRadians(self) -> float:
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
        return self._joystick.getDirectionRadians()

    def getDirectionDegrees(self) -> float:
        """
        Get the direction of the vector formed by the joystick and its origin in degrees. 0 is forward
        and clockwise is positive. (Straight right is 90.)

        :returns: The direction of the vector in degrees
        """
        return self._joystick.getDirectionDegrees()
