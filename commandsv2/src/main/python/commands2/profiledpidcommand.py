# validated: 2024-01-24 DS f29a7d2e501b ProfiledPIDCommand.java
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import Any, Generic

from wpimath.controller import ProfiledPIDController, ProfiledPIDControllerRadians
from wpimath.trajectory import TrapezoidProfile, TrapezoidProfileRadians

from .command import Command
from .subsystem import Subsystem
from .typing import (
    FloatOrFloatSupplier,
    FloatSupplier,
    TProfiledPIDController,
    UseOutputFunction,
)


class ProfiledPIDCommand(Command, Generic[TProfiledPIDController]):
    """A command that controls an output with a :class:`.ProfiledPIDController`. Runs forever by default -
    to add exit conditions and/or other behavior, subclass this class. The controller calculation and
    output are performed synchronously in the command's execute() method.
    """

    _stateCls: Any

    def __init__(
        self,
        controller: TProfiledPIDController,
        measurementSource: FloatSupplier,
        goalSource: FloatOrFloatSupplier,
        useOutput: UseOutputFunction,
        *requirements: Subsystem,
    ):
        """Creates a new ProfiledPIDCommand, which controls the given output with a ProfiledPIDController. Goal
        velocity is specified.

        :param controller:        the controller that controls the output.
        :param measurementSource: the measurement of the process variable
        :param goalSource:        the controller's goal
        :param useOutput:         the controller's output
        :param requirements:      the subsystems required by this command
        """

        super().__init__()
        if isinstance(controller, ProfiledPIDController):
            self._stateCls = TrapezoidProfile.State
        elif isinstance(controller, ProfiledPIDControllerRadians):
            self._stateCls = TrapezoidProfileRadians.State
        else:
            raise ValueError(f"unknown controller type {controller!r}")

        self._controller: TProfiledPIDController = controller
        self._useOutput = useOutput
        self._measurement = measurementSource
        if isinstance(goalSource, (float, int)):
            self._goal = lambda: float(goalSource)
        else:
            self._goal = goalSource

        self.addRequirements(*requirements)

    def initialize(self):
        self._controller.reset(self._measurement())

    def execute(self):
        self._useOutput.accept(
            self._controller.calculate(self._measurement(), self._goal()),
            self._controller.getSetpoint(),
        )

    def end(self, interrupted: bool):
        self._useOutput(0.0, self._stateCls())

    def getController(self):
        """Gets the controller used by the command"""
        return self._controller
