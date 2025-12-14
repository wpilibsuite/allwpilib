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

    kMaxVelocity = 3.0  # 3 meters per second
    kMaxAngularVelocity = math.pi  # 1/2 rotation per second

    def __init__(self) -> None:
        self.frontLeftMotor = wpilib.PWMSparkMax(1)
        self.frontRightMotor = wpilib.PWMSparkMax(2)
        self.backLeftMotor = wpilib.PWMSparkMax(3)
        self.backRightMotor = wpilib.PWMSparkMax(4)

        self.frontLeftEncoder = wpilib.Encoder(0, 1)
        self.frontRightEncoder = wpilib.Encoder(2, 3)
        self.backLeftEncoder = wpilib.Encoder(4, 5)
        self.backRightEncoder = wpilib.Encoder(6, 7)

        self.frontLeftLocation = wpimath.Translation2d(0.381, 0.381)
        self.frontRightLocation = wpimath.Translation2d(0.381, -0.381)
        self.backLeftLocation = wpimath.Translation2d(-0.381, 0.381)
        self.backRightLocation = wpimath.Translation2d(-0.381, -0.381)

        self.frontLeftPIDController = wpimath.PIDController(1, 0, 0)
        self.frontRightPIDController = wpimath.PIDController(1, 0, 0)
        self.backLeftPIDController = wpimath.PIDController(1, 0, 0)
        self.backRightPIDController = wpimath.PIDController(1, 0, 0)

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.kFlat)

        self.kinematics = wpimath.MecanumDriveKinematics(
            self.frontLeftLocation,
            self.frontRightLocation,
            self.backLeftLocation,
            self.backRightLocation,
        )

        # Here we use MecanumDrivePoseEstimator so that we can fuse odometry readings. The numbers
        # used below are robot specific, and should be tuned.
        self.poseEstimator = wpimath.MecanumDrivePoseEstimator(
            self.kinematics,
            self.imu.getRotation2d(),
            self.getCurrentDistances(),
            wpimath.Pose2d(),
            (0.05, 0.05, wpimath.units.degreesToRadians(5)),
            (0.5, 0.5, wpimath.units.degreesToRadians(30)),
        )

        # Gains are for example purposes only - must be determined for your own robot!
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 3)

        self.imu.resetYaw()

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.frontRightMotor.setInverted(True)
        self.backRightMotor.setInverted(True)

    def getCurrentState(self) -> wpimath.MecanumDriveWheelVelocities:
        """Returns the current state of the drivetrain.

        :returns: The current state of the drivetrain.
        """
        return wpimath.MecanumDriveWheelVelocities(
            self.frontLeftEncoder.getRate(),
            self.frontRightEncoder.getRate(),
            self.backLeftEncoder.getRate(),
            self.backRightEncoder.getRate(),
        )

    def getCurrentDistances(self) -> wpimath.MecanumDriveWheelPositions:
        """Returns the current distances measured by the drivetrain.

        :returns: The current distances measured by the drivetrain.
        """
        positions = wpimath.MecanumDriveWheelPositions()
        positions.frontLeft = self.frontLeftEncoder.getDistance()
        positions.frontRight = self.frontRightEncoder.getDistance()
        positions.rearLeft = self.backLeftEncoder.getDistance()
        positions.rearRight = self.backRightEncoder.getDistance()
        return positions

    def setVelocities(self, velocities: wpimath.MecanumDriveWheelVelocities) -> None:
        """Set the desired velocities for each wheel.

        :param velocities: The desired wheel velocities.
        """
        frontLeftFeedforward = self.feedforward.calculate(velocities.frontLeft)
        frontRightFeedforward = self.feedforward.calculate(velocities.frontRight)
        backLeftFeedforward = self.feedforward.calculate(velocities.rearLeft)
        backRightFeedforward = self.feedforward.calculate(velocities.rearRight)

        frontLeftOutput = self.frontLeftPIDController.calculate(
            self.frontLeftEncoder.getRate(), velocities.frontLeft
        )
        frontRightOutput = self.frontRightPIDController.calculate(
            self.frontRightEncoder.getRate(), velocities.frontRight
        )
        backLeftOutput = self.backLeftPIDController.calculate(
            self.backLeftEncoder.getRate(), velocities.rearLeft
        )
        backRightOutput = self.backRightPIDController.calculate(
            self.backRightEncoder.getRate(), velocities.rearRight
        )

        self.frontLeftMotor.setVoltage(frontLeftOutput + frontLeftFeedforward)
        self.frontRightMotor.setVoltage(frontRightOutput + frontRightFeedforward)
        self.backLeftMotor.setVoltage(backLeftOutput + backLeftFeedforward)
        self.backRightMotor.setVoltage(backRightOutput + backRightFeedforward)

    def drive(
        self,
        xVelocity: float,
        yVelocity: float,
        rot: float,
        fieldRelative: bool,
        period: float,
    ) -> None:
        """Method to drive the robot using joystick info.

        :param xVelocity: Velocity of the robot in the x direction (forward).
        :param yVelocity: Velocity of the robot in the y direction (sideways).
        :param rot: Angular rate of the robot.
        :param fieldRelative: Whether the provided x and y velocities are relative to the field.
        """
        chassisVelocities = wpimath.ChassisVelocities(xVelocity, yVelocity, rot)
        if fieldRelative:
            chassisVelocities = chassisVelocities.toRobotRelative(
                self.poseEstimator.getEstimatedPosition().rotation()
            )
        self.setVelocities(
            self.kinematics.toWheelVelocities(chassisVelocities.discretize(period)).desaturate(
                self.kMaxVelocity
            )
        )

    def updateOdometry(self) -> None:
        """Updates the field relative position of the robot."""
        self.poseEstimator.update(self.imu.getRotation2d(), self.getCurrentDistances())

        # Also apply vision measurements. We use 0.3 seconds in the past as an example -- on
        # a real robot, this must be calculated based either on latency or timestamps.
        self.poseEstimator.addVisionMeasurement(
            ExampleGlobalMeasurementSensor.getEstimatedGlobalPose(
                self.poseEstimator.getEstimatedPosition()
            ),
            wpilib.Timer.getTimestamp() - 0.3,
        )
