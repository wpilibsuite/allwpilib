#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import wpilib
import wpimath
import wpimath.units

import swervemodule
from exampleglobalmeasurementsensor import ExampleGlobalMeasurementSensor


class Drivetrain:
    """Represents a swerve drive style drivetrain."""

    MAX_VELOCITY = 3.0  # 3 meters per second
    MAX_ANGULAR_VELOCITY = math.pi  # 1/2 rotation per second

    def __init__(self) -> None:
        self.front_left_location = wpimath.Translation2d(0.381, 0.381)
        self.front_right_location = wpimath.Translation2d(0.381, -0.381)
        self.back_left_location = wpimath.Translation2d(-0.381, 0.381)
        self.back_right_location = wpimath.Translation2d(-0.381, -0.381)

        self.front_left = swervemodule.SwerveModule(1, 2, 0, 1, 2, 3)
        self.front_right = swervemodule.SwerveModule(3, 4, 4, 5, 6, 7)
        self.back_left = swervemodule.SwerveModule(5, 6, 8, 9, 10, 11)
        self.back_right = swervemodule.SwerveModule(7, 8, 12, 13, 14, 15)

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.FLAT)

        self.kinematics = wpimath.SwerveDrive4Kinematics(
            self.front_left_location,
            self.front_right_location,
            self.back_left_location,
            self.back_right_location,
        )

        # Here we use SwerveDrivePoseEstimator so that we can fuse odometry readings. The numbers
        # used below are robot specific, and should be tuned.
        self.pose_estimator = wpimath.SwerveDrive4PoseEstimator(
            self.kinematics,
            self.imu.get_rotation2d(),
            (
                self.front_left.get_position(),
                self.front_right.get_position(),
                self.back_left.get_position(),
                self.back_right.get_position(),
            ),
            wpimath.Pose2d(),
            (0.05, 0.05, wpimath.units.degrees_to_radians(5)),
            (0.5, 0.5, wpimath.units.degrees_to_radians(30)),
        )

        self.imu.reset_yaw()

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

        chassis_velocities = chassis_velocities.discretize(period)

        velocities = wpimath.SwerveDrive4Kinematics.desaturate_wheel_velocities(
            self.kinematics.to_swerve_module_velocities(chassis_velocities),
            self.MAX_VELOCITY,
        )

        self.front_left.set_desired_velocity(velocities[0])
        self.front_right.set_desired_velocity(velocities[1])
        self.back_left.set_desired_velocity(velocities[2])
        self.back_right.set_desired_velocity(velocities[3])

    def update_odometry(self) -> None:
        """Updates the field relative position of the robot."""
        self.pose_estimator.update(
            self.imu.get_rotation2d(),
            (
                self.front_left.get_position(),
                self.front_right.get_position(),
                self.back_left.get_position(),
                self.back_right.get_position(),
            ),
        )

        # Also apply vision measurements. We use 0.3 seconds in the past as an example -- on
        # a real robot, this must be calculated based either on latency or timestamps.
        self.pose_estimator.add_vision_measurement(
            ExampleGlobalMeasurementSensor.get_estimated_global_pose(
                self.pose_estimator.get_estimated_position()
            ),
            wpilib.Timer.get_timestamp() - 0.3,
        )
