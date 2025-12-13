# validated: 2024-01-20 DS 7a099cb02a33 button/JoystickButton.java
from wpilib.interfaces import GenericHID

from .trigger import Trigger


class JoystickButton(Trigger):
    """
    A Button that gets its state from a :class:`wpilib.interfaces.GenericHID`.
    """

    def __init__(self, joystick: GenericHID, buttonNumber: int):
        """
        Creates a joystick button for triggering commands.

        :param joystick: The GenericHID object that has the button (e.g. Joystick, KinectStick, etc)
        :param buttonNumber: The button number (see :func:`wpilib.interfaces.GenericHID.getRawButton`
        """
        super().__init__(lambda: joystick.getRawButton(buttonNumber))
