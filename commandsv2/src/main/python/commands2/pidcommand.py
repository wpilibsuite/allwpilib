# validated: 2024-01-19 DS f29a7d2e501b PIDCommand.java
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
from __future__ import annotations

from typing import Any, Callable, Union

from .command import Command
from .subsystem import Subsystem

from wpimath.controller import PIDController


class PIDCommand(Command):
    """
    A command that controls an output with a :class:`wpimath.controller.PIDController`. Runs forever by default - to add
    exit conditions and/or other behavior, subclass this class. The controller calculation and output
    are performed synchronously in the command's execute() method.
    """

    def __init__(
        self,
        controller: PIDController,
        measurementSource: Callable[[], float],
        setpoint: Union[Callable[[], float], float, int],
        useOutput: Callable[[float], Any],
        *requirements: Subsystem,
    ):
        """
        Creates a new PIDCommand, which controls the given output with a :class:`wpimath.controller.PIDController`.

        :param controller: the controller that controls the output.
        :param measurementSource: the measurement of the process variable
        :param setpoint: the controller's setpoint (either a function that returns a)
                         number or a number
        :param useOutput: the controller's output
        :param requirements: the subsystems required by this command
        """
        super().__init__()

        assert callable(measurementSource)
        assert callable(useOutput)

        self._controller = controller
        self._useOutput = useOutput
        self._measurement = measurementSource

        if isinstance(setpoint, (float, int)):
            setpoint = float(setpoint)
            self._setpoint = lambda: setpoint
        elif callable(setpoint):
            self._setpoint = setpoint
        else:
            raise ValueError(
                f"invalid setpoint (must be callable or number; got {type(setpoint)})"
            )

        self.addRequirements(*requirements)

    def initialize(self):
        self._controller.reset()

    def execute(self):
        self._useOutput(
            self._controller.calculate(self._measurement(), self._setpoint())
        )

    def end(self, interrupted):
        self._useOutput(0)

    def getController(self):
        """
        Returns the PIDController used by the command.

        :return: The PIDController
        """
        return self._controller
