#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import wpilib
import wpilib.simulation
import wpimath


class Drivetrain:
    """Represents a differential drive style drivetrain."""

    # 3 meters per second.
    kMaxVelocity = 3.0
    # 1/2 rotation per second.
    kMaxAngularVelocity = math.pi

    kTrackwidth = 0.381 * 2
    kWheelRadius = 0.0508
    kEncoderResolution = -4096

    def __init__(self) -> None:
        self.leftLeader = wpilib.PWMSparkMax(1)
        self.leftFollower = wpilib.PWMSparkMax(2)
        self.rightLeader = wpilib.PWMSparkMax(3)
        self.rightFollower = wpilib.PWMSparkMax(4)

        self.leftEncoder = wpilib.Encoder(0, 1)
        self.rightEncoder = wpilib.Encoder(2, 3)

        self.leftPIDController = wpimath.PIDController(8.5, 0, 0)
        self.rightPIDController = wpimath.PIDController(8.5, 0, 0)

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.kFlat)

        self.kinematics = wpimath.DifferentialDriveKinematics(self.kTrackwidth)
        self.odometry = wpimath.DifferentialDriveOdometry(
            self.imu.getRotation2d(),
            self.leftEncoder.getDistance(),
            self.rightEncoder.getDistance(),
        )

        # Gains are for example purposes only - must be determined for your own
        # robot!
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 3)

        # Simulation classes help us simulate our robot
        self.leftEncoderSim = wpilib.simulation.EncoderSim(self.leftEncoder)
        self.rightEncoderSim = wpilib.simulation.EncoderSim(self.rightEncoder)
        self.fieldSim = wpilib.Field2d()
        self.drivetrainSystem = wpimath.Models.differentialDriveFromSysId(
            1.98, 0.2, 1.5, 0.3
        )
        self.drivetrainSimulator = wpilib.simulation.DifferentialDrivetrainSim(
            self.drivetrainSystem,
            self.kTrackwidth,
            wpimath.DCMotor.CIM(2),
            8,
            self.kWheelRadius,
        )

        # Subsystem constructor.
        self.leftLeader.addFollower(self.leftFollower)
        self.rightLeader.addFollower(self.rightFollower)

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.rightLeader.setInverted(True)

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

        self.rightLeader.setInverted(True)
        wpilib.SmartDashboard.putData("Field", self.fieldSim)

    def setVelocities(self, velocities: wpimath.DifferentialDriveWheelVelocities) -> None:
        """Sets velocities to the drivetrain motors."""
        leftFeedforward = self.feedforward.calculate(velocities.left)
        rightFeedforward = self.feedforward.calculate(velocities.right)
        leftOutput = self.leftPIDController.calculate(
            self.leftEncoder.getRate(), velocities.left
        )
        rightOutput = self.rightPIDController.calculate(
            self.rightEncoder.getRate(), velocities.right
        )

        self.leftLeader.setVoltage(leftOutput + leftFeedforward)
        self.rightLeader.setVoltage(rightOutput + rightFeedforward)

    def drive(self, xVelocity: float, rot: float) -> None:
        """Controls the robot using arcade drive.

        :param xVelocity: the velocity for the x axis
        :param rot: the rotation
        """
        self.setVelocities(
            self.kinematics.toWheelVelocities(wpimath.ChassisVelocities(xVelocity, 0, rot))
        )

    def updateOdometry(self) -> None:
        """Update robot odometry."""
        self.odometry.update(
            self.imu.getRotation2d(),
            self.leftEncoder.getDistance(),
            self.rightEncoder.getDistance(),
        )

    def resetOdometry(self, pose: wpimath.Pose2d) -> None:
        """Resets robot odometry."""
        self.drivetrainSimulator.setPose(pose)
        self.odometry.resetPosition(
            self.imu.getRotation2d(),
            self.leftEncoder.getDistance(),
            self.rightEncoder.getDistance(),
            pose,
        )

    def getPose(self) -> wpimath.Pose2d:
        """Check the current robot pose."""
        return self.odometry.getPose()

    def simulationPeriodic(self) -> None:
        """Update our simulation. This should be run every robot loop in simulation."""
        # To update our simulation, we set motor voltage inputs, update the
        # simulation, and write the simulated positions and velocities to our
        # simulated encoder and gyro. We negate the right side so that positive
        # voltages make the right side move forward.
        self.drivetrainSimulator.setInputs(
            self.leftLeader.get() * wpilib.RobotController.getInputVoltage(),
            self.rightLeader.get() * wpilib.RobotController.getInputVoltage(),
        )
        self.drivetrainSimulator.update(0.02)

        self.leftEncoderSim.setDistance(self.drivetrainSimulator.getLeftPosition())
        self.leftEncoderSim.setRate(self.drivetrainSimulator.getLeftVelocity())
        self.rightEncoderSim.setDistance(self.drivetrainSimulator.getRightPosition())
        self.rightEncoderSim.setRate(self.drivetrainSimulator.getRightVelocity())
        # self.gyroSim.setAngle(-self.drivetrainSimulator.getHeading().getDegrees())

    def periodic(self) -> None:
        """Update odometry - this should be run every robot loop."""
        self.updateOdometry()
        self.fieldSim.setRobotPose(self.odometry.getPose())
