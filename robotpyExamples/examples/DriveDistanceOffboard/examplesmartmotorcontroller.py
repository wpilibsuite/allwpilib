#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import enum


class ExampleSmartMotorController:
    """A simplified stub class that simulates the API of a common "smart" motor controller.
    Has no actual functionality.
    """

    class PIDMode(enum.Enum):
        POSITION = enum.auto()
        VELOCITY = enum.auto()
        MOVEMENT_WITCHCRAFT = enum.auto()

    def __init__(self, port: int) -> None:
        """Creates a new ExampleSmartMotorController.

        Args:
            port: The port for the controller.
        """
        super().__init__()
        self._velocity = 0.0
        self._inverted = False
        self._leader = None

    def set_pid(self, kp: float, ki: float, kd: float) -> None:
        """Example method for setting the PID gains of the smart controller.

        Args:
            kp: The proportional gain.
            ki: The integral gain.
            kd: The derivative gain.
        """
        pass

    def set_setpoint(
        self, mode: PIDMode, setpoint: float, arb_feedforward: float
    ) -> None:
        """Example method for setting the setpoint of the smart controller in PID mode.

        Args:
            mode: The mode of the PID controller.
            setpoint: The controller setpoint.
            arb_feedforward: An arbitrary feedforward output (from -1 to 1).
        """
        pass

    def follow(self, leader: "ExampleSmartMotorController") -> None:
        """Places this motor controller in follower mode.

        Args:
            leader: The leader to follow.
        """
        self._leader = leader

    def get_encoder_distance(self) -> float:
        """Returns the encoder distance.

        Returns:
            The current encoder distance.
        """
        return 0

    def get_encoder_rate(self) -> float:
        """Returns the encoder rate.

        Returns:
            The current encoder rate.
        """
        return 0

    def reset_encoder(self) -> None:
        """Resets the encoder to zero distance."""
        pass

    def set_throttle(self, velocity: float) -> None:
        self._velocity = -velocity if self._inverted else velocity

    def get_throttle(self) -> float:
        return self._velocity

    def set_inverted(self, is_inverted: bool) -> None:
        self._inverted = is_inverted

    def get_inverted(self) -> bool:
        return self._inverted

    def disable(self) -> None:
        self._velocity = 0.0

    def stop_motor(self) -> None:
        self._velocity = 0.0
