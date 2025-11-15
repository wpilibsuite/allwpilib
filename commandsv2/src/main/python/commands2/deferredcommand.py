# validated: 2024-01-24 DS 192a28af4731 DeferredCommand.java
from typing import Callable

from wpiutil import SendableBuilder

from .command import Command
from .commandscheduler import CommandScheduler
from .printcommand import PrintCommand
from .subsystem import Subsystem


class DeferredCommand(Command):
    """
    Defers Command construction to runtime. Runs the command returned by a supplier when this command
    is initialized, and ends when it ends. Useful for performing runtime tasks before creating a new
    command. If this command is interrupted, it will cancel the command.

    Note that the supplier *must* create a new Command each call. For selecting one of a
    preallocated set of commands, use :class:`commands2.SelectCommand`.
    """

    def __init__(self, supplier: Callable[[], Command], *requirements: Subsystem):
        """

        Creates a new DeferredCommand that directly runs the supplied command when initialized, and
        ends when it ends. Useful for lazily creating commands when the DeferredCommand is initialized,
        such as if the supplied command depends on runtime state. The Supplier will be called
        each time this command is initialized. The Supplier *must* create a new Command each call.

        :param supplier:     The command supplier
        :param requirements: The command requirements.
        """
        super().__init__()

        assert callable(supplier)

        self._null_command = PrintCommand(
            f"[DeferredCommand] Supplied command (from {supplier!r} was None!"
        )
        self._supplier = supplier
        self._command = self._null_command
        self.addRequirements(*requirements)

    def initialize(self):
        cmd = self._supplier()
        if cmd is not None:
            self._command = cmd
            CommandScheduler.getInstance().registerComposedCommands([self._command])
        self._command.initialize()

    def execute(self):
        self._command.execute()

    def isFinished(self):
        return self._command.isFinished()

    def end(self, interrupted):
        self._command.end(interrupted)
        self._command = self._null_command

    def initSendable(self, builder: SendableBuilder):
        super().initSendable(builder)
        builder.addStringProperty(
            "deferred",
            lambda: (
                "null"
                if self._command is self._null_command
                else self._command.getName()
            ),
            lambda _: None,
        )
