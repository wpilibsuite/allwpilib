# validated: 2024-01-20 DS f29a7d2e501b Command.java
# Don't import stuff from the package here, it'll cause a circular import


from __future__ import annotations

from enum import Enum
from typing import TYPE_CHECKING, Any, Callable, Set, Union

from typing_extensions import Self, TypeAlias

if TYPE_CHECKING:
    from .instantcommand import InstantCommand
    from .subsystem import Subsystem
    from .parallelracegroup import ParallelRaceGroup
    from .sequentialcommandgroup import SequentialCommandGroup
    from .paralleldeadlinegroup import ParallelDeadlineGroup
    from .parallelcommandgroup import ParallelCommandGroup
    from .repeatcommand import RepeatCommand
    from .proxycommand import ProxyCommand
    from .conditionalcommand import ConditionalCommand
    from .wrappercommand import WrapperCommand

from wpilib import ComplexTunable, TelemetryLoggable, TelemetryTable, TunableTable


class InterruptionBehavior(Enum):
    """
    An enum describing the command's behavior when another command with a shared requirement is
    scheduled.
    """

    CANCEL_INCOMING = 0
    """
    This command ends, :func:`commands2.Command.end` is called, and the incoming command is
    scheduled normally.

    This is the default behavior.
    """

    CANCEL_SELF = 1
    """ This command continues, and the incoming command is not scheduled."""


