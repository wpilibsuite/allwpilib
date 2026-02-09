#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#
import math

import wpilib
import wpimath


class Drivetrain:
    """Represents a mecanum drive style drivetrain."""

    kMaxSpeed = 3.0  # 3 meters per second
    kMaxAngularSpeed = math.pi  # 1/2 rotation per second

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

        self.odometry = wpimath.MecanumDriveOdometry(
            self.kinematics, self.imu.getRotation2d(), self.getCurrentDistances()
        )

        # Gains are for example purposes only - must be determined for your own robot!
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 3)

        self.imu.resetYaw()

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.frontRightMotor.setInverted(True)
        self.backRightMotor.setInverted(True)

    def getCurrentState(self) -> wpimath.MecanumDriveWheelSpeeds:
        """Returns the current state of the drivetrain."""
        return wpimath.MecanumDriveWheelSpeeds(
            self.frontLeftEncoder.getRate(),
            self.frontRightEncoder.getRate(),
            self.backLeftEncoder.getRate(),
            self.backRightEncoder.getRate(),
        )

    def getCurrentDistances(self) -> wpimath.MecanumDriveWheelPositions:
        """Returns the current distances measured by the drivetrain."""
        positions = wpimath.MecanumDriveWheelPositions()
        positions.frontLeft = self.frontLeftEncoder.getDistance()
        positions.frontRight = self.frontRightEncoder.getDistance()
        positions.rearLeft = self.backLeftEncoder.getDistance()
        positions.rearRight = self.backRightEncoder.getDistance()
        return positions

    def setSpeeds(self, speeds: wpimath.MecanumDriveWheelSpeeds) -> None:
        """Sets the desired speeds for each wheel."""
        frontLeftFeedforward = self.feedforward.calculate(speeds.frontLeft)
        frontRightFeedforward = self.feedforward.calculate(speeds.frontRight)
        backLeftFeedforward = self.feedforward.calculate(speeds.rearLeft)
        backRightFeedforward = self.feedforward.calculate(speeds.rearRight)

        frontLeftOutput = self.frontLeftPIDController.calculate(
            self.frontLeftEncoder.getRate(), speeds.frontLeft
        )
        frontRightOutput = self.frontRightPIDController.calculate(
            self.frontRightEncoder.getRate(), speeds.frontRight
        )
        backLeftOutput = self.frontLeftPIDController.calculate(
            self.backLeftEncoder.getRate(), speeds.rearLeft
        )
        backRightOutput = self.frontRightPIDController.calculate(
            self.backRightEncoder.getRate(), speeds.rearRight
        )

        self.frontLeftMotor.setVoltage(frontLeftOutput + frontLeftFeedforward)
        self.frontRightMotor.setVoltage(frontRightOutput + frontRightFeedforward)
        self.backLeftMotor.setVoltage(backLeftOutput + backLeftFeedforward)
        self.backRightMotor.setVoltage(backRightOutput + backRightFeedforward)

    def drive(
        self,
        xSpeed: float,
        ySpeed: float,
        rot: float,
        fieldRelative: bool,
        periodSeconds: float,
    ) -> None:
        """Method to drive the robot using joystick info."""
        chassisSpeeds = wpimath.ChassisSpeeds(xSpeed, ySpeed, rot)
        if fieldRelative:
            chassisSpeeds = chassisSpeeds.toRobotRelative(self.imu.getRotation2d())

        self.setSpeeds(
            self.kinematics.toWheelSpeeds(
                chassisSpeeds.discretize(periodSeconds)
            ).desaturate(self.kMaxSpeed)
        )

    def updateOdometry(self) -> None:
        """Updates the field-relative position."""
        self.odometry.update(self.imu.getRotation2d(), self.getCurrentDistances())
