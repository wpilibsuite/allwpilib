#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#
import math

import wpilib
import wpimath


class Drivetrain:
    """Represents a differential drive style drivetrain."""

    MAX_VELOCITY = 3.0  # meters per second
    MAX_ANGULAR_VELOCITY = 2 * math.pi  # one rotation per second

    TRACKWIDTH = 0.381 * 2  # meters
    WHEEL_RADIUS = 0.0508  # meters
    ENCODER_RESOLUTION = 4096  # counts per revolution

    def __init__(self) -> None:
        self.left_leader = wpilib.PWMSparkMax(1)
        self.left_follower = wpilib.PWMSparkMax(2)
        self.right_leader = wpilib.PWMSparkMax(3)
        self.right_follower = wpilib.PWMSparkMax(4)

        # Make sure both motors for each side are in the same group
        self.left_leader.add_follower(self.left_follower)
        self.right_leader.add_follower(self.right_follower)

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.right_leader.set_inverted(True)

        self.left_encoder = wpilib.Encoder(0, 1)
        self.right_encoder = wpilib.Encoder(2, 3)

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.FLAT)

        self.left_pid_controller = wpimath.PIDController(1.0, 0.0, 0.0)
        self.right_pid_controller = wpimath.PIDController(1.0, 0.0, 0.0)

        self.kinematics = wpimath.DifferentialDriveKinematics(self.TRACKWIDTH)

        # Gains are for example purposes only - must be determined for your own robot!
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 3)

        self.imu.reset_yaw()

        # Set the distance per pulse for the drive encoders. We can simply use the
        # distance traveled for one rotation of the wheel divided by the encoder
        # resolution.
        self.left_encoder.set_distance_per_pulse(
            2 * math.pi * self.WHEEL_RADIUS / self.ENCODER_RESOLUTION
        )
        self.right_encoder.set_distance_per_pulse(
            2 * math.pi * self.WHEEL_RADIUS / self.ENCODER_RESOLUTION
        )

        self.left_encoder.reset()
        self.right_encoder.reset()

        self.odometry = wpimath.DifferentialDriveOdometry(
            self.imu.get_rotation2d(),
            self.left_encoder.get_distance(),
            self.right_encoder.get_distance(),
        )

    def set_velocities(
        self, velocities: wpimath.DifferentialDriveWheelVelocities
    ) -> None:
        """Sets the desired wheel velocities.

        :param velocities: The desired wheel velocities.
        """
        left_feedforward = self.feedforward.calculate(velocities.left)
        right_feedforward = self.feedforward.calculate(velocities.right)

        left_output = self.left_pid_controller.calculate(
            self.left_encoder.get_rate(), velocities.left
        )
        right_output = self.right_pid_controller.calculate(
            self.right_encoder.get_rate(), velocities.right
        )

        # Controls the left and right sides of the robot using the calculated outputs
        self.left_leader.set_voltage(left_output + left_feedforward)
        self.right_leader.set_voltage(right_output + right_feedforward)

    def drive(self, x_velocity: float, rot: float) -> None:
        """Drives the robot with the given linear velocity and angular velocity.

        :param x_velocity: Linear velocity in m/s.
        :param rot: Angular velocity in rad/s.
        """
        wheel_velocities = self.kinematics.to_wheel_velocities(
            wpimath.ChassisVelocities(x_velocity, 0.0, rot)
        )
        self.set_velocities(wheel_velocities)

    def update_odometry(self) -> None:
        """Updates the field-relative position."""
        self.odometry.update(
            self.imu.get_rotation2d(),
            self.left_encoder.get_distance(),
            self.right_encoder.get_distance(),
        )