class Command(TelemetryLoggable, ComplexTunable):
    """
    A state machine representing a complete action to be performed by the robot. Commands are run by
    the :class:`commands2.CommandScheduler`, and can be composed into CommandGroups to allow users to build
    complicated multistep actions without the need to roll the state machine logic themselves.

    Commands are run synchronously from the main robot loop; no multithreading is used, unless
    specified explicitly from the command implementation.
    """

    InterruptionBehavior: TypeAlias = (
        InterruptionBehavior  # type alias for 2023 location
    )

    requirements: Set[Subsystem]

    def __new__(cls, *args, **kwargs) -> Self:
        instance = super().__new__(
            cls,
        )
        instance.requirements = set()
        instance._name = cls.__name__
        instance._subsystem = ""
        return instance

    def __init__(self):
        pass

    def initialize(self):
        """The initial subroutine of a command. Called once when the command is initially scheduled."""
        pass

    def execute(self):
        """The main body of a command. Called repeatedly while the command is scheduled."""
        pass

    def end(self, interrupted: bool):
        """
        The action to take when the command ends. Called when either the command finishes normally, or
        when it interrupted/canceled.

        Do not schedule commands here that share requirements with this command. Use :meth:`.and_then` instead.

        :param interrupted: whether the command was interrupted/canceled
        """
        pass

    def is_finished(self) -> bool:
        """
        Whether the command has finished. Once a command finishes, the scheduler will call its :meth:`commands2.Command.end`
        method and un-schedule it.

        :returns: whether the command has finished.
        """
        return False

    def get_requirements(self) -> Set[Subsystem]:
        """
        Specifies the set of subsystems used by this command. Two commands cannot use the same
        subsystem at the same time. If another command is scheduled that shares a requirement, :meth:`.get_interruption_behavior` will be checked and followed. If no subsystems are required, return
        an empty set.

        .. note:: it is recommended that user implementations contain the requirements as a field, and
                  return that field here, rather than allocating a new set every time this is called.

        :returns: the set of subsystems that are required
        """
        return self.requirements

    def add_requirements(self, *requirements: Subsystem):
        """
        Adds the specified subsystems to the requirements of the command. The scheduler will prevent
        two commands that require the same subsystem from being scheduled simultaneously.

        .. note:: The scheduler determines the requirements of a command when it is scheduled, so
                  this method should normally be called from the command's constructor.

        :param requirements: the requirements to add
        """
        self.requirements.update(requirements)

    def get_name(self) -> str:
        """
        Gets the name of this Command.

        :returns: Name
        """
        return self._name

    def set_name(self, name: str):
        """
        Sets the name of this Command.

        :param name: Name
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

    def with_timeout(self, seconds: float) -> ParallelRaceGroup:
        """
        Decorates this command with a timeout. If the specified timeout is exceeded before the command
        finishes normally, the command will be interrupted and un-scheduled. Note that the timeout only
        applies to the command returned by this method; the calling command is not itself changed.

        .. note:: This decorator works by adding this command to a composition.
                  The command the decorator was called on cannot be scheduled
                  independently or be added to a different composition (namely,
                  decorators), unless it is manually cleared from the list of composed
                  commands with :func:`commands2.CommandScheduler.remove_composed_command`.
                  The command composition returned from this method can be further
                  decorated without issue.

        :param seconds: the timeout duration
        :returns: the command with the timeout added
        """
        from .waitcommand import WaitCommand

        return self.race_with(WaitCommand(seconds))

    def until(self, condition: Callable[[], bool]) -> ParallelRaceGroup:
        """
        Decorates this command with an interrupt condition. If the specified condition becomes true
        before the command finishes normally, the command will be interrupted and un-scheduled.

        .. note:: This decorator works by adding this command to a composition.
                  The command the decorator was called on cannot be scheduled
                  independently or be added to a different composition (namely,
                  decorators), unless it is manually cleared from the list of composed
                  commands with :func:`commands2.CommandScheduler.remove_composed_command`.
                  The command composition returned from this method can be further
                  decorated without issue.

        :param condition: the interrupt condition
        :returns: the command with the interrupt condition added
        """
        from .waituntilcommand import WaitUntilCommand

        return self.race_with(WaitUntilCommand(condition))

    def only_while(self, condition: Callable[[], bool]) -> ParallelRaceGroup:
        """
        Decorates this command with a run condition. If the specified condition becomes false before
        the command finishes normally, the command will be interrupted and un-scheduled.

        .. note:: This decorator works by adding this command to a composition.
                  The command the decorator was called on cannot be scheduled
                  independently or be added to a different composition (namely,
                  decorators), unless it is manually cleared from the list of composed
                  commands with :func:`commands2.CommandScheduler.remove_composed_command`.
                  The command composition returned from this method can be further
                  decorated without issue.

        :param condition: the interrupt condition
        :returns: the command with the interrupt condition added
        """
        assert callable(condition)
        return self.until(lambda: not condition())

    def before_starting(
        self, before: Union[Command, Callable[[], None]]
    ) -> SequentialCommandGroup:
        """
        Decorates this command with a callable or command to run before this command starts.

        .. note:: This decorator works by adding this command to a composition.
                  The command the decorator was called on cannot be scheduled
                  independently or be added to a different composition (namely,
                  decorators), unless it is manually cleared from the list of composed
                  commands with :func:`commands2.CommandScheduler.remove_composed_command`.
                  The command composition returned from this method can be further
                  decorated without issue.

        :param before: the command to run before this one
        :returns: the decorated command
        """
        from .instantcommand import InstantCommand
        from .sequentialcommandgroup import SequentialCommandGroup

        if callable(before):
            before = InstantCommand(before)

        return SequentialCommandGroup(before, self)

    def and_then(self, *next: Command) -> SequentialCommandGroup:
        """
        Decorates this command with a set of commands to run after it in sequence. Often more
        convenient/less-verbose than constructing a new SequentialCommandGroup explicitly.

        .. note:: This decorator works by adding this command to a composition.
                  The command the decorator was called on cannot be scheduled
                  independently or be added to a different composition (namely,
                  decorators), unless it is manually cleared from the list of composed
                  commands with :func:`commands2.CommandScheduler.remove_composed_command`.
                  The command composition returned from this method can be further
                  decorated without issue.

        :param next: the commands to run next
        :returns: the decorated command
        """
        from .sequentialcommandgroup import SequentialCommandGroup

        return SequentialCommandGroup(self, *next)

    def deadline_for(self, *parallel: Command) -> ParallelDeadlineGroup:
        """
        Decorates this command with a set of commands to run parallel to it, ending when the calling
        command ends and interrupting all the others. Often more convenient/less-verbose than
        constructing a new ParallelDeadlineGroup explicitly.

        .. note:: This decorator works by adding this command to a composition.
                  The command the decorator was called on cannot be scheduled
                  independently or be added to a different composition (namely,
                  decorators), unless it is manually cleared from the list of composed
                  commands with :func:`commands2.CommandScheduler.remove_composed_command`.
                  The command composition returned from this method can be further
                  decorated without issue.

        :param parallel: the commands to run in parallel
        :returns: the decorated command
        """
        from .paralleldeadlinegroup import ParallelDeadlineGroup

        return ParallelDeadlineGroup(self, *parallel)

    def along_with(self, *parallel: Command) -> ParallelCommandGroup:
        """
        Decorates this command with a set of commands to run parallel to it, ending when the last
        command ends. Often more convenient/less-verbose than constructing a new {@link
        ParallelCommandGroup} explicitly.

        .. note:: This decorator works by adding this command to a composition.
                  The command the decorator was called on cannot be scheduled
                  independently or be added to a different composition (namely,
                  decorators), unless it is manually cleared from the list of composed
                  commands with :func:`commands2.CommandScheduler.remove_composed_command`.
                  The command composition returned from this method can be further
                  decorated without issue.

        :param parallel: the commands to run in parallel
        :returns: the decorated command
        """
        from .parallelcommandgroup import ParallelCommandGroup

        return ParallelCommandGroup(self, *parallel)

    def race_with(self, *parallel: Command) -> ParallelRaceGroup:
        """
        Decorates this command with a set of commands to run parallel to it, ending when the first
        command ends. Often more convenient/less-verbose than constructing a new {@link
        ParallelRaceGroup} explicitly.

        .. note:: This decorator works by adding this command to a composition.
                  The command the decorator was called on cannot be scheduled
                  independently or be added to a different composition (namely,
                  decorators), unless it is manually cleared from the list of composed
                  commands with :func:`commands2.CommandScheduler.remove_composed_command`.
                  The command composition returned from this method can be further
                  decorated without issue.

        :param parallel: the commands to run in parallel
        :returns: the decorated command
        """
        from .parallelracegroup import ParallelRaceGroup

        return ParallelRaceGroup(self, *parallel)

    def repeatedly(self) -> RepeatCommand:
        """
        Decorates this command to run repeatedly, restarting it when it ends, until this command is
        interrupted. The decorated command can still be canceled.

        .. note:: This decorator works by adding this command to a composition.
                  The command the decorator was called on cannot be scheduled
                  independently or be added to a different composition (namely,
                  decorators), unless it is manually cleared from the list of composed
                  commands with :func:`commands2.CommandScheduler.remove_composed_command`.
                  The command composition returned from this method can be further
                  decorated without issue.

        :returns: the decorated command
        """
        from .repeatcommand import RepeatCommand

        return RepeatCommand(self)

    def as_proxy(self) -> ProxyCommand:
        """
        Decorates this command to run "by proxy" by wrapping it in a ProxyCommand. Use this for
        "forking off" from command compositions when the user does not wish to extend the command's
        requirements to the entire command composition. ProxyCommand has unique implications and
        semantics, see the `WPILib docs <https://docs.wpilib.org/en/stable/docs/software/commandbased/command-compositions.html#scheduling-other-commands>`_ for a full explanation.

        :returns: the decorated command
        """
        from .proxycommand import ProxyCommand

        return ProxyCommand(self)

    def unless(self, condition: Callable[[], bool]) -> ConditionalCommand:
        """
        Decorates this command to only run if this condition is not met. If the command is already
        running and the condition changes to true, the command will not stop running. The requirements
        of this command will be kept for the new conditional command.

        .. note:: This decorator works by adding this command to a composition.
                  The command the decorator was called on cannot be scheduled
                  independently or be added to a different composition (namely,
                  decorators), unless it is manually cleared from the list of composed
                  commands with :func:`commands2.CommandScheduler.remove_composed_command`.
                  The command composition returned from this method can be further
                  decorated without issue.

        :param condition: the condition that will prevent the command from running
        :returns: the decorated command
        """
        from .conditionalcommand import ConditionalCommand
        from .instantcommand import InstantCommand

        return ConditionalCommand(InstantCommand(), self, condition)

    def only_if(self, condition: Callable[[], bool]) -> ConditionalCommand:
        """
        Decorates this command to only run if this condition is met. If the command is already running
        and the condition changes to false, the command will not stop running. The requirements of this
        command will be kept for the new conditional command.

        .. note:: This decorator works by adding this command to a composition.
                  The command the decorator was called on cannot be scheduled
                  independently or be added to a different composition (namely,
                  decorators), unless it is manually cleared from the list of composed
                  commands with :func:`commands2.CommandScheduler.remove_composed_command`.
                  The command composition returned from this method can be further
                  decorated without issue.

        :param condition: the condition that will allow the command to run
        :returns: the decorated command
        """
        return self.unless(lambda: not condition())

    def ignoring_disable(self, does_run_when_disabled: bool) -> WrapperCommand:
        """
        Decorates this command to run or stop when disabled.

        :param does_run_when_disabled: true to run when disabled.
        :returns: the decorated command
        """
        from .wrappercommand import WrapperCommand

        class W(WrapperCommand):
            def runs_when_disabled(self) -> bool:
                return does_run_when_disabled

        return W(self)

    def with_interrupt_behavior(self, behavior: InterruptionBehavior) -> WrapperCommand:
        """
        Decorates this command to have a different InterruptionBehavior interruption behavior.

        :param behavior: the desired interrupt behavior
        :returns: the decorated command
        """
        from .wrappercommand import WrapperCommand

        class W(WrapperCommand):
            def get_interruption_behavior(self) -> InterruptionBehavior:
                return behavior

        return W(self)

    def finally_do(self, end: Callable[[bool], Any]) -> WrapperCommand:
        """
        Decorates this command with a lambda to call on interrupt or end, following the command's
        inherent :func:`commands2.Command.end` method.

        :param end: a lambda accepting a boolean parameter specifying whether the command was
            interrupted.
        :return: the decorated command
        """
        from .wrappercommand import WrapperCommand

        class W(WrapperCommand):
            def end(self, interrupted: bool) -> None:
                self._command.end(interrupted)
                end(interrupted)

        return W(self)

    def handle_interrupt(self, handler: Callable[[], Any]) -> WrapperCommand:
        """
        Decorates this command with a lambda to call on interrupt, following the command's inherent
        :func:`commands2.Command.end` method.

        :param handler: a lambda to run when the command is interrupted
        :returns: the decorated command
        """
        return self.finally_do(lambda interrupted: handler() if interrupted else None)

    def schedule(self) -> None:
        """Schedules this command."""
        from .commandscheduler import CommandScheduler

        CommandScheduler.get_instance().schedule(self)

    def cancel(self) -> None:
        """
        Cancels this command. Will call ``end(true)``. Commands will be canceled
        regardless of InterruptionBehavior interruption behavior.
        """
        from .commandscheduler import CommandScheduler

        CommandScheduler.get_instance().cancel(self)

    def is_scheduled(self) -> bool:
        """
        Whether the command is currently scheduled. Note that this does not detect whether the command
        is in a composition, only whether it is directly being run by the scheduler.

        :returns: Whether the command is scheduled.
        """
        from .commandscheduler import CommandScheduler

        return CommandScheduler.get_instance().is_scheduled(self)

    def has_requirement(self, requirement: Subsystem) -> bool:
        """
        Whether the command requires a given subsystem.

        :param requirement: the subsystem to inquire about
        :returns: whether the subsystem is required
        """
        return requirement in self.requirements

    def get_interruption_behavior(self) -> InterruptionBehavior:
        """
        How the command behaves when another command with a shared requirement is scheduled.

        :returns: a variant of InterruptionBehavior, defaulting to {@link InterruptionBehavior#K_CANCEL_SELF K_CANCEL_SELF}.
        """
        return InterruptionBehavior.CANCEL_SELF

    def runs_when_disabled(self) -> bool:
        """
        Whether the given command should run when the robot is disabled. Override to return true if the
        command should run when disabled.

        :returns: whether the command should run when the robot is disabled
        """
        return False

    def with_name(self, name: str) -> WrapperCommand:
        """
        Decorates this command with a name.

        :param name: the name of the command
        :returns: the decorated command
        """
        from .wrappercommand import WrapperCommand

        wrapper = WrapperCommand(self)
        wrapper.set_name(name)
        return wrapper

    def log_to(self, table: TelemetryTable) -> None:
        from .commandscheduler import CommandScheduler

        table.log(".name", self.get_name())
        table.log("running", self.is_scheduled())
        table.log(".isParented", CommandScheduler.get_instance().is_composed(self))
        table.log("interruptBehavior", self.get_interruption_behavior().name)
        table.log("runsWhenDisabled", self.runs_when_disabled())

    def get_telemetry_type(self) -> str:
        return "Command"

    def publish_tunable(self, table: TunableTable) -> None:

        def on_set(value: bool) -> None:
            if value:
                if not self.is_scheduled():
                    self.schedule()
            else:
                if self.is_scheduled():
                    self.cancel()

        table.publish_boolean("running", self.is_scheduled, on_set)

    def get_tunable_type(self) -> str:
        return "Command"
