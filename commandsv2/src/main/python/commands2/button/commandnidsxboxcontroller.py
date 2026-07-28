# validated: 2024-01-20 DS 3ba501f9478a button/CommandXboxController.java
from typing import Optional

from wpilib import EventLoop, NiDsXboxController

from ..commandscheduler import CommandScheduler
from .commandgenerichid import CommandGenericHID
from .trigger import Trigger


class CommandNiDsXboxController:
    """
    A version of NI DS XboxController with Trigger factories for command-based.
    """

    _command_hid: CommandGenericHID
    _hid: NiDsXboxController

    def __init__(self, port: int):
        """
        Construct an instance of a controller.

        :param port: The port index on the Driver Station that the controller is plugged into.
        """
        self._command_hid = CommandGenericHID.get_command_generic_hid(port)
        self._hid = NiDsXboxController(self._command_hid.get_hid())

    def __getattr__(self, name: str):
        return getattr(self._command_hid, name)

    def get_hid(self) -> CommandGenericHID:
        """
        Get the underlying CommandGenericHID object.

        :returns: the wrapped CommandGenericHID object
        """
        return self._command_hid

    def get_ni_ds_xbox_controller(self) -> NiDsXboxController:
        """
        Get the underlying NiDsXboxController object.

        :returns: the wrapped NiDsXboxController object
        """
        return self._hid

    def left_bumper(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the left bumper's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the right bumper's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_left_bumper_button())

    def right_bumper(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the right bumper's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the left bumper's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_right_bumper_button())

    def left_stick(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the left stick button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the left stick button's digital signal attached to the
                  given loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_left_stick_button())

    def right_stick(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the right stick button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the right stick button's digital signal attached to the
                  given loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_right_stick_button())

    def a(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the A button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the A button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_a_button())

    def b(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the B button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the B button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_b_button())

    def x(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the X button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the X button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_x_button())

    def y(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the Y button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the Y button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_y_button())

    def start(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the start button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the start button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_start_button())

    def back(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the back button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the back button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_back_button())

    def left_trigger(
        self, threshold: float = 0.5, loop: Optional[EventLoop] = None
    ) -> Trigger:
        """
        Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
        will be true when the axis value is greater than {@code threshold}.

        :param threshold: the minimum axis value for the returned Trigger to be true. This value
                          should be in the range [0, 1] where 0 is the unpressed state of the axis.
        :param loop: the event loop instance to attach the Trigger to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: a Trigger instance that is true when the left trigger's axis exceeds the provided
            threshold, attached to the given event loop
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_left_trigger_axis() > threshold)

    def right_trigger(
        self, threshold: float = 0.5, loop: Optional[EventLoop] = None
    ) -> Trigger:
        """
        Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
        will be true when the axis value is greater than {@code threshold}.

        :param threshold: the minimum axis value for the returned Trigger to be true. This value
                          should be in the range [0, 1] where 0 is the unpressed state of the axis.
        :param loop: the event loop instance to attach the Trigger to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: a Trigger instance that is true when the right trigger's axis exceeds the provided
                  threshold, attached to the given event loop
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_right_trigger_axis() > threshold)

    def get_left_x(self) -> float:
        """
        Get the X axis value of left side of the controller. Right is positive.

        :returns: The axis value.
        """
        return self._hid.get_left_x()

    def get_right_x(self) -> float:
        """
        Get the X axis value of right side of the controller. Right is positive.

        :returns: The axis value.
        """
        return self._hid.get_right_x()

    def get_left_y(self) -> float:
        """
        Get the Y axis value of left side of the controller. Back is positive.

        :returns: The axis value.
        """
        return self._hid.get_left_y()

    def get_right_y(self) -> float:
        """
        Get the Y axis value of right side of the controller. Back is positive.

        :returns: The axis value.
        """
        return self._hid.get_right_y()

    def get_left_trigger_axis(self) -> float:
        """
        Get the left trigger (LT) axis value of the controller. Note that this axis is bound to the
        range of [0, 1] as opposed to the usual [-1, 1].

        :returns: The axis value.
        """
        return self._hid.get_left_trigger_axis()

    def get_right_trigger_axis(self) -> float:
        """
        Get the right trigger (RT) axis value of the controller. Note that this axis is bound to the
        range of [0, 1] as opposed to the usual [-1, 1].

        :returns: The axis value.
        """
        return self._hid.get_right_trigger_axis()
