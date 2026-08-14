# validated: 2024-01-20 DS 7a099cb02a33 button/JoystickButton.java
from wpilib import GenericHID

from .trigger import Trigger


class JoystickButton(Trigger):
    """
    A Button that gets its state from a :class:`wpilib.GenericHID`.
    """

    def __init__(self, joystick: GenericHID, button_number: int):
        """
        Creates a joystick button for triggering commands.

        :param joystick: The GenericHID object that has the button (e.g. Joystick, KinectStick, etc)
        :param button_number: The button number (see :func:`wpilib.GenericHID.get_raw_button`
        """
        super().__init__(lambda: joystick.get_raw_button(button_number))
