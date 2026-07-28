# validated: 2024-01-19 DS 192a28af4731 ProxyCommand.java
from __future__ import annotations

from wpiutil import SendableBuilder

from .command import Command


class ProxyCommand(Command):
    """
    Schedules a given command when this command is initialized and ends when it ends, but does not
    directly run it. Use this for including a command in a composition without adding its
    requirements, but only if you know what you are doing. If you are unsure, see
    `the WPILib docs <https://docs.wpilib.org/en/stable/docs/software/commandbased/command-compositions.html#scheduling-other-commands>`_
    for a complete explanation of proxy semantics. Do not proxy a command from a subsystem already
    required by the composition, or else the composition will cancel itself when the proxy is reached.
    If this command is interrupted, it will cancel the command.
    """

    _command: Command

    def __init__(self, command: Command):
        """
        Creates a new ProxyCommand that schedules the given command when initialized, and ends when it
        is no longer scheduled.

        :param command: the command to run by proxy
        """
        super().__init__()

        self.set_name(f"Proxy({command.get_name()})")
        self._command = command

    def initialize(self):
        self._command.schedule()

    def end(self, interrupted: bool):
        assert self._command is not None
        if interrupted:
            self._command.cancel()
        self._command = None

    def execute(self):
        pass

    def is_finished(self) -> bool:
        # because we're between `initialize` and `end`, `self._command` is necessarily not None
        # but if called otherwise and m_command is None,
        # it's UB, so we can do whatever we want -- like return true.
        return self._command is None or not self._command.is_scheduled()

    def runs_when_disabled(self) -> bool:
        """
        Whether the given command should run when the robot is disabled. Override to return true if the
        command should run when disabled.

        :returns: true. Otherwise, this proxy would cancel commands that do run when disabled.
        """
        return True

    def init_sendable(self, builder: SendableBuilder):
        super().init_sendable(builder)
        builder.add_string_property(
            "proxied",
            lambda: "null" if self._command is None else self._command.get_name(),
            lambda _: None,
        )
