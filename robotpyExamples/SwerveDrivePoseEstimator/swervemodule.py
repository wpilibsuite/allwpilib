#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import wpilib
import wpimath

kWheelRadius = 0.0508
kEncoderResolution = 4096

kModuleMaxAngularVelocity = math.pi
kModuleMaxAngularAcceleration = math.tau  # radians per second squared


class SwerveModule:
    def __init__(
        self,
        driveMotorChannel: int,
        turningMotorChannel: int,
        driveEncoderChannelA: int,
        driveEncoderChannelB: int,
        turningEncoderChannelA: int,
        turningEncoderChannelB: int,
    ) -> None:
        """Constructs a SwerveModule with a drive motor, turning motor, drive encoder and
        turning encoder.

        :param driveMotorChannel: PWM output for the drive motor.
        :param turningMotorChannel: PWM output for the turning motor.
        :param driveEncoderChannelA: DIO input for the drive encoder channel A
        :param driveEncoderChannelB: DIO input for the drive encoder channel B
        :param turningEncoderChannelA: DIO input for the turning encoder channel A
        :param turningEncoderChannelB: DIO input for the turning encoder channel B
        """
        self.driveMotor = wpilib.PWMSparkMax(driveMotorChannel)
        self.turningMotor = wpilib.PWMSparkMax(turningMotorChannel)

        self.driveEncoder = wpilib.Encoder(driveEncoderChannelA, driveEncoderChannelB)
        self.turningEncoder = wpilib.Encoder(
            turningEncoderChannelA, turningEncoderChannelB
        )

        # Gains are for example purposes only - must be determined for your own robot!
        self.drivePIDController = wpimath.PIDController(1, 0, 0)

        # Gains are for example purposes only - must be determined for your own robot!
        self.turningPIDController = wpimath.ProfiledPIDController(
            1,
            0,
            0,
            wpimath.TrapezoidProfile.Constraints(
                kModuleMaxAngularVelocity, kModuleMaxAngularAcceleration
            ),
        )

        # Gains are for example purposes only - must be determined for your own robot!
        self.driveFeedforward = wpimath.SimpleMotorFeedforwardMeters(1, 3)
        self.turnFeedforward = wpimath.SimpleMotorFeedforwardMeters(1, 0.5)

        # Set the distance per pulse for the drive encoder. We can simply use the
        # distance traveled for one rotation of the wheel divided by the encoder
        # resolution.
        self.driveEncoder.setDistancePerPulse(
            math.tau * kWheelRadius / kEncoderResolution
        )

        # Set the distance (in this case, angle) in radians per pulse for the turning encoder.
        # This is the the angle through an entire rotation (2 * pi) divided by the
        # encoder resolution.
        self.turningEncoder.setDistancePerPulse(math.tau / kEncoderResolution)

        # Limit the PID Controller's input range between -pi and pi and set the input
        # to be continuous.
        self.turningPIDController.enableContinuousInput(-math.pi, math.pi)

    def getState(self) -> wpimath.SwerveModuleVelocity:
        """Returns the current state of the module.

        :returns: The current state of the module.
        """
        return wpimath.SwerveModuleVelocity(
            self.driveEncoder.getRate(),
            wpimath.Rotation2d(self.turningEncoder.getDistance()),
        )

    def getPosition(self) -> wpimath.SwerveModulePosition:
        """Returns the current position of the module.

        :returns: The current position of the module.
        """
        return wpimath.SwerveModulePosition(
            self.driveEncoder.getDistance(),
            wpimath.Rotation2d(self.turningEncoder.getDistance()),
        )

    def setDesiredVelocity(self, desiredVelocity: wpimath.SwerveModuleVelocity) -> None:
        """Sets the desired state for the module.

        :param desiredVelocity: Desired state with velocity and angle.
        """
        encoderRotation = wpimath.Rotation2d(self.turningEncoder.getDistance())

        # Optimize the reference state to avoid spinning further than 90 degrees
        desiredVelocity.optimize(encoderRotation)

        # Scale velocity by cosine of angle error. This scales down movement perpendicular to the
        # desired direction of travel that can occur when modules change directions. This results
        # in smoother driving.
        desiredVelocity.cosineScale(encoderRotation)

        # Calculate the drive output from the drive PID controller.
        driveOutput = self.drivePIDController.calculate(
            self.driveEncoder.getRate(), desiredVelocity.velocity
        )

        driveFeedforward = self.driveFeedforward.calculate(desiredVelocity.velocity)

        # Calculate the turning motor output from the turning PID controller.
        turnOutput = self.turningPIDController.calculate(
            self.turningEncoder.getDistance(), desiredVelocity.angle.radians()
        )

        turnFeedforward = self.turnFeedforward.calculate(
            self.turningPIDController.getSetpoint().velocity
        )

        self.driveMotor.setVoltage(driveOutput + driveFeedforward)
        self.turningMotor.setVoltage(turnOutput + turnFeedforward)
