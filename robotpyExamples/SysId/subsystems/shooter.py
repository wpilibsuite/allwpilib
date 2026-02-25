#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import Callable

from commands2 import Command, Subsystem
from commands2.sysid import SysIdRoutine
from wpilib import Encoder, PWMSparkMax, RobotController
from wpilib.sysid import SysIdRoutineLog

import wpimath
import wpimath.units

from constants import ShooterConstants


class Shooter(Subsystem):
    """Represents the shooter subsystem."""

    def __init__(self) -> None:
        """Creates a new Shooter subsystem."""
        super().__init__()

        # The motor on the shooter wheel .
        self.shooter_motor = PWMSparkMax(ShooterConstants.kShooterMotorPort)

        # The motor on the feeder wheels.
        self.feeder_motor = PWMSparkMax(ShooterConstants.kFeederMotorPort)

        # The shooter wheel encoder
        self.shooter_encoder = Encoder(
            ShooterConstants.kEncoderPorts[0],
            ShooterConstants.kEncoderPorts[1],
            ShooterConstants.kEncoderReversed,
        )

        # Create a new SysId routine for characterizing the shooter.
        self.sys_id_routine = SysIdRoutine(
            # Empty config defaults to 1 volt/second ramp rate and 7 volt step voltage.
            SysIdRoutine.Config(),
            SysIdRoutine.Mechanism(
                # Tell SysId how to plumb the driving voltage to the motor(s).
                self.shooter_motor.setVoltage,
                # Tell SysId how to record a frame of data for each motor on the mechanism being
                # characterized.
                self._log,
                # Tell SysId to make generated commands require this subsystem, suffix test state in
                # WPILog with this subsystem's name ("shooter")
                self,
            ),
        )

        # PID controller to run the shooter wheel in closed-loop, set the constants equal to those
        # calculated by SysId
        self.shooter_feedback = wpimath.PIDController(ShooterConstants.kP, 0, 0)
        # Feedforward controller to run the shooter wheel in closed-loop, set the constants equal to
        # those calculated by SysId
        self.shooter_feedforward = wpimath.SimpleMotorFeedforwardRadians(
            ShooterConstants.kS,
            ShooterConstants.kV / wpimath.units.rotationsToRadians(1),
            ShooterConstants.kA / wpimath.units.rotationsToRadians(1),
        )

        # Sets the distance per pulse for the encoders
        self.shooter_encoder.setDistancePerPulse(
            ShooterConstants.kEncoderDistancePerPulse
        )

    def _log(self, sys_id_routine: SysIdRoutineLog) -> None:
        # Record a frame for the shooter motor.
        sys_id_routine.motor("shooter-wheel").voltage(
            self.shooter_motor.get() * RobotController.getBatteryVoltage()
        ).angularPosition(self.shooter_encoder.getDistance()).angularVelocity(
            self.shooter_encoder.getRate()
        )

    def runShooter(self, shooterSpeed: Callable[[], float]) -> Command:
        """Returns a command that runs the shooter at a specifc velocity.

        :param shooterSpeed: The commanded shooter wheel speed in rotations per second
        """

        # Run shooter wheel at the desired speed using a PID controller and feedforward.
        def _run_shooter() -> None:
            target_speed = shooterSpeed()
            target_speed_radians = wpimath.units.rotationsToRadians(target_speed)
            self.shooter_motor.setVoltage(
                self.shooter_feedback.calculate(
                    self.shooter_encoder.getRate(), target_speed
                )
                + self.shooter_feedforward.calculate(target_speed_radians)
            )
            self.feeder_motor.set(ShooterConstants.kFeederSpeed)

        def _stop_motors(interrupted: bool) -> None:
            self.shooter_motor.stopMotor()
            self.feeder_motor.stopMotor()

        return self.run(_run_shooter).finallyDo(_stop_motors).withName("runShooter")

    def sysIdQuasistatic(self, direction: SysIdRoutine.Direction) -> Command:
        """Returns a command that will execute a quasistatic test in the given direction.

        :param direction: The direction (forward or reverse) to run the test in
        """

        return self.sys_id_routine.quasistatic(direction)

    def sysIdDynamic(self, direction: SysIdRoutine.Direction) -> Command:
        """Returns a command that will execute a dynamic test in the given direction.

        :param direction: The direction (forward or reverse) to run the test in
        """

        return self.sys_id_routine.dynamic(direction)
