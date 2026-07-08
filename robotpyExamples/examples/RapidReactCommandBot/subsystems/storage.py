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
        self.motor = wpilib.PWMSparkMax(StorageConstants.kMotorPort)
        self.ballSensor = wpilib.DigitalInput(StorageConstants.kBallSensorPort)

        # Expose trigger from subsystem to improve readability and ease
        # inter-subsystem communications
        # Whether the ball storage is full.
        self.hasCargo = Trigger(self.ballSensor.get)

        # Set default command to turn off the storage motor and then idle
        self.setDefaultCommand(
            self.runOnce(self.motor.disable)
            .andThen(self.run(lambda: None))
            .withName("Idle")
        )

    def runCommand(self) -> Command:
        """Returns a command that runs the storage motor indefinitely."""
        return self.run(lambda: self.motor.set(1)).withName("run")
