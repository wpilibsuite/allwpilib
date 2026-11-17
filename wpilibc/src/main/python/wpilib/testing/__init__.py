from typing import NamedTuple

from hal import RobotMode

__all__ = ["OpMode"]


class OpMode(NamedTuple):
    """A published operating mode identified by robot mode and name.

    Returned by the ``opmode`` pytest fixture and accepted by
    :meth:`.RobotTestController.step_timing`. The controller looks up the native
    ID on the running robot, so test parameters and errors remain readable.

    Names are unique within a robot mode, not across all robot modes.
    """

    mode: RobotMode
    name: str
