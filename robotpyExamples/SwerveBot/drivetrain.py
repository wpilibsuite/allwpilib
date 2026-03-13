#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math
import wpilib
import swervemodule
import wpimath

kMaxVelocity = 3.0  # 3 meters per second
kMaxAngularVelocity = math.pi  # 1/2 rotation per second


class Drivetrain:
    """
    Represents a swerve drive style drivetrain.
    """

    def __init__(self) -> None:
        self.frontLeftLocation = wpimath.Translation2d(0.381, 0.381)
        self.frontRightLocation = wpimath.Translation2d(0.381, -0.381)
        self.backLeftLocation = wpimath.Translation2d(-0.381, 0.381)
        self.backRightLocation = wpimath.Translation2d(-0.381, -0.381)

        self.frontLeft = swervemodule.SwerveModule(1, 2, 0, 1, 2, 3)
        self.frontRight = swervemodule.SwerveModule(3, 4, 4, 5, 6, 7)
        self.backLeft = swervemodule.SwerveModule(5, 6, 8, 9, 10, 11)
        self.backRight = swervemodule.SwerveModule(7, 8, 12, 13, 14, 15)

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.FLAT)

        self.kinematics = wpimath.SwerveDrive4Kinematics(
            self.frontLeftLocation,
            self.frontRightLocation,
            self.backLeftLocation,
            self.backRightLocation,
        )

        self.odometry = wpimath.SwerveDrive4Odometry(
            self.kinematics,
            self.imu.getRotation2d(),
            (
                self.frontLeft.getPosition(),
                self.frontRight.getPosition(),
                self.backLeft.getPosition(),
                self.backRight.getPosition(),
            ),
        )

        self.imu.resetYaw()

    def drive(
        self,
        xVelocity: float,
        yVelocity: float,
        rot: float,
        fieldRelative: bool,
        periodSeconds: float,
    ) -> None:
        """
        Method to drive the robot using joystick info.
        :param xVelocity: Velocity of the robot in the x direction (forward).
        :param yVelocity: Velocity of the robot in the y direction (sideways).
        :param rot: Angular rate of the robot.
        :param fieldRelative: Whether the provided x and y velocities are relative to the field.
        :param periodSeconds: Time
        """
        chassisVelocities = wpimath.ChassisVelocities(xVelocity, yVelocity, rot)
        if fieldRelative:
            chassisVelocities = chassisVelocities.toRobotRelative(
                self.imu.getRotation2d()
            )

        chassisVelocities = chassisVelocities.discretize(periodSeconds)

        velocities = wpimath.SwerveDrive4Kinematics.desaturateWheelVelocities(
            self.kinematics.toSwerveModuleVelocities(chassisVelocities),
            kMaxVelocity,
        )

        self.frontLeft.setDesiredVelocity(velocities[0])
        self.frontRight.setDesiredVelocity(velocities[1])
        self.backLeft.setDesiredVelocity(velocities[2])
        self.backRight.setDesiredVelocity(velocities[3])

    def updateOdometry(self) -> None:
        """Updates the field relative position of the robot."""
        self.odometry.update(
            self.imu.getRotation2d(),
            (
                self.frontLeft.getPosition(),
                self.frontRight.getPosition(),
                self.backLeft.getPosition(),
                self.backRight.getPosition(),
            ),
        )
