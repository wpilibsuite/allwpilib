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
        self.shooterMotor = wpilib.PWMSparkMax(ShooterConstants.kShooterMotorPort)
        self.feederMotor = wpilib.PWMSparkMax(ShooterConstants.kFeederMotorPort)
        self.shooterEncoder = wpilib.Encoder(
            ShooterConstants.kEncoderPorts[0],
            ShooterConstants.kEncoderPorts[1],
            ShooterConstants.kEncoderReversed,
        )
        self.shooterFeedforward = wpimath.SimpleMotorFeedforwardMeters(
            ShooterConstants.kS, ShooterConstants.kV
        )
        self.shooterFeedback = wpimath.PIDController(ShooterConstants.kP, 0.0, 0.0)

        self.shooterFeedback.setTolerance(ShooterConstants.kShooterToleranceRPS)
        self.shooterEncoder.setDistancePerPulse(
            ShooterConstants.kEncoderDistancePerPulse
        )

        # Set default command to turn off both the shooter and feeder motors, and then idle
        self.setDefaultCommand(
            self.runOnce(
                lambda: (
                    self.shooterMotor.disable(),
                    self.feederMotor.disable(),
                )
            )
            .andThen(self.run(lambda: None))
            .withName("Idle")
        )

    def shootCommand(self, setpointRotationsPerSecond: float) -> Command:
        """Returns a command to shoot the balls currently stored in the robot. Spins the shooter
        flywheel up to the specified setpoint, and then runs the feeder motor.

        :param setpointRotationsPerSecond: The desired shooter velocity
        """

        def _run_shooter() -> None:
            self.shooterMotor.set(
                self.shooterFeedforward.calculate(setpointRotationsPerSecond)
                + self.shooterFeedback.calculate(
                    self.shooterEncoder.getRate(), setpointRotationsPerSecond
                )
            )

        return cmd.parallel(
            # Run the shooter flywheel at the desired setpoint using feedforward and feedback
            self.run(_run_shooter),
            # Wait until the shooter has reached the setpoint, and then run the feeder
            cmd.waitUntil(self.shooterFeedback.atSetpoint).andThen(
                InstantCommand(lambda: self.feederMotor.set(1))
            ),
        ).withName("Shoot")
