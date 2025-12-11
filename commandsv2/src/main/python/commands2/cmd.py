# validated: 2024-01-20 DS 8aeee0362672 Commands.java
from typing import Any, Callable, Dict, Hashable

from wpimath import units

from .command import Command
from .conditionalcommand import ConditionalCommand
from .functionalcommand import FunctionalCommand
from .instantcommand import InstantCommand
from .parallelcommandgroup import ParallelCommandGroup
from .paralleldeadlinegroup import ParallelDeadlineGroup
from .parallelracegroup import ParallelRaceGroup
from .printcommand import PrintCommand
from .runcommand import RunCommand
from .selectcommand import SelectCommand
from .sequentialcommandgroup import SequentialCommandGroup
from .startendcommand import StartEndCommand
from .subsystem import Subsystem
from .waitcommand import WaitCommand
from .waituntilcommand import WaitUntilCommand

# Is this whole file just to avoid the `new` keyword in Java?


def none() -> Command:
    """
    Constructs a command that does nothing, finishing immediately.

    :returns: the command
    """
    return InstantCommand()


def idle(*requirements: Subsystem) -> Command:
    """
    Constructs a command that does nothing until interrupted.

    :param requirements: Subsystems to require
    :return: the command
    """
    return run(lambda: None, *requirements)


def runOnce(action: Callable[[], Any], *requirements: Subsystem) -> Command:
    """
    Constructs a command that runs an action once and finishes.

    :param action: the action to run
    :param requirements: subsystems the action requires
    :returns: the command
    """
    return InstantCommand(action, *requirements)


def run(action: Callable[[], Any], *requirements: Subsystem) -> Command:
    """
    Constructs a command that runs an action every iteration until interrupted.

    :param action: the action to run
    :param requirements: subsystems the action requires
    :returns: the command
    """
    return RunCommand(action, *requirements)


def startEnd(
    run: Callable[[], Any], end: Callable[[], Any], *requirements: Subsystem
) -> Command:
    """
    Constructs a command that runs an action once and another action when the command is
    interrupted.

    :param start: the action to run on start
    :param end: the action to run on interrupt
    :param requirements: subsystems the action requires
    :returns: the command
    """
    return StartEndCommand(run, end, *requirements)


def runEnd(
    run: Callable[[], Any], end: Callable[[], Any], *requirements: Subsystem
) -> Command:
    """
    Constructs a command that runs an action every iteration until interrupted, and then runs a
    second action.

    :param run: the action to run every iteration
    :param end: the action to run on interrupt
    :param requirements: subsystems the action requires
    :returns: the command
    """
    return FunctionalCommand(
        lambda: None, run, lambda interrupted: end(), lambda: False, *requirements
    )


def startRun(
    start: Callable[[], Any], run: Callable[[], Any], *requirements: Subsystem
) -> Command:
    """
    Constructs a command that runs an action once and another action every iteration until interrupted.

    :param start: the action to run on start
    :param run: the action to run every iteration
    :param requirements: subsystems the action requires
    :returns: the command
    """
    return FunctionalCommand(
        start, run, lambda interrupt: None, lambda: False, *requirements
    )


def print_(message: str) -> Command:
    """
    Constructs a command that prints a message and finishes.

    :param message: the message to print
    :returns: the command
    """
    return PrintCommand(message)


def waitSeconds(seconds: units.seconds) -> Command:
    """
    Constructs a command that does nothing, finishing after a specified duration.

    :param seconds: after how long the command finishes
    :returns: the command
    """
    return WaitCommand(seconds)


def waitUntil(condition: Callable[[], bool]) -> Command:
    """
    Constructs a command that does nothing, finishing once a condition becomes true.

    :param condition: the condition
    :returns: the command
    """
    return WaitUntilCommand(condition)


def either(onTrue: Command, onFalse: Command, selector: Callable[[], bool]) -> Command:
    """
    Runs one of two commands, based on the boolean selector function.

    :param onTrue: the command to run if the selector function returns true
    :param onFalse: the command to run if the selector function returns false
    :param selector: the selector function
    :returns: the command
    """
    return ConditionalCommand(onTrue, onFalse, selector)


def select(
    commands: Dict[Hashable, Command], selector: Callable[[], Hashable]
) -> Command:
    """
    Runs one of several commands, based on the selector function.

    :param selector: the selector function
    :param commands: map of commands to select from
    :returns: the command
    """
    return SelectCommand(commands, selector)


def sequence(*commands: Command) -> Command:
    """
    Runs a group of commands in series, one after the other.

    :param commands: the commands to include
    :returns: the command group
    """
    return SequentialCommandGroup(*commands)


def repeatingSequence(*commands: Command) -> Command:
    """
    Runs a group of commands in series, one after the other. Once the last command ends, the group
    is restarted.

    :param commands: the commands to include
    :returns: the command group
    """
    return sequence(*commands).repeatedly()


def parallel(*commands: Command) -> Command:
    """
    Runs a group of commands at the same time. Ends once all commands in the group finish.

    :param commands: the commands to include
    :returns: the command
    """
    return ParallelCommandGroup(*commands)


def race(*commands: Command) -> Command:
    """
    Runs a group of commands at the same time. Ends once any command in the group finishes, and
    cancels the others.

    :param commands: the commands to include
    :returns: the command group
    """
    return ParallelRaceGroup(*commands)


def deadline(deadline: Command, *commands: Command) -> Command:
    """
    Runs a group of commands at the same time. Ends once a specific command finishes, and cancels
    the others.

    :param deadline: the deadline command
    :param commands: the commands to include
    :returns: the command group
    """
    return ParallelDeadlineGroup(deadline, *commands)


__all__ = [
    "none",
    "runOnce",
    "run",
    "startEnd",
    "runEnd",
    "print_",
    "waitSeconds",
    "waitUntil",
    "either",
    "select",
    "sequence",
    "repeatingSequence",
    "parallel",
    "race",
    "deadline",
]
