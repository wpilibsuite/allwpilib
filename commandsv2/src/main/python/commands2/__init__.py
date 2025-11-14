from .command import Command, InterruptionBehavior

from . import button
from . import cmd
from . import typing

from .commandscheduler import CommandScheduler
from .conditionalcommand import ConditionalCommand
from .deferredcommand import DeferredCommand
from .exceptions import IllegalCommandUse
from .functionalcommand import FunctionalCommand
from .instantcommand import InstantCommand
from .notifiercommand import NotifierCommand
from .parallelcommandgroup import ParallelCommandGroup
from .paralleldeadlinegroup import ParallelDeadlineGroup
from .parallelracegroup import ParallelRaceGroup
from .pidcommand import PIDCommand
from .pidsubsystem import PIDSubsystem
from .printcommand import PrintCommand
from .profiledpidcommand import ProfiledPIDCommand
from .profiledpidsubsystem import ProfiledPIDSubsystem
from .proxycommand import ProxyCommand
from .repeatcommand import RepeatCommand
from .runcommand import RunCommand
from .schedulecommand import ScheduleCommand
from .selectcommand import SelectCommand
from .sequentialcommandgroup import SequentialCommandGroup
from .startendcommand import StartEndCommand
from .subsystem import Subsystem
from .swervecontrollercommand import SwerveControllerCommand
from .timedcommandrobot import TimedCommandRobot
from .trapezoidprofilecommand import TrapezoidProfileCommand
from .trapezoidprofilesubsystem import TrapezoidProfileSubsystem
from .waitcommand import WaitCommand
from .waituntilcommand import WaitUntilCommand
from .wrappercommand import WrapperCommand

from typing import TYPE_CHECKING

__all__ = [
    "button",
    "cmd",
    "Command",
    "CommandScheduler",
    "ConditionalCommand",
    "DeferredCommand",
    "FunctionalCommand",
    "IllegalCommandUse",
    "InstantCommand",
    "InterruptionBehavior",
    "NotifierCommand",
    "ParallelCommandGroup",
    "ParallelDeadlineGroup",
    "ParallelRaceGroup",
    "PIDCommand",
    "PIDSubsystem",
    "PrintCommand",
    "ProfiledPIDCommand",
    "ProfiledPIDSubsystem",
    "ProxyCommand",
    "RepeatCommand",
    "RunCommand",
    "ScheduleCommand",
    "SelectCommand",
    "SequentialCommandGroup",
    "StartEndCommand",
    "Subsystem",
    "SwerveControllerCommand",
    "TimedCommandRobot",
    "TrapezoidProfileCommand",
    "TrapezoidProfileSubsystem",
    "WaitCommand",
    "WaitUntilCommand",
    "WrapperCommand",
]

if not TYPE_CHECKING:

    def __getattr__(attr):
        if attr == "SubsystemBase":
            import warnings

            warnings.warn(
                "SubsystemBase is deprecated", DeprecationWarning, stacklevel=2
            )
            return Subsystem

        raise AttributeError(f"module {__name__!r} has no attribute {attr!r}")
