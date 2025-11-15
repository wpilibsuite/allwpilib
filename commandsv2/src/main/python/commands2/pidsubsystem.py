# validated: 2024-01-19 DS f29a7d2e501b PIDSubsystem.java
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
from __future__ import annotations

from wpimath.controller import PIDController

from .subsystem import Subsystem


class PIDSubsystem(Subsystem):
    """
    A subsystem that uses a :class:`wpimath.controller.PIDController` to control an output. The
    controller is run synchronously from the subsystem's periodic() method.
    """

    def __init__(self, controller: PIDController, initial_position: float = 0.0):
        """
        Creates a new PIDSubsystem.

        :param controller: The PIDController to use.
        :param initial_position: The initial setpoint of the subsystem.
        """
        super().__init__()

        self._controller = controller
        self.setSetpoint(initial_position)
        self.addChild("PID Controller", self._controller)
        self._enabled = False

    def periodic(self):
        """
        Executes the PID control logic during each periodic update.

        This method is called synchronously from the subsystem's periodic() method.
        """
        if self._enabled:
            self.useOutput(
                self._controller.calculate(self.getMeasurement()), self.getSetpoint()
            )

    def getController(self) -> PIDController:
        """
        Returns the PIDController used by the subsystem.

        :return: The PIDController.
        """
        return self._controller

    def setSetpoint(self, setpoint: float):
        """
        Sets the setpoint for the subsystem.

        :param setpoint: The setpoint for the subsystem.
        """
        self._controller.setSetpoint(setpoint)

    def getSetpoint(self) -> float:
        """
        Returns the current setpoint of the subsystem.

        :return: The current setpoint.
        """
        return self._controller.getSetpoint()

    def useOutput(self, output: float, setpoint: float):
        """
        Uses the output from the PIDController.

        :param output: The output of the PIDController.
        :param setpoint: The setpoint of the PIDController (for feedforward).
        """
        raise NotImplementedError(f"{self.__class__} must implement useOutput")

    def getMeasurement(self) -> float:
        """
        Returns the measurement of the process variable used by the PIDController.

        :return: The measurement of the process variable.
        """
        raise NotImplementedError(f"{self.__class__} must implement getMeasurement")

    def enable(self):
        """Enables the PID control. Resets the controller."""
        self._enabled = True
        self._controller.reset()

    def disable(self):
        """Disables the PID control. Sets output to zero."""
        self._enabled = False
        self.useOutput(0, 0)

    def isEnabled(self) -> bool:
        """
        Returns whether the controller is enabled.

        :return: Whether the controller is enabled.
        """
        return self._enabled
