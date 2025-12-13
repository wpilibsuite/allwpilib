# validated: 2024-01-23 DS 8aeee0362672 CommandScheduler.java
from __future__ import annotations

import inspect
import os.path
import traceback
from typing import Any, Callable, Dict, Iterable, List, Optional, Set, Union

import hal
from typing_extensions import Self
from wpilib import (
    RobotBase,
    RobotState,
    TimedRobot,
    Watchdog,
    reportWarning,
)
from wpilib.event import EventLoop
from wpiutil import Sendable, SendableBuilder, SendableRegistry

from .command import Command, InterruptionBehavior
from .exceptions import IllegalCommandUse
from .subsystem import Subsystem

_cmd_path = os.path.dirname(__file__)


class CommandScheduler(Sendable):
    """
    The scheduler responsible for running Commands. A Command-based robot should call
    :meth:`.run` on the singleton instance in its periodic block in order to run commands
    synchronously from the main loop. Subsystems should be registered with the scheduler using :meth:`.registerSubsystem` in order for their :meth:`commands2.Subsystem.periodic`
    methods to be called and for their default commands to be scheduled.
    """

    _instance: Optional[CommandScheduler] = None

    def __new__(cls) -> CommandScheduler:
        if cls._instance is None:
            return super().__new__(cls)
        return cls._instance

    @staticmethod
    def getInstance() -> CommandScheduler:
        """
        Returns the Scheduler instance.

        :returns: the instance
        """
        return CommandScheduler()

    @staticmethod
    def resetInstance() -> None:
        """
        Resets the scheduler instance, which is useful for testing purposes. This should not be
        called by user code.
        """
        inst = CommandScheduler._instance
        if inst:
            inst._defaultButtonLoop.clear()
            SendableRegistry.remove(inst)

        CommandScheduler._instance = None

    def __init__(self) -> None:
        """
        Gets the scheduler instance.
        """
        super().__init__()
        if CommandScheduler._instance is not None:
            return
        CommandScheduler._instance = self
        self._composedCommands: Dict[Command, str] = {}

        # A set of the currently-running commands.
        self._scheduledCommands: Dict[Command, None] = {}

        # A map from required subsystems to their requiring commands. Also used as a set
        # of the currently-required subsystems.
        self._requirements: Dict[Subsystem, Command] = {}

        # A map from subsystems registered with the scheduler to their default commands.
        # Also used as a list of currently-registered subsystems.
        self._subsystems: Dict[Subsystem, Optional[Command]] = {}

        self._defaultButtonLoop = EventLoop()

        # The set of currently-registered buttons that will be polled every iteration.
        self._activeButtonLoop = self._defaultButtonLoop

        self._disabled = False

        # Lists of user-supplied actions to be executed on scheduling events for every
        # command.
        self._initActions: List[Callable[[Command], None]] = []
        self._executeActions: List[Callable[[Command], None]] = []
        self._interruptActions: List[Callable[[Command, Optional[Command]], None]] = []
        self._finishActions: List[Callable[[Command], None]] = []

        self._inRunLoop = False
        self._toSchedule: Dict[Command, None] = {}
        # python: toCancelInterruptors stored in _toCancel dict
        self._toCancel: Dict[Command, Optional[Command]] = {}
        # self._toCancelInterruptors: List[Optional[Command]] = []
        self._endingCommands: Set[Command] = set()

        self._watchdog = Watchdog(TimedRobot.kDefaultPeriod, lambda: None)

        hal.reportUsage("CommandScheduler", "")

    def setPeriod(self, period: float) -> None:
        """
        Changes the period of the loop overrun watchdog. This should be kept in sync with the
        TimedRobot period.

        :param period: Period in seconds.
        """
        self._watchdog.setTimeout(period)

    def getDefaultButtonLoop(self) -> EventLoop:
        """
        Get the default button poll.

        :returns: a reference to the default EventLoop object polling buttons.
        """
        return self._defaultButtonLoop

    def getActiveButtonLoop(self) -> EventLoop:
        """
        Get the active button poll.

        :returns: a reference to the current EventLoop object polling buttons.
        """
        return self._activeButtonLoop

    def setActiveButtonLoop(self, loop: EventLoop) -> None:
        """
        Replace the button poll with another one.

        :param loop: the new button polling loop object.
        """
        self._activeButtonLoop = loop

    def _initCommand(self, command: Command, *requirements: Subsystem) -> None:
        """
        Initializes a given command, adds its requirements to the list, and performs the init actions.

        :param command: The command to initialize
        :param requirements: The command requirements
        """
        self._scheduledCommands[command] = None
        for requirement in requirements:
            self._requirements[requirement] = command
        command.initialize()
        for action in self._initActions:
            action(command)
        self._watchdog.addEpoch(f"{command.getName()}.initialize()")

    def schedule(self, *commands: Command) -> None:
        """
        Schedules a command for execution. Does nothing if the command is already scheduled. If a
        command's requirements are not available, it will only be started if all the commands currently
        using those requirements have been scheduled as interruptible. If this is the case, they will
        be interrupted and the command will be scheduled.

        WARNING: using this function directly can often lead to unexpected behavior and should be
        avoided. Instead Triggers should be used to schedule Commands.

        :param commands: the commands to schedule.
        """
        for command in commands:
            self._schedule(command)

    def _schedule(self, command: Optional[Command]) -> None:
        if command is None:
            reportWarning("Tried to schedule a null command!", True)
            return

        if self._inRunLoop:
            self._toSchedule[command] = None
            return

        self.requireNotComposed(command)

        # Do nothing if the scheduler is disabled, the robot is disabled and the command
        # doesn't run when disabled, or the command is already scheduled.

        if self._disabled:
            return

        if self.isScheduled(command):
            return

        if RobotState.isDisabled() and not command.runsWhenDisabled():
            return

        requirements = command.getRequirements()

        # Schedule the command if the requirements are not currently in-use.
        if self._requirements.keys().isdisjoint(requirements):
            self._initCommand(command, *requirements)
        else:
            # Else check if the requirements that are in use have all have interruptible
            # commands, and if so, interrupt those commands and schedule the new
            # command.
            for requirement in requirements:
                requiringCommand = self.requiring(requirement)
                if (
                    requiringCommand is not None
                    and requiringCommand.getInterruptionBehavior()
                    == InterruptionBehavior.kCancelIncoming
                ):
                    return

            for requirement in requirements:
                requiringCommand = self.requiring(requirement)
                if requiringCommand is not None:
                    self._cancel(requiringCommand, command)

            self._initCommand(command, *requirements)

    def run(self) -> None:
        """
        Runs a single iteration of the scheduler. The execution occurs in the following order:

        * Subsystem periodic methods are called.
        * Button bindings are polled, and new commands are scheduled from them.
        * Currently-scheduled commands are executed.
        * End conditions are checked on currently-scheduled commands, and commands that are finished
          have their end methods called and are removed.
        * Any subsystems not being used as requirements have their default methods started.
        """
        if self._disabled:
            return
        self._watchdog.reset()

        # Run the periodic method of all registered subsystems.
        for subsystem in self._subsystems:
            subsystem.periodic()
            if RobotBase.isSimulation():
                subsystem.simulationPeriodic()
            self._watchdog.addEpoch(f"{subsystem.getName()}.periodic()")

        # Cache the active instance to avoid concurrency problems if setActiveLoop() is
        # called from inside the button bindings.
        loopCache = self._activeButtonLoop
        # Poll buttons for new commands to add.
        loopCache.poll()
        self._watchdog.addEpoch("buttons.run()")

        self._inRunLoop = True
        isDisabled = RobotState.isDisabled()

        # Run scheduled commands, remove finished commands.
        for command in self._scheduledCommands.copy():
            if isDisabled and not command.runsWhenDisabled():
                self._cancel(command, None)
                continue

            command.execute()
            for action in self._executeActions:
                action(command)
            self._watchdog.addEpoch(f"{command.getName()}.execute()")
            if command.isFinished():
                self._endingCommands.add(command)
                command.end(False)
                for action in self._finishActions:
                    action(command)
                self._endingCommands.remove(command)
                self._scheduledCommands.pop(command)
                for requirement in command.getRequirements():
                    self._requirements.pop(requirement)
                self._watchdog.addEpoch(f"{command.getName()}.end(False)")

        self._inRunLoop = False

        # Schedule/cancel commands from queues populated during loop
        for command in self._toSchedule:
            self._schedule(command)

        for command, interruptor in self._toCancel.items():
            self._cancel(command, interruptor)

        self._toSchedule.clear()
        self._toCancel.clear()

        # Add default commands for un-required registered subsystems.
        for subsystem, scommand in self._subsystems.items():
            if subsystem not in self._requirements and scommand is not None:
                self._schedule(scommand)

        self._watchdog.disable()
        if self._watchdog.isExpired():
            self._watchdog.printEpochs()

    def registerSubsystem(self, *subsystems: Subsystem) -> None:
        """
        Registers subsystems with the scheduler. This must be called for the subsystem's periodic block
        to run when the scheduler is run, and for the subsystem's default command to be scheduled. It
        is recommended to call this from the constructor of your subsystem implementations.

        :param subsystems: the subsystem to register
        """
        for subsystem in subsystems:
            if subsystem is None:
                reportWarning("Tried to register a null subsystem", True)
                continue
            if subsystem in self._subsystems:
                reportWarning("Tried to register an already-registered subsystem", True)
                continue
            self._subsystems[subsystem] = None

    def unregisterSubsystem(self, *subsystems: Subsystem) -> None:
        """
        Un-registers subsystems with the scheduler. The subsystem will no longer have its periodic
        block called, and will not have its default command scheduled.

        :param subsystems: the subsystem to un-register
        """
        for subsystem in subsystems:
            self._subsystems.pop(subsystem, None)

    def unregisterAllSubsystems(self):
        """
        Un-registers all registered Subsystems with the scheduler. All currently registered subsystems
        will no longer have their periodic block called, and will not have their default command
        scheduled.
        """
        self._subsystems.clear()

    def setDefaultCommand(self, subsystem: Subsystem, defaultCommand: Command) -> None:
        """
        Sets the default command for a subsystem. Registers that subsystem if it is not already
        registered. Default commands will run whenever there is no other command currently scheduled
        that requires the subsystem. Default commands should be written to never end (i.e. their
        :func:`commands2.Command.isFinished` method should return False), as they would simply be re-scheduled if they
        do. Default commands must also require their subsystem.

        :param subsystem: the subsystem whose default command will be set
        :param defaultCommand: the default command to associate with the subsystem
        """
        if subsystem is None:
            reportWarning("Tried to set a default command for a null subsystem", True)
            return
        if defaultCommand is None:
            reportWarning("Tried to set a null default command", True)
            return

        self.requireNotComposed(defaultCommand)

        if subsystem not in defaultCommand.getRequirements():
            raise IllegalCommandUse(
                "Default commands must require their subsystem!",
                command=defaultCommand,
                subsystem=subsystem,
            )

        if (
            defaultCommand.getInterruptionBehavior()
            == InterruptionBehavior.kCancelIncoming
        ):
            reportWarning(
                "Registering a non-interruptible default command\nThis will likely prevent any other commands from requiring this subsystem.",
                True,
            )
            # Warn, but allow -- there might be a use case for this.

        self._subsystems[subsystem] = defaultCommand

    def removeDefaultCommand(self, subsystem: Subsystem) -> None:
        """
        Removes the default command for a subsystem. The current default command will run until another
        command is scheduled that requires the subsystem, at which point the current default command
        will not be re-scheduled.

        :param subsystem: the subsystem whose default command will be removed
        """
        if subsystem is None:
            reportWarning(
                "Tried to remove a default command for a null subsystem", True
            )
            return

        self._subsystems[subsystem] = None

    def getDefaultCommand(self, subsystem: Subsystem) -> Optional[Command]:
        """
        Gets the default command associated with this subsystem. Null if this subsystem has no default
        command associated with it.

        :param subsystem: the subsystem to inquire about
        :returns: the default command associated with the subsystem
        """
        return self._subsystems[subsystem]

    def cancel(
        self,
        *commands: Command,
    ) -> None:
        """
        Cancels commands. The scheduler will only call :meth:`commands2.Command.end` method of the
        canceled command with ``True``, indicating they were canceled (as opposed to finishing
        normally).

        Commands will be canceled regardless of InterruptionBehavior interruption behavior.

        :param commands: the commands to cancel
        """
        for command in commands:
            self._cancel(command, None)

    def _cancel(self, command: Command, interruptor: Optional[Command]):
        if command is None:
            reportWarning("Tried to cancel a null command", True)
            return

        if command in self._endingCommands:
            return

        if self._inRunLoop:
            self._toCancel[command] = interruptor
            return

        if not self.isScheduled(command):
            return

        self._endingCommands.add(command)
        command.end(True)
        for action in self._interruptActions:
            action(command, interruptor)

        self._endingCommands.remove(command)
        self._scheduledCommands.pop(command)
        for requirement in command.getRequirements():
            del self._requirements[requirement]
        self._watchdog.addEpoch(f"{command.getName()}.end(true)")

    def cancelAll(self) -> None:
        """Cancels all commands that are currently scheduled."""
        self.cancel(*self._scheduledCommands)

    def isScheduled(self, *commands: Command) -> bool:
        """
        Whether the given commands are running. Note that this only works on commands that are directly
        scheduled by the scheduler; it will not work on commands inside compositions, as the scheduler
        does not see them.

        :param commands: the command to query
        :returns: whether the command is currently scheduled
        """
        return all(command in self._scheduledCommands for command in commands)

    def requiring(self, subsystem: Subsystem) -> Optional[Command]:
        """
        Returns the command currently requiring a given subsystem. None if no command is currently
        requiring the subsystem

        :param subsystem: the subsystem to be inquired about
        :returns: the command currently requiring the subsystem, or None if no command is currently
            scheduled
        """
        return self._requirements.get(subsystem)

    def disable(self) -> None:
        """Disables the command scheduler."""
        self._disabled = True

    def enable(self) -> None:
        """Enables the command scheduler."""
        self._disabled = False

    def printWatchdogEpochs(self) -> None:
        """
        Prints list of epochs added so far and their times.
        """
        self._watchdog.printEpochs()

    def onCommandInitialize(self, action: Callable[[Command], Any]) -> None:
        """
        Adds an action to perform on the initialization of any command by the scheduler.

        :param action: the action to perform
        """
        assert callable(action)
        self._initActions.append(action)

    def onCommandExecute(self, action: Callable[[Command], Any]) -> None:
        """
        Adds an action to perform on the execution of any command by the scheduler.

        :param action: the action to perform
        """
        assert callable(action)
        self._executeActions.append(action)

    def onCommandInterrupt(self, action: Callable[[Command], Any]) -> None:
        """
        Adds an action to perform on the interruption of any command by the scheduler.

        :param action: the action to perform
        """

        assert callable(action)
        self._interruptActions.append(lambda command, interruptor: action(command))

    def onCommandInterruptWithCause(
        self, action: Callable[[Command, Optional[Command]], Any]
    ) -> None:
        """
        Adds an action to perform on the interruption of any command by the scheduler. The action receives the interrupted command and the command that interrupted it

        :param action: the action to perform
        """

        assert callable(action)
        self._interruptActions.append(action)

    def onCommandFinish(self, action: Callable[[Command], Any]) -> None:
        """
        Adds an action to perform on the finishing of any command by the scheduler.

        :param action: the action to perform
        """
        assert callable(action)
        self._finishActions.append(action)

    def registerComposedCommands(self, commands: Iterable[Command]) -> None:
        """
        Register commands as composed. An exception will be thrown if these commands are scheduled
        directly or added to a composition.

        :param commands: the commands to register

        :raises IllegalCommandUse: if the given commands have already been composed.
        """
        cmds = tuple(commands)
        if len(set(cmds)) != len(cmds):
            raise IllegalCommandUse(
                "Cannot compose a command twice in the same composition!"
            )

        self.requireNotComposedOrScheduled(*cmds)

        # Find where the user called us from
        # - it would be better to give a full traceback, but this is fine for now
        location = "<unknown>"

        for f, lineno in traceback.walk_stack(None):
            info = inspect.getframeinfo(f)
            if not info.filename.startswith(_cmd_path):
                location = f"{info.filename}:{lineno}"
                break

        for cmd in cmds:
            self._composedCommands[cmd] = location

    def clearComposedCommands(self) -> None:
        """
        Clears the list of composed commands, allowing all commands to be freely used again.

        .. warning:: Using this haphazardly can result in unexpected/undesirable behavior. Do not use
                     this unless you fully understand what you are doing.
        """
        self._composedCommands.clear()

    def removeComposedCommand(self, command: Command) -> None:
        """
        Removes a single command from the list of composed commands, allowing it to be freely used
        again.

        .. warning:: Using this haphazardly can result in unexpected/undesirable behavior. Do not use
                     this unless you fully understand what you are doing.

        :param command: the command to remove from the list of grouped commands
        """
        self._composedCommands.pop(command, None)

    def requireNotComposed(self, *commands: Command) -> None:
        """
        Requires that the specified command hasn't been already added to a composition.

        :param commands: The commands to check

        :raises IllegalCommandUse: if the given commands have already been composed.
        """
        for command in commands:
            location = self._composedCommands.get(command)
            if location is not None:
                raise IllegalCommandUse(
                    "Commands that have been composed may not be added to another"
                    f"composition or scheduled individually (originally composed at {location})",
                    command=command,
                )

    def requireNotComposedOrScheduled(self, *commands: Command) -> None:
        """
        Requires that the specified command hasn't already been added to a composition, and is not
        currently scheduled.

        :param command: The command to check

        :raises IllegalCommandUse: if the given command has already been composed or scheduled.
        """
        for command in commands:
            if self.isScheduled(command):
                raise IllegalCommandUse(
                    "Commands that have been scheduled individually may not be added to a composition!",
                    command=command,
                )
            self.requireNotComposed(command)

    def isComposed(self, command: Command) -> bool:
        """
        Check if the given command has been composed.

        :param command: The command to check
        :returns: true if composed
        """
        return command in self._composedCommands

    def initSendable(self, builder: SendableBuilder):
        builder.setSmartDashboardType("Scheduler")
        builder.addStringArrayProperty(
            "Names",
            lambda: [command.getName() for command in self._scheduledCommands],
            lambda _: None,
        )
        builder.addIntegerArrayProperty(
            "Ids",
            lambda: [id(command) for command in self._scheduledCommands],
            lambda _: None,
        )

        def cancel_commands(to_cancel: List[int]):
            ids = {id(command): command for command in self._scheduledCommands}
            for hash_value in to_cancel:
                cancelCmd = ids.get(hash_value)
                if cancelCmd is not None:
                    self.cancel(cancelCmd)

        builder.addIntegerArrayProperty(
            "Cancel", lambda: [], lambda to_cancel: cancel_commands(to_cancel)  # type: ignore
        )
