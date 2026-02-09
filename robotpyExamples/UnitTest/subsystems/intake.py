#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib

from constants import IntakeConstants


class Intake:
    def __init__(self) -> None:
        self.motor = wpilib.PWMSparkMax(IntakeConstants.kMotorPort)
        self.piston = wpilib.DoubleSolenoid(
            0,
            wpilib.PneumaticsModuleType.CTREPCM,
            IntakeConstants.kPistonFwdChannel,
            IntakeConstants.kPistonRevChannel,
        )

    def deploy(self) -> None:
        self.piston.set(wpilib.DoubleSolenoid.Value.kForward)

    def retract(self) -> None:
        self.piston.set(wpilib.DoubleSolenoid.Value.kReverse)
        self.motor.set(0)  # turn off the motor

    def activate(self, speed: float) -> None:
        if self.isDeployed():
            self.motor.set(speed)
        else:  # if piston isn't open, do nothing
            self.motor.set(0)

    def isDeployed(self) -> bool:
        return self.piston.get() == wpilib.DoubleSolenoid.Value.kForward
