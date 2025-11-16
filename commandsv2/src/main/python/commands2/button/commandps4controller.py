# validated: 2024-01-20 DS d426873ed15b button/CommandPS4Controller.java
from typing import Optional

from wpilib import PS4Controller
from wpilib.event import EventLoop

from ..commandscheduler import CommandScheduler
from .commandgenerichid import CommandGenericHID
from .trigger import Trigger


class CommandPS4Controller(CommandGenericHID):
    """
    A version of PS4Controller with Trigger factories for command-based.
    """

    _hid: PS4Controller

    def __init__(self, port: int):
        """
        Construct an instance of a device.

        :param port: The port index on the Driver Station that the device is plugged into.
        """
        super().__init__(port)
        self._hid = PS4Controller(port)

    def getHID(self) -> PS4Controller:
        """
        Get the underlying GenericHID object.

        :returns: the wrapped GenericHID object
        """
        return self._hid

    def L2(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the L2 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the L2 button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getL2Button())

    def R2(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the R2 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the R2 button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getR2Button())

    def L1(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the L1 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the L1 button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getL1Button())

    def R1(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the R1 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the R1 button's digital signal attached to the given
            loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getR1Button())

    def L3(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the L3 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the L3 button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getL3Button())

    def R3(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the R3 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the R3 button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getR3Button())

    def square(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the square button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the square button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getSquareButton())

    def cross(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the cross button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the cross button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getCrossButton())

    def triangle(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the triangle button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the triangle button's digital signal attached to the
                  given loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getTriangleButton())

    def circle(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the circle button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the circle button's digital signal attached to the given
            loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getCircleButton())

    def share(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the share button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the share button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getShareButton())

    def PS(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the PS button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the PS button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getPSButton())

    def options(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the options button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the options button's digital signal attached to the
                  given loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getOptionsButton())

    def touchpad(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the touchpad's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the touchpad's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getTouchpad())

    def getLeftX(self) -> float:
        """
        Get the X axis value of left side of the controller. Right is positive.

        :returns: the axis value.
        """
        return self._hid.getLeftX()

    def getRightX(self) -> float:
        """
        Get the X axis value of right side of the controller. Right is positive.

        :returns: the axis value.
        """
        return self._hid.getRightX()

    def getLeftY(self) -> float:
        """
        Get the Y axis value of left side of the controller. Back is positive.

        :returns: the axis value.
        """
        return self._hid.getLeftY()

    def getRightY(self) -> float:
        """
        Get the Y axis value of right side of the controller. Back is positive.

        :returns: the axis value.
        """
        return self._hid.getRightY()

    def getL2Axis(self) -> float:
        """
        Get the L2 axis value of the controller. Note that this axis is bound to the range of [0, 1] as
        opposed to the usual [-1, 1].

        :returns: the axis value.
        """
        return self._hid.getL2Axis()

    def getR2Axis(self) -> float:
        """
        Get the R2 axis value of the controller. Note that this axis is bound to the range of [0, 1] as
        opposed to the usual [-1, 1].

        :returns: the axis value.
        """
        return self._hid.getR2Axis()
