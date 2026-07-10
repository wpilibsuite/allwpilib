# validated: 2024-01-20 DS 92149efa11fa button/CommandGamepad.java
from typing import Optional, Union

from wpilib import DriverStation, EventLoop, Gamepad

from .commandgenerichid import CommandGenericHID
from .trigger import Trigger


def _enum_value(value) -> int:
    try:
        return int(value)
    except TypeError:
        return value.value


class CommandGamepad:
    """
    A version of :class:`wpilib.Gamepad` with :class:`.Trigger` factories for command-based.
    """

    _hid: CommandGenericHID
    _gamepad: Gamepad

    def __init__(self, hid: Union[int, Gamepad]):
        """
        Construct an instance of a controller.

        :param hid: The port index on the Driver Station that the controller is plugged into,
                    or the Gamepad object to use for this controller.
        """

        if isinstance(hid, int):
            self._hid = CommandGenericHID.get_command_generic_hid(hid)
            self._gamepad = DriverStation.get_gamepad(hid)
        else:
            self._hid = CommandGenericHID(hid.get_hid())
            self._gamepad = hid

    def __getattr__(self, name: str):
        return getattr(self._hid, name)

    def get_hid(self) -> CommandGenericHID:
        """
        Get the underlying CommandGenericHID object.

        :returns: the wrapped CommandGenericHID object
        """
        return self._hid

    def get_gamepad(self) -> Gamepad:
        """
        Get the underlying Gamepad object.

        :returns: the wrapped Gamepad object
        """
        return self._gamepad

    def button(self, button, loop: Optional[EventLoop] = None) -> Trigger:
        """
        Constructs an event instance around this button's digital signal.

        :param button: the button index or :class:`wpilib.Gamepad.Button`
        :param loop: the event loop instance to attach the event to
        """
        return self._hid.button(_enum_value(button), loop)

    def face_down(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.FACE_DOWN, loop)

    def face_right(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.FACE_RIGHT, loop)

    def face_left(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.FACE_LEFT, loop)

    def face_up(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.FACE_UP, loop)

    def back(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.BACK, loop)

    def guide(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.GUIDE, loop)

    def start(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.START, loop)

    def left_stick(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.LEFT_STICK, loop)

    def right_stick(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.RIGHT_STICK, loop)

    def left_bumper(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.LEFT_BUMPER, loop)

    def right_bumper(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.RIGHT_BUMPER, loop)

    def dpad_up(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.DPAD_UP, loop)

    def dpad_down(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.DPAD_DOWN, loop)

    def dpad_left(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.DPAD_LEFT, loop)

    def dpad_right(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.DPAD_RIGHT, loop)

    def misc1(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.MISC_1, loop)

    def right_paddle1(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.RIGHT_PADDLE_1, loop)

    def left_paddle1(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.LEFT_PADDLE_1, loop)

    def right_paddle2(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.RIGHT_PADDLE_2, loop)

    def left_paddle2(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.LEFT_PADDLE_2, loop)

    def touchpad(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.TOUCHPAD, loop)

    def misc2(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.MISC_2, loop)

    def misc3(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.MISC_3, loop)

    def misc4(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.MISC_4, loop)

    def misc5(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.MISC_5, loop)

    def misc6(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.MISC_6, loop)

    def left_trigger(
        self, threshold: float = 0.5, loop: Optional[EventLoop] = None
    ) -> Trigger:
        return self.axis_greater_than(Gamepad.Axis.LEFT_TRIGGER, threshold, loop)

    def right_trigger(
        self, threshold: float = 0.5, loop: Optional[EventLoop] = None
    ) -> Trigger:
        return self.axis_greater_than(Gamepad.Axis.RIGHT_TRIGGER, threshold, loop)

    def axis_less_than(
        self, axis, threshold: float, loop: Optional[EventLoop] = None
    ) -> Trigger:
        return self._hid.axis_less_than(_enum_value(axis), threshold, loop)

    def axis_greater_than(
        self, axis, threshold: float, loop: Optional[EventLoop] = None
    ) -> Trigger:
        return self._hid.axis_greater_than(_enum_value(axis), threshold, loop)

    def axis_magnitude_greater_than(
        self, axis, threshold: float, loop: Optional[EventLoop] = None
    ) -> Trigger:
        return self._hid.axis_magnitude_greater_than(_enum_value(axis), threshold, loop)

    def get_axis(self, axis) -> float:
        """
        Get the value of the axis.

        :param axis: the axis index or :class:`wpilib.Gamepad.Axis`
        """
        return self._hid.get_raw_axis(_enum_value(axis))

    def get_left_x(self) -> float:
        return self._gamepad.get_left_x()

    def get_left_y(self) -> float:
        return self._gamepad.get_left_y()

    def get_right_x(self) -> float:
        return self._gamepad.get_right_x()

    def get_right_y(self) -> float:
        return self._gamepad.get_right_y()

    def get_left_trigger_axis(self) -> float:
        return self._gamepad.get_left_trigger_axis()

    def get_right_trigger_axis(self) -> float:
        return self._gamepad.get_right_trigger_axis()
