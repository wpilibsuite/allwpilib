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
        self.motor = wpilib.PWMSparkMax(IntakeConstants.kMotorPort)

        # Double solenoid connected to two channels of a PCM with the default CAN ID
        self.pistons = wpilib.DoubleSolenoid(
            0,
            wpilib.PneumaticsModuleType.CTREPCM,
            IntakeConstants.kSolenoidPorts[0],
            IntakeConstants.kSolenoidPorts[1],
        )

    def intakeCommand(self) -> Command:
        """Returns a command that deploys the intake, and then runs the intake motor
        indefinitely.
        """
        return (
            self.runOnce(lambda: self.pistons.set(wpilib.DoubleSolenoid.Value.kForward))
            .andThen(self.run(lambda: self.motor.set(1.0)))
            .withName("Intake")
        )

    def retractCommand(self) -> Command:
        """Returns a command that turns off and retracts the intake."""
        return self.runOnce(
            lambda: (
                self.motor.disable(),
                self.pistons.set(wpilib.DoubleSolenoid.Value.kReverse),
            )
        ).withName("Retract")
