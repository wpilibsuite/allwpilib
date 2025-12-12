from hal import RobotMode
from typing import Optional
from wpiutil import Color

__all__ = ["OpModeRobot"]

from ._wpilib import OpModeRobotBase, OpMode

class OpModeRobot(OpModeRobotBase):
    """
    OpModeRobot implements the opmode-based robot program framework.

    The OpModeRobot class is intended to be subclassed by a user creating a robot
    program.

    Opmodes are constructed when selected on the driver station, and destroyed
    when the robot is disabled after being enabled or a different opmode is
    selected. When no opmode is selected, nonePeriodic() is called. The
    driverStationConnected() function is called the first time the driver station
    connects to the robot.
    """

    def __init__(self):
        super().__init__()

    def addOpMode(self,
                  opmodeCls: type,
                  mode: RobotMode,
                  name: str,
                  group: Optional[str] = None,
                  description: Optional[str] = None,
                  textColor: Optional[Color] = None,
                  backgroundColor: Optional[Color] = None) -> None:
        """
        Adds an operating mode option. It's necessary to call PublishOpModes() to
        make the added modes visible to the driver station.

        The textColor and backgroundColor parameters are optional, but setting
        only one has no effect (if only one is provided, it will be ignored).

        :param opmodeCls: opmode class; must be a public, non-abstract subclass of OpMode
                          with a constructor that either takes no arguments or accepts a
                          single argument of this class's type (the latter is preferred).
        :param mode: robot mode
        :param name: name of the operating mode
        :param group: group of the operating mode
        :param description: description of the operating mode
        :param textColor: text color
        :param backgroundColor: background color
        """
        def makeOpModeInstance() -> OpMode:
            # Try to instantiate with robot argument first
            try:
                return opmodeCls(self)  # type: ignore
            except TypeError:
                # Fallback to no-argument constructor
                return opmodeCls()  # type: ignore
        if textColor is None or backgroundColor is None:
            self.addOpModeFactory(makeOpModeInstance, mode, name, group or "", description or "")
        else:
            self.addOpModeFactory(makeOpModeInstance, mode, name, group or "", description or "", textColor, backgroundColor)
