# validated: 2024-01-20 DS d426873ed15b button/CommandNiDsPS4Controller.java
from typing import Optional

from wpilib import EventLoop, NiDsPS4Controller

from ..commandscheduler import CommandScheduler
from .commandgenerichid import CommandGenericHID
from .trigger import Trigger


class CommandNiDsPS4Controller:
    """
    A version of NI DS PS4Controller with Trigger factories for command-based.
    """

    _command_hid: CommandGenericHID
    _hid: NiDsPS4Controller

    def __init__(self, port: int):
        """
        Construct an instance of a device.

        :param port: The port index on the Driver Station that the device is plugged into.
        """
        self._command_hid = CommandGenericHID.get_command_generic_hid(port)
        self._hid = NiDsPS4Controller(self._command_hid.get_hid())

    def __getattr__(self, name: str):
        return getattr(self._command_hid, name)

    def get_hid(self) -> CommandGenericHID:
        """
        Get the underlying CommandGenericHID object.

        :returns: the wrapped CommandGenericHID object
        """
        return self._command_hid

    def get_ni_ds_ps4_controller(self) -> NiDsPS4Controller:
        """
        Get the underlying NiDsPS4Controller object.

        :returns: the wrapped NiDsPS4Controller object
        """
        return self._hid

    def l2(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the L2 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the L2 button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_l2_button())

    def r2(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the R2 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the R2 button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_r2_button())

    def l1(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the L1 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the L1 button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_l1_button())

    def r1(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the R1 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the R1 button's digital signal attached to the given
            loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_r1_button())

    def l3(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the L3 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the L3 button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_l3_button())

    def r3(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the R3 button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the R3 button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_r3_button())

    def square(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the square button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the square button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_square_button())

    def cross(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the cross button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the cross button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_cross_button())

    def triangle(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the triangle button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the triangle button's digital signal attached to the
                  given loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_triangle_button())

    def circle(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the circle button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the circle button's digital signal attached to the given
            loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_circle_button())

    def share(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the share button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the share button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_share_button())

    def ps(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the PS button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the PS button's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_ps_button())

    def options(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the options button's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the options button's digital signal attached to the
                  given loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_options_button())

    def touchpad(self, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around the touchpad's digital signal.

        :param loop: the event loop instance to attach the event to, defaults
                     to :func:`commands2.CommandScheduler.get_default_button_loop`

        :returns: an event instance representing the touchpad's digital signal attached to the given
                  loop.
        """
        if loop is None:
            loop = CommandScheduler.get_instance().get_default_button_loop()
        return Trigger(loop, lambda: self._hid.get_touchpad())

    def get_left_x(self) -> float:
        """
        Get the X axis value of left side of the controller. Right is positive.

        :returns: the axis value.
        """
        return self._hid.get_left_x()

    def get_right_x(self) -> float:
        """
        Get the X axis value of right side of the controller. Right is positive.

        :returns: the axis value.
        """
        return self._hid.get_right_x()

    def get_left_y(self) -> float:
        """
        Get the Y axis value of left side of the controller. Back is positive.

        :returns: the axis value.
        """
        return self._hid.get_left_y()

    def get_right_y(self) -> float:
        """
        Get the Y axis value of right side of the controller. Back is positive.

        :returns: the axis value.
        """
        return self._hid.get_right_y()

    def get_l2_axis(self) -> float:
        """
        Get the L2 axis value of the controller. Note that this axis is bound to the range of [0, 1] as
        opposed to the usual [-1, 1].

        :returns: the axis value.
        """
        return self._hid.get_l2_axis()

    def get_r2_axis(self) -> float:
        """
        Get the R2 axis value of the controller. Note that this axis is bound to the range of [0, 1] as
        opposed to the usual [-1, 1].

        :returns: the axis value.
        """
        return self._hid.get_r2_axis()
