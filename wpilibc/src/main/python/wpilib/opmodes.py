from collections.abc import Callable
from typing import Optional, TypeVar, overload

from hal import RobotMode
from wpiutil import Color

__all__ = ["OpModeRobot", "autonomous", "teleop", "utility"]

from ._impl import opmode as _opmode
from ._wpilib import OpModeRobotBase, OpMode

_OpModeT = TypeVar("_OpModeT", bound=OpMode)


def _apply_opmode_decorator(
    cls: type[_OpModeT] | None,
    *,
    mode: RobotMode,
    name: str,
    group: str,
    description: str,
    text_color: Color | None,
    background_color: Color | None,
) -> type[_OpModeT] | Callable[[type[_OpModeT]], type[_OpModeT]]:
    def apply(opmode_cls: type[_OpModeT]) -> type[_OpModeT]:
        return _opmode.attach_metadata(
            opmode_cls,
            mode=mode,
            name=name,
            group=group,
            description=description,
            text_color=text_color,
            background_color=background_color,
        )

    return apply if cls is None else apply(cls)


@overload
def autonomous(
    cls: type[_OpModeT],
    *,
    name: str = "",
    group: str = "",
    description: str = "",
    text_color: Color | None = None,
    background_color: Color | None = None,
) -> type[_OpModeT]: ...


@overload
def autonomous(
    cls: None = None,
    *,
    name: str = "",
    group: str = "",
    description: str = "",
    text_color: Color | None = None,
    background_color: Color | None = None,
) -> Callable[[type[_OpModeT]], type[_OpModeT]]: ...


def autonomous(
    cls: type[_OpModeT] | None = None,
    *,
    name: str = "",
    group: str = "",
    description: str = "",
    text_color: Color | None = None,
    background_color: Color | None = None,
) -> type[_OpModeT] | Callable[[type[_OpModeT]], type[_OpModeT]]:
    """Mark an OpMode subclass for autonomous automatic registration.

    Use this decorator bare (``@autonomous``) or configured
    (``@autonomous(name=..., group=...)``). The optional description and colors
    are published with the Driver Station option.
    """
    return _apply_opmode_decorator(
        cls,
        mode=RobotMode.AUTONOMOUS,
        name=name,
        group=group,
        description=description,
        text_color=text_color,
        background_color=background_color,
    )


@overload
def teleop(
    cls: type[_OpModeT],
    *,
    name: str = "",
    group: str = "",
    description: str = "",
    text_color: Color | None = None,
    background_color: Color | None = None,
) -> type[_OpModeT]: ...


@overload
def teleop(
    cls: None = None,
    *,
    name: str = "",
    group: str = "",
    description: str = "",
    text_color: Color | None = None,
    background_color: Color | None = None,
) -> Callable[[type[_OpModeT]], type[_OpModeT]]: ...


def teleop(
    cls: type[_OpModeT] | None = None,
    *,
    name: str = "",
    group: str = "",
    description: str = "",
    text_color: Color | None = None,
    background_color: Color | None = None,
) -> type[_OpModeT] | Callable[[type[_OpModeT]], type[_OpModeT]]:
    """Mark an OpMode subclass for teleoperated automatic registration.

    Use this decorator bare (``@teleop``) or configured
    (``@teleop(name=..., group=...)``). The optional description and colors are
    published with the Driver Station option.
    """
    return _apply_opmode_decorator(
        cls,
        mode=RobotMode.TELEOPERATED,
        name=name,
        group=group,
        description=description,
        text_color=text_color,
        background_color=background_color,
    )


@overload
def utility(
    cls: type[_OpModeT],
    *,
    name: str = "",
    group: str = "",
    description: str = "",
    text_color: Color | None = None,
    background_color: Color | None = None,
) -> type[_OpModeT]: ...


@overload
def utility(
    cls: None = None,
    *,
    name: str = "",
    group: str = "",
    description: str = "",
    text_color: Color | None = None,
    background_color: Color | None = None,
) -> Callable[[type[_OpModeT]], type[_OpModeT]]: ...


def utility(
    cls: type[_OpModeT] | None = None,
    *,
    name: str = "",
    group: str = "",
    description: str = "",
    text_color: Color | None = None,
    background_color: Color | None = None,
) -> type[_OpModeT] | Callable[[type[_OpModeT]], type[_OpModeT]]:
    """Mark an OpMode subclass for utility automatic registration.

    Use this decorator bare (``@utility``) or configured
    (``@utility(name=..., group=...)``). The optional description and colors are
    published with the Driver Station option.
    """
    return _apply_opmode_decorator(
        cls,
        mode=RobotMode.UTILITY,
        name=name,
        group=group,
        description=description,
        text_color=text_color,
        background_color=background_color,
    )


class OpModeRobot(OpModeRobotBase):
    """
    OpModeRobot implements the opmode-based robot program framework.

    Base class for a robot program that uses selectable operating modes (OpModes).

    Create your robot class by inheriting from OpModeRobot. Mark your OpMode
    classes with ``@autonomous``, ``@teleop``, or ``@utility``. OpModeRobot
    automatically registers these classes when you import them or place them
    in an ``opmodes`` package next to your robot module.

    To disable automatic discovery and registration, call
    ``super().__init__(auto_discover=False)`` in your robot constructor. Use
    ``add_opmode()`` and ``publish_opmodes()`` to register and publish modes manually.

    Selecting an OpMode on the Driver Station creates a new instance of it.
    That instance is discarded when you select a different OpMode or disable
    the robot after enabling it.

    Override ``none_periodic()`` to run code repeatedly while no OpMode is
    selected. Override ``driver_station_connected()`` to run code once, when
    the Driver Station first connects.
    """

    def __init__(self, *, auto_discover: bool = True):
        """
        :param auto_discover: Automatically discover, register, and publish decorated
                              OpModes. If False, skip all automatic registration,
                              including already-imported decorated classes, and
                              automatic publication.
        """
        super().__init__()
        if auto_discover:
            _opmode.discover_and_register(self)

    def add_opmode(
        self,
        opmode_cls: type,
        mode: RobotMode,
        name: str,
        group: Optional[str] = None,
        description: Optional[str] = None,
        text_color: Optional[Color] = None,
        background_color: Optional[Color] = None,
    ) -> None:
        """
        Adds an operating mode option. It's necessary to call publish_opmodes() to
        make the added modes visible to the driver station.

        The text_color and background_color parameters are optional, but setting
        only one has no effect (if only one is provided, it will be ignored).

        :param opmode_cls: opmode class; must be a public, non-abstract subclass of OpMode
                          with a constructor that either takes no arguments or accepts a
                          single argument of this class's type (the latter is preferred).
        :param mode: robot mode
        :param name: name of the operating mode
        :param group: group of the operating mode
        :param description: description of the operating mode
        :param text_color: text color
        :param background_color: background color
        """

        _opmode.register_opmode(
            self,
            opmode_cls,
            mode,
            name,
            group,
            description,
            text_color,
            background_color,
            add_factory=self.add_opmode_factory,
        )
