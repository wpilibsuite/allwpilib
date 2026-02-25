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

    kMaxSpeed = 3.0  # 3 meters per second
    kMaxAngularSpeed = math.pi  # 1/2 rotation per second

    def __init__(self) -> None:
        self.frontLeftLocation = wpimath.Translation2d(0.381, 0.381)
        self.frontRightLocation = wpimath.Translation2d(0.381, -0.381)
        self.backLeftLocation = wpimath.Translation2d(-0.381, 0.381)
        self.backRightLocation = wpimath.Translation2d(-0.381, -0.381)

        self.frontLeft = swervemodule.SwerveModule(1, 2, 0, 1, 2, 3)
        self.frontRight = swervemodule.SwerveModule(3, 4, 4, 5, 6, 7)
        self.backLeft = swervemodule.SwerveModule(5, 6, 8, 9, 10, 11)
        self.backRight = swervemodule.SwerveModule(7, 8, 12, 13, 14, 15)

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.kFlat)

        self.kinematics = wpimath.SwerveDrive4Kinematics(
            self.frontLeftLocation,
            self.frontRightLocation,
            self.backLeftLocation,
            self.backRightLocation,
        )

        # Here we use SwerveDrivePoseEstimator so that we can fuse odometry readings. The numbers
        # used below are robot specific, and should be tuned.
        self.poseEstimator = wpimath.SwerveDrive4PoseEstimator(
            self.kinematics,
            self.imu.getRotation2d(),
            (
                self.frontLeft.getPosition(),
                self.frontRight.getPosition(),
                self.backLeft.getPosition(),
                self.backRight.getPosition(),
            ),
            wpimath.Pose2d(),
            (0.05, 0.05, wpimath.units.degreesToRadians(5)),
            (0.5, 0.5, wpimath.units.degreesToRadians(30)),
        )

        self.imu.resetYaw()

    def drive(
        self,
        xSpeed: float,
        ySpeed: float,
        rot: float,
        fieldRelative: bool,
        period: float,
    ) -> None:
        """Method to drive the robot using joystick info.

        :param xSpeed: Speed of the robot in the x direction (forward).
        :param ySpeed: Speed of the robot in the y direction (sideways).
        :param rot: Angular rate of the robot.
        :param fieldRelative: Whether the provided x and y speeds are relative to the field.
        """
        chassisSpeeds = wpimath.ChassisSpeeds(xSpeed, ySpeed, rot)
        if fieldRelative:
            chassisSpeeds = chassisSpeeds.toRobotRelative(
                self.poseEstimator.getEstimatedPosition().rotation()
            )

        chassisSpeeds = chassisSpeeds.discretize(period)

        states = self.kinematics.toSwerveModuleStates(chassisSpeeds)
        wpimath.SwerveDrive4Kinematics.desaturateWheelSpeeds(states, self.kMaxSpeed)

        self.frontLeft.setDesiredState(states[0])
        self.frontRight.setDesiredState(states[1])
        self.backLeft.setDesiredState(states[2])
        self.backRight.setDesiredState(states[3])

    def updateOdometry(self) -> None:
        """Updates the field relative position of the robot."""
        self.poseEstimator.update(
            self.imu.getRotation2d(),
            (
                self.frontLeft.getPosition(),
                self.frontRight.getPosition(),
                self.backLeft.getPosition(),
                self.backRight.getPosition(),
            ),
        )

        # Also apply vision measurements. We use 0.3 seconds in the past as an example -- on
        # a real robot, this must be calculated based either on latency or timestamps.
        self.poseEstimator.addVisionMeasurement(
            ExampleGlobalMeasurementSensor.getEstimatedGlobalPose(
                self.poseEstimator.getEstimatedPosition()
            ),
            wpilib.Timer.getTimestamp() - 0.3,
        )
