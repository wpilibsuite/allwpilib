#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import Command, InstantCommand, Subsystem, cmd
import wpilib
import wpimath

from constants import ShooterConstants


class Shooter(Subsystem):
    def __init__(self) -> None:
        """The shooter subsystem for the robot."""
        super().__init__()
        self.shooter_motor = wpilib.PWMSparkMax(ShooterConstants.SHOOTER_MOTOR_PORT)
        self.feeder_motor = wpilib.PWMSparkMax(ShooterConstants.FEEDER_MOTOR_PORT)
        self.shooter_encoder = wpilib.Encoder(
            ShooterConstants.ENCODER_PORTS[0],
            ShooterConstants.ENCODER_PORTS[1],
            ShooterConstants.ENCODER_REVERSED,
        )
        self.shooter_feedforward = wpimath.SimpleMotorFeedforwardMeters(
            ShooterConstants.S, ShooterConstants.V
        )
        self.shooter_feedback = wpimath.PIDController(ShooterConstants.P, 0.0, 0.0)

        self.shooter_feedback.set_tolerance(ShooterConstants.SHOOTER_TOLERANCE_RPS)
        self.shooter_encoder.set_distance_per_pulse(
            ShooterConstants.ENCODER_DISTANCE_PER_PULSE
        )

        # Set default command to turn off both the shooter and feeder motors, and then idle
        self.set_default_command(
            self.run_once(
                lambda: (
                    self.shooter_motor.disable(),
                    self.feeder_motor.disable(),
                )
            )
            .and_then(self.run(lambda: None))
            .with_name("Idle")
        )

    def shoot_command(self, setpoint_rotations_per_second: float) -> Command:
        """Returns a command to shoot the balls currently stored in the robot. Spins the shooter
        flywheel up to the specified setpoint, and then runs the feeder motor.

        :param setpointRotationsPerSecond: The desired shooter velocity
        """

        def _run_shooter() -> None:
            self.shooter_motor.set(
                self.shooter_feedforward.calculate(setpoint_rotations_per_second)
                + self.shooter_feedback.calculate(
                    self.shooter_encoder.get_rate(), setpoint_rotations_per_second
                )
            )

        return cmd.parallel(
            # Run the shooter flywheel at the desired setpoint using feedforward and feedback
            self.run(_run_shooter),
            # Wait until the shooter has reached the setpoint, and then run the feeder
            cmd.wait_until(self.shooter_feedback.at_setpoint).and_then(
                InstantCommand(lambda: self.feeder_motor.set(1))
            ),
        ).with_name("Shoot")
