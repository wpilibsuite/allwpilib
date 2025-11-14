# validated: 2024-01-20 DS 7a099cb02a33 button/POVButton.java
from wpilib.interfaces import GenericHID

from .trigger import Trigger


class POVButton(Trigger):
    """
    A Button that gets its state from a POV on a :class:`wpilib.interfaces.GenericHID`.
    """

    def __init__(self, joystick: GenericHID, angle: int, povNumber: int = 0):
        """
        Creates a POV button for triggering commands.

        :param joystick: The GenericHID object that has the POV
        :param angle: The desired angle in degrees (e.g. 90, 270)
        :param povNumber: The POV number (see :func:`wpilib.interfaces.GenericHID.getPOV`)
        """
        super().__init__(lambda: joystick.getPOV(povNumber) == angle)
