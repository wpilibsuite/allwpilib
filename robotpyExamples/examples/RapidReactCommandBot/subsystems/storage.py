#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import Command, Subsystem
from commands2.button import Trigger
import wpilib

from constants import StorageConstants


class Storage(Subsystem):
    def __init__(self) -> None:
        """Create a new Storage subsystem."""
        super().__init__()
        self.motor = wpilib.PWMSparkMax(StorageConstants.MOTOR_PORT)
        self.ball_sensor = wpilib.DigitalInput(StorageConstants.BALL_SENSOR_PORT)

        # Expose trigger from subsystem to improve readability and ease
        # inter-subsystem communications
        # Whether the ball storage is full.
        self.has_cargo = Trigger(self.ball_sensor.get)

        # Set default command to turn off the storage motor and then idle
        self.set_default_command(
            self.run_once(self.motor.disable)
            .and_then(self.run(lambda: None))
            .with_name("Idle")
        )

    def run_command(self) -> Command:
        """Returns a command that runs the storage motor indefinitely."""
        return self.run(lambda: self.motor.set(1)).with_name("run")
