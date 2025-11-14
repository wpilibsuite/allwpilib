# validated: 2024-01-24 DS 192a28af4731 TrapezoidProfileCommand.java
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#
from __future__ import annotations

from typing import Callable, Any

from wpilib import Timer

from .command import Command
from .subsystem import Subsystem


class TrapezoidProfileCommand(Command):
    """
    A command that runs a :class:`wpimath.trajectory.TrapezoidProfile`. Useful for smoothly controlling mechanism motion.
    """

    def __init__(
        self,
        profile,
        output: Callable[[Any], Any],
        goal: Callable[[], Any],
        currentState: Callable[[], Any],
        *requirements: Subsystem,
    ):
        """Creates a new TrapezoidProfileCommand that will execute the given :class:`wpimath.trajectory.TrapezoidProfile`.
        Output will be piped to the provided consumer function.

        :param profile:      The motion profile to execute.
        :param output:       The consumer for the profile output.
        :param goal:         The supplier for the desired state
        :param currentState: The supplier for the current state
        :param requirements: The subsystems required by this command.
        """
        super().__init__()
        self._profile = profile
        self._output = output
        self._goal = goal
        self._currentState = currentState
        self._timer = Timer()

        self.addRequirements(*requirements)

    def initialize(self) -> None:
        self._timer.restart()

    def execute(self) -> None:
        self._output(
            self._profile.calculate(
                self._timer.get(),
                self._currentState(),
                self._goal(),
            )
        )

    def end(self, interrupted) -> None:
        self._timer.stop()

    def isFinished(self) -> bool:
        return self._timer.hasElapsed(self._profile.totalTime())
