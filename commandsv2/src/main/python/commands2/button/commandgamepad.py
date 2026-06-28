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
            self._hid = CommandGenericHID.getCommandGenericHID(hid)
            self._gamepad = DriverStation.getGamepad(hid)
        else:
            self._hid = CommandGenericHID(hid.getHID())
            self._gamepad = hid

    def __getattr__(self, name: str):
        return getattr(self._hid, name)

    def getHID(self) -> CommandGenericHID:
        """
        Get the underlying CommandGenericHID object.

        :returns: the wrapped CommandGenericHID object
        """
        return self._hid

    def getGamepad(self) -> Gamepad:
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

    def faceDown(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.FACE_DOWN, loop)

    def faceRight(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.FACE_RIGHT, loop)

    def faceLeft(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.FACE_LEFT, loop)

    def faceUp(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.FACE_UP, loop)

    def back(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.BACK, loop)

    def guide(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.GUIDE, loop)

    def start(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.START, loop)

    def leftStick(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.LEFT_STICK, loop)

    def rightStick(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.RIGHT_STICK, loop)

    def leftBumper(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.LEFT_BUMPER, loop)

    def rightBumper(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.RIGHT_BUMPER, loop)

    def dpadUp(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.DPAD_UP, loop)

    def dpadDown(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.DPAD_DOWN, loop)

    def dpadLeft(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.DPAD_LEFT, loop)

    def dpadRight(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.DPAD_RIGHT, loop)

    def misc1(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.MISC_1, loop)

    def rightPaddle1(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.RIGHT_PADDLE_1, loop)

    def leftPaddle1(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.LEFT_PADDLE_1, loop)

    def rightPaddle2(self, loop: Optional[EventLoop] = None) -> Trigger:
        return self.button(Gamepad.Button.RIGHT_PADDLE_2, loop)

    def leftPaddle2(self, loop: Optional[EventLoop] = None) -> Trigger:
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

    def leftTrigger(
        self, threshold: float = 0.5, loop: Optional[EventLoop] = None
    ) -> Trigger:
        return self.axisGreaterThan(Gamepad.Axis.LEFT_TRIGGER, threshold, loop)

    def rightTrigger(
        self, threshold: float = 0.5, loop: Optional[EventLoop] = None
    ) -> Trigger:
        return self.axisGreaterThan(Gamepad.Axis.RIGHT_TRIGGER, threshold, loop)

    def axisLessThan(
        self, axis, threshold: float, loop: Optional[EventLoop] = None
    ) -> Trigger:
        return self._hid.axisLessThan(_enum_value(axis), threshold, loop)

    def axisGreaterThan(
        self, axis, threshold: float, loop: Optional[EventLoop] = None
    ) -> Trigger:
        return self._hid.axisGreaterThan(_enum_value(axis), threshold, loop)

    def axisMagnitudeGreaterThan(
        self, axis, threshold: float, loop: Optional[EventLoop] = None
    ) -> Trigger:
        return self._hid.axisMagnitudeGreaterThan(_enum_value(axis), threshold, loop)

    def getAxis(self, axis) -> float:
        """
        Get the value of the axis.

        :param axis: the axis index or :class:`wpilib.Gamepad.Axis`
        """
        return self._hid.getRawAxis(_enum_value(axis))

    def getLeftX(self) -> float:
        return self._gamepad.getLeftX()

    def getLeftY(self) -> float:
        return self._gamepad.getLeftY()

    def getRightX(self) -> float:
        return self._gamepad.getRightX()

    def getRightY(self) -> float:
        return self._gamepad.getRightY()

    def getLeftTriggerAxis(self) -> float:
        return self._gamepad.getLeftTriggerAxis()

    def getRightTriggerAxis(self) -> float:
        return self._gamepad.getRightTriggerAxis()
