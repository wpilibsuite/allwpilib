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

    kMaxSpeed = 3.0  # meters per second
    kMaxAngularSpeed = 2 * math.pi  # one rotation per second

    kTrackwidth = 0.381 * 2  # meters
    kWheelRadius = 0.0508  # meters
    kEncoderResolution = 4096  # counts per revolution

    def __init__(self) -> None:
        self.leftLeader = wpilib.PWMSparkMax(1)
        self.leftFollower = wpilib.PWMSparkMax(2)
        self.rightLeader = wpilib.PWMSparkMax(3)
        self.rightFollower = wpilib.PWMSparkMax(4)

        # Make sure both motors for each side are in the same group
        self.leftLeader.addFollower(self.leftFollower)
        self.rightLeader.addFollower(self.rightFollower)

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.rightLeader.setInverted(True)

        self.leftEncoder = wpilib.Encoder(0, 1)
        self.rightEncoder = wpilib.Encoder(2, 3)

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.kFlat)

        self.leftPIDController = wpimath.PIDController(1.0, 0.0, 0.0)
        self.rightPIDController = wpimath.PIDController(1.0, 0.0, 0.0)

        self.kinematics = wpimath.DifferentialDriveKinematics(self.kTrackwidth)

        # Gains are for example purposes only - must be determined for your own robot!
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 3)

        self.imu.resetYaw()

        # Set the distance per pulse for the drive encoders. We can simply use the
        # distance traveled for one rotation of the wheel divided by the encoder
        # resolution.
        self.leftEncoder.setDistancePerPulse(
            2 * math.pi * self.kWheelRadius / self.kEncoderResolution
        )
        self.rightEncoder.setDistancePerPulse(
            2 * math.pi * self.kWheelRadius / self.kEncoderResolution
        )

        self.leftEncoder.reset()
        self.rightEncoder.reset()

        self.odometry = wpimath.DifferentialDriveOdometry(
            self.imu.getRotation2d(),
            self.leftEncoder.getDistance(),
            self.rightEncoder.getDistance(),
        )

    def setSpeeds(self, speeds: wpimath.DifferentialDriveWheelSpeeds) -> None:
        """Sets the desired wheel speeds.

        :param speeds: The desired wheel speeds.
        """
        leftFeedforward = self.feedforward.calculate(speeds.left)
        rightFeedforward = self.feedforward.calculate(speeds.right)

        leftOutput = self.leftPIDController.calculate(
            self.leftEncoder.getRate(), speeds.left
        )
        rightOutput = self.rightPIDController.calculate(
            self.rightEncoder.getRate(), speeds.right
        )

        # Controls the left and right sides of the robot using the calculated outputs
        self.leftLeader.setVoltage(leftOutput + leftFeedforward)
        self.rightLeader.setVoltage(rightOutput + rightFeedforward)

    def drive(self, xSpeed: float, rot: float) -> None:
        """Drives the robot with the given linear velocity and angular velocity.

        :param xSpeed: Linear velocity in m/s.
        :param rot: Angular velocity in rad/s.
        """
        wheelSpeeds = self.kinematics.toWheelSpeeds(
            wpimath.ChassisSpeeds(xSpeed, 0.0, rot)
        )
        self.setSpeeds(wheelSpeeds)

    def updateOdometry(self) -> None:
        """Updates the field-relative position."""
        self.odometry.update(
            self.imu.getRotation2d(),
            self.leftEncoder.getDistance(),
            self.rightEncoder.getDistance(),
        )
