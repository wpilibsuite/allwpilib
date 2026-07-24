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
        self.shooter_motor = PWMSparkMax(ShooterConstants.SHOOTER_MOTOR_PORT)

        # The motor on the feeder wheels.
        self.feeder_motor = PWMSparkMax(ShooterConstants.FEEDER_MOTOR_PORT)

        # The shooter wheel encoder
        self.shooter_encoder = Encoder(
            ShooterConstants.ENCODER_PORTS[0],
            ShooterConstants.ENCODER_PORTS[1],
            ShooterConstants.ENCODER_REVERSED,
        )

        # Create a new SysId routine for characterizing the shooter.
        self.sys_id_routine = SysIdRoutine(
            # Empty config defaults to 1 volt/second ramp rate and 7 volt step voltage.
            SysIdRoutine.Config(),
            SysIdRoutine.Mechanism(
                # Tell SysId how to plumb the driving voltage to the motor(s).
                self.shooter_motor.set_voltage,
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
        self.shooter_feedback = wpimath.PIDController(ShooterConstants.P, 0, 0)
        # Feedforward controller to run the shooter wheel in closed-loop, set the constants equal to
        # those calculated by SysId
        self.shooter_feedforward = wpimath.SimpleMotorFeedforward(
            ShooterConstants.S,
            ShooterConstants.V / wpimath.units.rotations_to_radians(1),
            ShooterConstants.A / wpimath.units.rotations_to_radians(1),
        )

        # Sets the distance per pulse for the encoders
        self.shooter_encoder.set_distance_per_pulse(
            ShooterConstants.ENCODER_DISTANCE_PER_PULSE
        )

    def _log(self, sys_id_routine: SysIdRoutineLog) -> None:
        # Record a frame for the shooter motor.
        sys_id_routine.motor("shooter-wheel").voltage(
            self.shooter_motor.get() * RobotController.get_battery_voltage()
        ).angular_position(self.shooter_encoder.get_distance()).angular_velocity(
            self.shooter_encoder.get_rate()
        )

    def run_shooter(self, shooter_velocity: Callable[[], float]) -> Command:
        """Returns a command that runs the shooter at a specifc velocity.

        :param shooterVelocity: The commanded shooter wheel velocity in rotations per second
        """

        # Run shooter wheel at the desired velocity using a PID controller and feedforward.
        def _run_shooter() -> None:
            target_velocity = shooter_velocity()
            target_velocity_radians = wpimath.units.rotations_to_radians(
                target_velocity
            )
            self.shooter_motor.set_voltage(
                self.shooter_feedback.calculate(
                    self.shooter_encoder.get_rate(), target_velocity
                )
                + self.shooter_feedforward.calculate(target_velocity_radians)
            )
            self.feeder_motor.set_throttle(ShooterConstants.FEEDER_VELOCITY)

        def _stop_motors(interrupted: bool) -> None:
            self.shooter_motor.stop_motor()
            self.feeder_motor.stop_motor()

        return self.run(_run_shooter).finally_do(_stop_motors).with_name("runShooter")

    def sys_id_quasistatic(self, direction: SysIdRoutine.Direction) -> Command:
        """Returns a command that will execute a quasistatic test in the given direction.

        :param direction: The direction (forward or reverse) to run the test in
        """

        return self.sys_id_routine.quasistatic(direction)

    def sys_id_dynamic(self, direction: SysIdRoutine.Direction) -> Command:
        """Returns a command that will execute a dynamic test in the given direction.

        :param direction: The direction (forward or reverse) to run the test in
        """

        return self.sys_id_routine.dynamic(direction)
