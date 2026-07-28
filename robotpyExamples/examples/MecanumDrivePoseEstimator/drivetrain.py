#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import wpilib
import wpimath
import wpimath.units

from exampleglobalmeasurementsensor import ExampleGlobalMeasurementSensor


class Drivetrain:
    """Represents a mecanum drive style drivetrain."""

    MAX_VELOCITY = 3.0  # 3 meters per second
    MAX_ANGULAR_VELOCITY = math.pi  # 1/2 rotation per second

    def __init__(self) -> None:
        self.front_left_motor = wpilib.PWMSparkMax(1)
        self.front_right_motor = wpilib.PWMSparkMax(2)
        self.back_left_motor = wpilib.PWMSparkMax(3)
        self.back_right_motor = wpilib.PWMSparkMax(4)

        self.front_left_encoder = wpilib.Encoder(0, 1)
        self.front_right_encoder = wpilib.Encoder(2, 3)
        self.back_left_encoder = wpilib.Encoder(4, 5)
        self.back_right_encoder = wpilib.Encoder(6, 7)

        self.front_left_location = wpimath.Translation2d(0.381, 0.381)
        self.front_right_location = wpimath.Translation2d(0.381, -0.381)
        self.back_left_location = wpimath.Translation2d(-0.381, 0.381)
        self.back_right_location = wpimath.Translation2d(-0.381, -0.381)

        self.front_left_pid_controller = wpimath.PIDController(1, 0, 0)
        self.front_right_pid_controller = wpimath.PIDController(1, 0, 0)
        self.back_left_pid_controller = wpimath.PIDController(1, 0, 0)
        self.back_right_pid_controller = wpimath.PIDController(1, 0, 0)

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.FLAT)

        self.kinematics = wpimath.MecanumDriveKinematics(
            self.front_left_location,
            self.front_right_location,
            self.back_left_location,
            self.back_right_location,
        )

        # Here we use MecanumDrivePoseEstimator so that we can fuse odometry readings. The numbers
        # used below are robot specific, and should be tuned.
        self.pose_estimator = wpimath.MecanumDrivePoseEstimator(
            self.kinematics,
            self.imu.get_rotation2d(),
            self.get_current_distances(),
            wpimath.Pose2d(),
            (0.05, 0.05, wpimath.units.degrees_to_radians(5)),
            (0.5, 0.5, wpimath.units.degrees_to_radians(30)),
        )

        # Gains are for example purposes only - must be determined for your own robot!
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 3)

        self.imu.reset_yaw()

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.front_right_motor.set_inverted(True)
        self.back_right_motor.set_inverted(True)

    def get_current_state(self) -> wpimath.MecanumDriveWheelVelocities:
        """Returns the current state of the drivetrain.

        :returns: The current state of the drivetrain.
        """
        return wpimath.MecanumDriveWheelVelocities(
            self.front_left_encoder.get_rate(),
            self.front_right_encoder.get_rate(),
            self.back_left_encoder.get_rate(),
            self.back_right_encoder.get_rate(),
        )

    def get_current_distances(self) -> wpimath.MecanumDriveWheelPositions:
        """Returns the current distances measured by the drivetrain.

        :returns: The current distances measured by the drivetrain.
        """
        positions = wpimath.MecanumDriveWheelPositions()
        positions.front_left = self.front_left_encoder.get_distance()
        positions.front_right = self.front_right_encoder.get_distance()
        positions.rear_left = self.back_left_encoder.get_distance()
        positions.rear_right = self.back_right_encoder.get_distance()
        return positions

    def set_velocities(self, velocities: wpimath.MecanumDriveWheelVelocities) -> None:
        """Set the desired velocities for each wheel.

        :param velocities: The desired wheel velocities.
        """
        front_left_feedforward = self.feedforward.calculate(velocities.front_left)
        front_right_feedforward = self.feedforward.calculate(velocities.front_right)
        back_left_feedforward = self.feedforward.calculate(velocities.rear_left)
        back_right_feedforward = self.feedforward.calculate(velocities.rear_right)

        front_left_output = self.front_left_pid_controller.calculate(
            self.front_left_encoder.get_rate(), velocities.front_left
        )
        front_right_output = self.front_right_pid_controller.calculate(
            self.front_right_encoder.get_rate(), velocities.front_right
        )
        back_left_output = self.back_left_pid_controller.calculate(
            self.back_left_encoder.get_rate(), velocities.rear_left
        )
        back_right_output = self.back_right_pid_controller.calculate(
            self.back_right_encoder.get_rate(), velocities.rear_right
        )

        self.front_left_motor.set_voltage(front_left_output + front_left_feedforward)
        self.front_right_motor.set_voltage(front_right_output + front_right_feedforward)
        self.back_left_motor.set_voltage(back_left_output + back_left_feedforward)
        self.back_right_motor.set_voltage(back_right_output + back_right_feedforward)

    def drive(
        self,
        x_velocity: float,
        y_velocity: float,
        rot: float,
        field_relative: bool,
        period: float,
    ) -> None:
        """Method to drive the robot using joystick info.

        :param x_velocity: Velocity of the robot in the x direction (forward).
        :param y_velocity: Velocity of the robot in the y direction (sideways).
        :param rot: Angular rate of the robot.
        :param field_relative: Whether the provided x and y velocities are relative to the field.
        """
        chassis_velocities = wpimath.ChassisVelocities(x_velocity, y_velocity, rot)
        if field_relative:
            chassis_velocities = chassis_velocities.to_robot_relative(
                self.pose_estimator.get_estimated_position().rotation()
            )
        self.set_velocities(
            self.kinematics.to_wheel_velocities(
                chassis_velocities.discretize(period)
            ).desaturate(self.MAX_VELOCITY)
        )

    def update_odometry(self) -> None:
        """Updates the field relative position of the robot."""
        self.pose_estimator.update(
            self.imu.get_rotation2d(), self.get_current_distances()
        )

        # Also apply vision measurements. We use 0.3 seconds in the past as an example -- on
        # a real robot, this must be calculated based either on latency or timestamps.
        self.pose_estimator.add_vision_measurement(
            ExampleGlobalMeasurementSensor.get_estimated_global_pose(
                self.pose_estimator.get_estimated_position()
            ),
            wpilib.Timer.get_timestamp() - 0.3,
        )
