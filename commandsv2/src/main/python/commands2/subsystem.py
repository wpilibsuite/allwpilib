# validated: 2024-01-20 DS 1144115da01f Subsystem.java
# Don't import stuff from the package here, it'll cause a circular import

from __future__ import annotations

from typing import TYPE_CHECKING, Any, Callable, Optional
from typing_extensions import Self

if TYPE_CHECKING:
    from .command import Command
    from .commandscheduler import CommandScheduler

from wpilib import TelemetryLoggable, TelemetryTable


class Subsystem(TelemetryLoggable):
    """
    A robot subsystem. Subsystems are the basic unit of robot organization in the Command-based
    framework; they encapsulate low-level hardware objects (motor controllers, sensors, etc.) and
    provide methods through which they can be used by Commands. Subsystems are used by the
    CommandScheduler's resource management system to ensure multiple robot actions are not
    "fighting" over the same hardware; Commands that use a subsystem should include that subsystem in
    their :func:`commands2.Command.get_requirements` method, and resources used within a subsystem should
    generally remain encapsulated and not be shared by other parts of the robot.

    Subsystems must be registered with the scheduler with the :func:`commands2.CommandScheduler.register_subsystem`
    method in order for the :func:`.periodic` method to be called. It is recommended that this method be called from the
    constructor of users' Subsystem implementations.
    """

    def __new__(cls, *arg, **kwargs) -> Self:
        instance = super().__new__(cls)
        instance._name = cls.__name__
        instance._subsystem = cls.__name__
        # add to the scheduler
        from .commandscheduler import CommandScheduler

        CommandScheduler.get_instance().register_subsystem(instance)
        return instance

    def __init__(self) -> None:
        pass

    def periodic(self) -> None:
        """
        This method is called periodically by the CommandScheduler. Useful for updating
        subsystem-specific state that you don't want to offload to a Command. Teams should try
        to be consistent within their own codebases about which responsibilities will be handled by
        Commands, and which will be handled here.
        """
        pass

    def simulation_periodic(self) -> None:
        """
        This method is called periodically by the CommandScheduler. Useful for updating
        subsystem-specific state that needs to be maintained for simulations, such as for updating simulation classes and setting simulated sensor readings.
        """
        pass

    def set_default_command(self, command: Command) -> None:
        """
        Sets the default Command of the subsystem. The default command will be automatically
        scheduled when no other commands are scheduled that require the subsystem. Default commands
        should generally not end on their own, i.e. their :func:`commands2.Command.is_finished` method should
        always return false. Will automatically register this subsystem with the CommandScheduler.

        :param command: the default command to associate with this subsystem
        """
        from .commandscheduler import CommandScheduler

        CommandScheduler.get_instance().set_default_command(self, command)

    def remove_default_command(self) -> None:
        """
        Removes the default command for the subsystem. This will not cancel the default command if it
        is currently running.
        """
        from .commandscheduler import CommandScheduler

        CommandScheduler.get_instance().remove_default_command(self)

    def get_default_command(self) -> Optional[Command]:
        """
        Gets the default command for this subsystem. Returns None if no default command is currently
        associated with the subsystem.

        :returns: the default command associated with this subsystem
        """
        from .commandscheduler import CommandScheduler

        return CommandScheduler.get_instance().get_default_command(self)

    def get_current_command(self) -> Optional[Command]:
        """
        Returns the command currently running on this subsystem. Returns None if no command is
        currently scheduled that requires this subsystem.

        :returns: the scheduled command currently requiring this subsystem
        """
        from .commandscheduler import CommandScheduler

        return CommandScheduler.get_instance().requiring(self)

    def register(self):
        """
        Registers this subsystem with the :class:`.CommandScheduler`, allowing its
        :func:`.periodic` method to be called when the scheduler runs.
        """
        from .commandscheduler import CommandScheduler

        return CommandScheduler.get_instance().register_subsystem(self)

    def run_once(self, action: Callable[[], None]) -> Command:
        """
        Constructs a command that runs an action once and finishes. Requires this subsystem.

        :param action: the action to run
        :return: the command
        """
        from .cmd import run_once

        return run_once(action, self)

    def run(self, action: Callable[[], None]) -> Command:
        """
        Constructs a command that runs an action every iteration until interrupted. Requires this
        subsystem.

        :param action: the action to run
        :returns: the command"""
        from .cmd import run

        return run(action, self)

    def start_end(self, start: Callable[[], None], end: Callable[[], None]) -> Command:
        """
        Constructs a command that runs an action once and another action when the command is
        interrupted. Requires this subsystem.

        :param start: the action to run on start
        :param end: the action to run on interrupt
        :returns: the command
        """
        from .cmd import start_end

        return start_end(start, end, self)

    def run_end(self, run: Callable[[], None], end: Callable[[], None]) -> Command:
        """
        Constructs a command that runs an action every iteration until interrupted, and then runs a
        second action. Requires this subsystem.

        :param run: the action to run every iteration
        :param end: the action to run on interrupt
        :returns: the command
        """
        from .cmd import run_end

        return run_end(run, end, self)

    def start_run(self, start: Callable[[], None], run: Callable[[], None]) -> Command:
        """
        Constructs a command that runs an action once and another action every iteration until interrupted. Requires this subsystem.

        :param start: the action to run on start
        :param run: the action to run every iteration
        :returns: the command
        """
        from .cmd import start_run

        return start_run(start, run, self)

    def idle(self) -> Command:
        """
        Constructs a command that does nothing until interrupted. Requires this subsystem.

        :returns: the command
        """
        from .cmd import idle

        return idle(self)

    #
    # From SubsystemBase
    #

    def get_name(self) -> str:
        """
        Gets the name of this Subsystem.

        :returns: Name
        """
        return self._name

    def set_name(self, name: str) -> None:
        """
        Set the name of this Subsystem.
        """
        self._name = name

    def get_subsystem(self) -> str:
        """
        Gets the subsystem name of this Subsystem.

        :returns: Subsystem name
        """
        return self._subsystem

    def set_subsystem(self, subsystem: str):
        """
        Sets the subsystem name of this Subsystem.

        :param subsystem: subsystem name
        """
        self._subsystem = subsystem

    def add_child(self, name: str, child: Any) -> None:
        """
        Associates a named child with this Subsystem when the child exposes naming methods.

        :param name:  name to give child
        :param child: sendable
        """
        if hasattr(child, "set_name"):
            child.set_name(name)
        if hasattr(child, "set_subsystem"):
            child.set_subsystem(self.get_subsystem())

    def log_to(self, table: TelemetryTable) -> None:
        default = self.get_default_command()
        table.log(".hasDefault", default is not None)

        def get_default():
            command = self.get_default_command()
            if command is not None:
                return command.get_name()
            return "none"

        table.log(".default", get_default())
        current = self.get_current_command()
        table.log(".hasCommand", current is not None)

        def get_current():
            command = self.get_current_command()
            if command is not None:
                return command.get_name()
            return "none"

        table.log(".command", get_current())

    def get_telemetry_type(self) -> str:
        return "Subsystem"
