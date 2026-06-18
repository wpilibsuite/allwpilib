#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import enum


class ExampleSmartMotorController:
    """A simplified stub class that simulates the API of a common "smart" motor controller.

    Has no actual functionality.
    """

    class PIDMode(enum.Enum):
        kPosition = enum.auto()
        kVelocity = enum.auto()
        kMovementWitchcraft = enum.auto()

    def __init__(self, port: int) -> None:
        """Creates a new ExampleSmartMotorController.

        :param port: The port for the controller.
        """
        self._port = port
        self._inverted = False

    def setPID(self, kp: float, ki: float, kd: float) -> None:
        """Example method for setting the PID gains of the smart controller.

        :param kp: The proportional gain.
        :param ki: The integral gain.
        :param kd: The derivative gain.
        """
        pass

    def setSetpoint(
        self,
        mode: "ExampleSmartMotorController.PIDMode",
        setpoint: float,
        arbFeedforward: float,
    ) -> None:
        """Example method for setting the setpoint of the smart controller in PID mode.

        :param mode: The mode of the PID controller.
        :param setpoint: The controller setpoint.
        :param arbFeedforward: An arbitrary feedforward output (from -1 to 1).
        """
        pass

    def follow(self, leader: "ExampleSmartMotorController") -> None:
        """Places this motor controller in follower mode.

        :param leader: The leader to follow.
        """
        pass

    def getEncoderDistance(self) -> float:
        """Returns the encoder distance.

        :returns: The current encoder distance.
        """
        return 0

    def getEncoderRate(self) -> float:
        """Returns the encoder rate.

        :returns: The current encoder rate.
        """
        return 0

    def resetEncoder(self) -> None:
        """Resets the encoder to zero distance."""
        pass

    def set(self, velocity: float) -> None:
        pass

    def get(self) -> float:
        return 0

    def setInverted(self, isInverted: bool) -> None:
        self._inverted = isInverted

    def getInverted(self) -> bool:
        return self._inverted

    def disable(self) -> None:
        pass

    def stopMotor(self) -> None:
        pass
