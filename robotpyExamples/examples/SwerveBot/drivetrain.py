#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math
import wpilib
import swervemodule
import wpimath

MAX_VELOCITY = 3.0  # 3 meters per second
MAX_ANGULAR_VELOCITY = math.pi  # 1/2 rotation per second


class Drivetrain:
    """
    Represents a swerve drive style drivetrain.
    """

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

        self.odometry = wpimath.SwerveDrive4Odometry(
            self.kinematics,
            self.imu.get_rotation2d(),
            (
                self.front_left.get_position(),
                self.front_right.get_position(),
                self.back_left.get_position(),
                self.back_right.get_position(),
            ),
        )

        self.imu.reset_yaw()

    def drive(
        self,
        x_velocity: float,
        y_velocity: float,
        rot: float,
        field_relative: bool,
        period_seconds: float,
    ) -> None:
        """
        Method to drive the robot using joystick info.
        :param x_velocity: Velocity of the robot in the x direction (forward).
        :param y_velocity: Velocity of the robot in the y direction (sideways).
        :param rot: Angular rate of the robot.
        :param field_relative: Whether the provided x and y velocities are relative to the field.
        :param period_seconds: Time
        """
        chassis_velocities = wpimath.ChassisVelocities(x_velocity, y_velocity, rot)
        if field_relative:
            chassis_velocities = chassis_velocities.to_robot_relative(
                self.imu.get_rotation2d()
            )

        chassis_velocities = chassis_velocities.discretize(period_seconds)

        velocities = wpimath.SwerveDrive4Kinematics.desaturate_wheel_velocities(
            self.kinematics.to_swerve_module_velocities(chassis_velocities),
            MAX_VELOCITY,
        )

        self.front_left.set_desired_velocity(velocities[0])
        self.front_right.set_desired_velocity(velocities[1])
        self.back_left.set_desired_velocity(velocities[2])
        self.back_right.set_desired_velocity(velocities[3])

    def update_odometry(self) -> None:
        """Updates the field relative position of the robot."""
        self.odometry.update(
            self.imu.get_rotation2d(),
            (
                self.front_left.get_position(),
                self.front_right.get_position(),
                self.back_left.get_position(),
                self.back_right.get_position(),
            ),
        )
