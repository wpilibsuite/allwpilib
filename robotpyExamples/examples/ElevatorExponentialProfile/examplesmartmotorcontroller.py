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
        POSITION = enum.auto()
        VELOCITY = enum.auto()
        MOVEMENT_WITCHCRAFT = enum.auto()

    def __init__(self, port: int) -> None:
        """Creates a new ExampleSmartMotorController.

        :param port: The port for the controller.
        """
        self._port = port
        self._inverted = False

    def set_pid(self, kp: float, ki: float, kd: float) -> None:
        """Example method for setting the PID gains of the smart controller.

        :param kp: The proportional gain.
        :param ki: The integral gain.
        :param kd: The derivative gain.
        """
        pass

    def set_setpoint(
        self,
        mode: "ExampleSmartMotorController.PIDMode",
        setpoint: float,
        arb_feedforward: float,
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

    def get_encoder_distance(self) -> float:
        """Returns the encoder distance.

        :returns: The current encoder distance.
        """
        return 0

    def get_encoder_rate(self) -> float:
        """Returns the encoder rate.

        :returns: The current encoder rate.
        """
        return 0

    def reset_encoder(self) -> None:
        """Resets the encoder to zero distance."""
        pass

    def set(self, velocity: float) -> None:
        pass

    def get(self) -> float:
        return 0

    def set_inverted(self, is_inverted: bool) -> None:
        self._inverted = is_inverted

    def get_inverted(self) -> bool:
        return self._inverted

    def disable(self) -> None:
        pass

    def stop_motor(self) -> None:
        pass
