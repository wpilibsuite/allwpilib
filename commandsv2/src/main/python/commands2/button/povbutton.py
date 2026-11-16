# validated: 2024-01-20 DS 7a099cb02a33 button/POVButton.java
from wpilib import GenericHID

from .trigger import Trigger


class POVButton(Trigger):
    """
    A Button that gets its state from a POV on a :class:`wpilib.GenericHID`.
    """

    def __init__(self, joystick: GenericHID, angle: int, pov_number: int = 0):
        """
        Creates a POV button for triggering commands.

        :param joystick: The GenericHID object that has the POV
        :param angle: The desired angle in degrees (e.g. 90, 270)
        :param pov_number: The POV number (see :func:`wpilib.GenericHID.get_pov`)
        """
        super().__init__(lambda: joystick.get_pov(pov_number) == angle)
