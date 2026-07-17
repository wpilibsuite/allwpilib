#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import Command, Subsystem
import wpilib

from constants import IntakeConstants


class Intake(Subsystem):
    def __init__(self) -> None:
        super().__init__()
        self.motor = wpilib.PWMSparkMax(IntakeConstants.MOTOR_PORT)

        # Double solenoid connected to two channels of a PCM with the default CAN ID
        self.pistons = wpilib.DoubleSolenoid(
            0,
            wpilib.PneumaticsModuleType.CTRE_PCM,
            IntakeConstants.SOLENOID_PORTS[0],
            IntakeConstants.SOLENOID_PORTS[1],
        )

    def intake_command(self) -> Command:
        """Returns a command that deploys the intake, and then runs the intake motor
        indefinitely.
        """
        return (
            self.run_once(lambda: self.pistons.set(wpilib.DoubleSolenoid.Value.FORWARD))
            .and_then(self.run(lambda: self.motor.set_throttle(1.0)))
            .with_name("Intake")
        )

    def retract_command(self) -> Command:
        """Returns a command that turns off and retracts the intake."""
        return self.run_once(
            lambda: (
                self.motor.disable(),
                self.pistons.set(wpilib.DoubleSolenoid.Value.REVERSE),
            )
        ).with_name("Retract")
