#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import wpilib
import wpimath

WHEEL_RADIUS = 0.0508
ENCODER_RESOLUTION = 4096

MODULE_MAX_ANGULAR_VELOCITY = math.pi
MODULE_MAX_ANGULAR_ACCELERATION = math.tau  # radians per second squared


class SwerveModule:
    def __init__(
        self,
        drive_motor_channel: int,
        turning_motor_channel: int,
        drive_encoder_channel_a: int,
        drive_encoder_channel_b: int,
        turning_encoder_channel_a: int,
        turning_encoder_channel_b: int,
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
        self.drive_motor = wpilib.PWMSparkMax(drive_motor_channel)
        self.turning_motor = wpilib.PWMSparkMax(turning_motor_channel)

        self.drive_encoder = wpilib.Encoder(
            drive_encoder_channel_a, drive_encoder_channel_b
        )
        self.turning_encoder = wpilib.Encoder(
            turning_encoder_channel_a, turning_encoder_channel_b
        )

        # Gains are for example purposes only - must be determined for your own robot!
        self.drive_pid_controller = wpimath.PIDController(1, 0, 0)

        # Gains are for example purposes only - must be determined for your own robot!
        self.turning_pid_controller = wpimath.ProfiledPIDController(
            1,
            0,
            0,
            wpimath.TrapezoidProfile.Constraints(
                MODULE_MAX_ANGULAR_VELOCITY, MODULE_MAX_ANGULAR_ACCELERATION
            ),
        )

        # Gains are for example purposes only - must be determined for your own robot!
        self.drive_feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 3)
        self.turn_feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 0.5)

        # Set the distance per pulse for the drive encoder. We can simply use the
        # distance traveled for one rotation of the wheel divided by the encoder
        # resolution.
        self.drive_encoder.set_distance_per_pulse(
            math.tau * WHEEL_RADIUS / ENCODER_RESOLUTION
        )

        # Set the distance (in this case, angle) in radians per pulse for the turning encoder.
        # This is the the angle through an entire rotation (2 * pi) divided by the
        # encoder resolution.
        self.turning_encoder.set_distance_per_pulse(math.tau / ENCODER_RESOLUTION)

        # Limit the PID Controller's input range between -pi and pi and set the input
        # to be continuous.
        self.turning_pid_controller.enable_continuous_input(-math.pi, math.pi)

    def get_state(self) -> wpimath.SwerveModuleVelocity:
        """Returns the current state of the module.

        :returns: The current state of the module.
        """
        return wpimath.SwerveModuleVelocity(
            self.drive_encoder.get_rate(),
            wpimath.Rotation2d(self.turning_encoder.get_distance()),
        )

    def get_position(self) -> wpimath.SwerveModulePosition:
        """Returns the current position of the module.

        :returns: The current position of the module.
        """
        return wpimath.SwerveModulePosition(
            self.drive_encoder.get_distance(),
            wpimath.Rotation2d(self.turning_encoder.get_distance()),
        )

    def set_desired_velocity(
        self, desired_velocity: wpimath.SwerveModuleVelocity
    ) -> None:
        """Sets the desired state for the module.

        :param desiredVelocity: Desired state with velocity and angle.
        """
        encoder_rotation = wpimath.Rotation2d(self.turning_encoder.get_distance())

        # Optimize the desired velocity to avoid spinning further than 90 degrees, then scale
        # velocity by cosine of angle error. This scales down movement perpendicular to the desired
        # direction of travel that can occur when modules change directions. This results in
        # smoother driving.
        velocity = desired_velocity.optimize(encoder_rotation).cosine_scale(
            encoder_rotation
        )

        # Calculate the drive output from the drive PID controller and feedforward.
        drive_output = self.drive_pid_controller.calculate(
            self.drive_encoder.get_rate(), velocity.velocity
        ) + self.drive_feedforward.calculate(velocity.velocity)

        # Calculate the turning motor output from the turning PID controller and feedforward.
        turn_output = self.turning_pid_controller.calculate(
            self.turning_encoder.get_distance(), velocity.angle.radians()
        ) + self.turn_feedforward.calculate(
            self.turning_pid_controller.get_setpoint().velocity
        )

        self.drive_motor.set_voltage(drive_output)
        self.turning_motor.set_voltage(turn_output)
