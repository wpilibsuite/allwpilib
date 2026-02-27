#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import Callable

from commands2 import Command, Subsystem
import wpilib
import wpimath

from constants import DriveConstants


class Drive(Subsystem):
    def __init__(self) -> None:
        """Creates a new Drive subsystem."""
        super().__init__()

        # The motors on the left side of the drive.
        self.leftLeader = wpilib.PWMSparkMax(DriveConstants.kLeftMotor1Port)
        self.leftFollower = wpilib.PWMSparkMax(DriveConstants.kLeftMotor2Port)

        # The motors on the right side of the drive.
        self.rightLeader = wpilib.PWMSparkMax(DriveConstants.kRightMotor1Port)
        self.rightFollower = wpilib.PWMSparkMax(DriveConstants.kRightMotor2Port)

        # The robot's drive
        self.drive = wpilib.DifferentialDrive(self.leftLeader, self.rightLeader)

        # The left-side drive encoder
        self.leftEncoder = wpilib.Encoder(
            DriveConstants.kLeftEncoderPorts[0],
            DriveConstants.kLeftEncoderPorts[1],
            DriveConstants.kLeftEncoderReversed,
        )

        # The right-side drive encoder
        self.rightEncoder = wpilib.Encoder(
            DriveConstants.kRightEncoderPorts[0],
            DriveConstants.kRightEncoderPorts[1],
            DriveConstants.kRightEncoderReversed,
        )

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.kFlat)
        self.controller = wpimath.ProfiledPIDController(
            DriveConstants.kTurnP,
            DriveConstants.kTurnI,
            DriveConstants.kTurnD,
            wpimath.TrapezoidProfile.Constraints(
                DriveConstants.kMaxTurnRateDegPerS,
                DriveConstants.kMaxTurnAccelerationDegPerSSquared,
            ),
        )
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(
            DriveConstants.kS, DriveConstants.kV, DriveConstants.kA
        )

        self.leftLeader.addFollower(self.leftFollower)
        self.rightLeader.addFollower(self.rightFollower)

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.rightLeader.setInverted(True)

        # Sets the distance per pulse for the encoders
        self.leftEncoder.setDistancePerPulse(DriveConstants.kEncoderDistancePerPulse)
        self.rightEncoder.setDistancePerPulse(DriveConstants.kEncoderDistancePerPulse)

        # Set the controller to be continuous (because it is an angle controller)
        self.controller.enableContinuousInput(-180, 180)
        # Set the controller tolerance - the delta tolerance ensures the robot is stationary at the
        # setpoint before it is considered as having reached the reference
        self.controller.setTolerance(
            DriveConstants.kTurnToleranceDeg, DriveConstants.kTurnRateToleranceDegPerS
        )

    def arcadeDriveCommand(
        self, fwd: Callable[[], float], rot: Callable[[], float]
    ) -> Command:
        """Returns a command that drives the robot with arcade controls.

        :param fwd: the commanded forward movement
        :param rot: the commanded rotation
        """
        # A split-stick arcade command, with forward/backward controlled by the left
        # hand, and turning controlled by the right.
        return self.run(lambda: self.drive.arcadeDrive(fwd(), rot())).withName(
            "arcadeDrive"
        )

    def driveDistanceCommand(self, distance: float, speed: float) -> Command:
        """Returns a command that drives the robot forward a specified distance at a specified
        speed.

        :param distance: The distance to drive forward in meters
        :param speed: The fraction of max speed at which to drive
        """
        return (
            self.runOnce(
                lambda: (
                    self.leftEncoder.reset(),
                    self.rightEncoder.reset(),
                )
            )
            .andThen(self.run(lambda: self.drive.arcadeDrive(speed, 0)))
            .until(
                lambda: max(
                    self.leftEncoder.getDistance(), self.rightEncoder.getDistance()
                )
                >= distance
            )
            .finallyDo(lambda interrupted: self.drive.stopMotor())
        )

    def turnToAngleCommand(self, angleDeg: float) -> Command:
        """Returns a command that turns to robot to the specified angle using a motion profile and
        PID controller.

        :param angleDeg: The angle to turn to
        """

        def _reset() -> None:
            self.controller.reset(self.imu.getRotation2d().degrees())

        def _run() -> None:
            rotation_output = self.controller.calculate(
                self.imu.getRotation2d().degrees(), angleDeg
            )
            feedforward = self.feedforward.calculate(
                self.controller.getSetpoint().velocity
            )
            self.drive.arcadeDrive(
                0,
                rotation_output
                + feedforward / wpilib.RobotController.getBatteryVoltage(),
            )

        return (
            self.startRun(_reset, _run)
            .until(self.controller.atGoal)
            .finallyDo(lambda interrupted: self.drive.arcadeDrive(0, 0))
        )
