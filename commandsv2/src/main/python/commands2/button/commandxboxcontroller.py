# validated: 2024-01-20 DS 3ba501f9478a button/CommandXboxController.java
from typing import Optional

from wpilib import XboxController
from wpilib.event import EventLoop

from ..commandscheduler import CommandScheduler
from .commandgenerichid import CommandGenericHID
from .trigger import Trigger


class CommandXboxController(CommandGenericHID):
    """
    A version of XboxController with Trigger factories for command-based.
    """

    _hid: XboxController

    def __init__(self, port: int):
        """
        Construct an instance of a controller.

        :param port: The port index on the Driver Station that the controller is plugged into.
        """
        super().__init__(port)
        self._hid = XboxController(port)

    def getHID(self) -> XboxController:
        """
        Get the underlying GenericHID object.

        :returns: the wrapped GenericHID object
        """
        return self._hid

    def leftBumper(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the left bumper's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the right bumper's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getLeftBumper())

    def rightBumper(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the right bumper's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the left bumper's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getRightBumper())

    def leftStick(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the left stick button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the left stick button's digital signal attached to the
                  given loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getLeftStickButton())

    def rightStick(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the right stick button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the right stick button's digital signal attached to the
                  given loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getRightStickButton())

    def a(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the A button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the A button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getAButton())

    def b(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the B button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the B button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getBButton())

    def x(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the X button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the X button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getXButton())

    def y(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the Y button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the Y button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getYButton())

    def start(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the start button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the start button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getStartButton())

    def back(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the back button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: an event instance representing the back button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getBackButton())

    def leftTrigger(
        self, threshold: float = 0.5, loop: Optional[EventLoop] = None
    ) -> Trigger:
        """
        Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
        will be true when the axis value is greater than {@code threshold}.

        :param threshold: the minimum axis value for the returned Trigger to be true. This value
                          should be in the range [0, 1] where 0 is the unpressed state of the axis.
        :param loop: the event loop instance to attach the Trigger to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: a Trigger instance that is true when the left trigger's axis exceeds the provided
            threshold, attached to the given event loop
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getLeftTriggerAxis() > threshold)

    def rightTrigger(
        self, threshold: float = 0.5, loop: Optional[EventLoop] = None
    ) -> Trigger:
        """
        Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
        will be true when the axis value is greater than {@code threshold}.

        :param threshold: the minimum axis value for the returned Trigger to be true. This value
                          should be in the range [0, 1] where 0 is the unpressed state of the axis.
        :param loop: the event loop instance to attach the Trigger to, defaults
                     to :func:`commands2.CommandScheduler.getDefaultButtonLoop`

        :returns: a Trigger instance that is true when the right trigger's axis exceeds the provided
                  threshold, attached to the given event loop
        """
        if loop is None:
            loop = CommandScheduler.getInstance().getDefaultButtonLoop()
        return Trigger(loop, lambda: self._hid.getRightTriggerAxis() > threshold)

    def getLeftX(self) -> float:
        """
        Get the X axis value of left side of the controller. Right is positive.

        :returns: The axis value.
        """
        return self._hid.getLeftX()

    def getRightX(self) -> float:
        """
        Get the X axis value of right side of the controller. Right is positive.

        :returns: The axis value.
        """
        return self._hid.getRightX()

    def getLeftY(self) -> float:
        """
        Get the Y axis value of left side of the controller. Back is positive.

        :returns: The axis value.
        """
        return self._hid.getLeftY()

    def getRightY(self) -> float:
        """
        Get the Y axis value of right side of the controller. Back is positive.

        :returns: The axis value.
        """
        return self._hid.getRightY()

    def getLeftTriggerAxis(self) -> float:
        """
        Get the left trigger (LT) axis value of the controller. Note that this axis is bound to the
        range of [0, 1] as opposed to the usual [-1, 1].

        :returns: The axis value.
        """
        return self._hid.getLeftTriggerAxis()

    def getRightTriggerAxis(self) -> float:
        """
        Get the right trigger (RT) axis value of the controller. Note that this axis is bound to the
        range of [0, 1] as opposed to the usual [-1, 1].

        :returns: The axis value.
        """
        return self._hid.getRightTriggerAxis()
