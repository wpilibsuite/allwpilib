#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import ntcore
import wpilib
import wpilib.simulation
import wpimath
import wpimath.units
import robotpy_apriltag


class Drivetrain:
    """Represents a differential drive style drivetrain."""

    kMaxSpeed = 3.0  # meters per second
    kMaxAngularSpeed = math.tau  # one rotation per second

    kTrackwidth = 0.381 * 2  # meters
    kWheelRadius = 0.0508  # meters
    kEncoderResolution = 4096

    def __init__(self, cameraToObjectTopic: ntcore.DoubleArrayTopic) -> None:
        self.leftLeader = wpilib.PWMSparkMax(1)
        self.leftFollower = wpilib.PWMSparkMax(2)
        self.rightLeader = wpilib.PWMSparkMax(3)
        self.rightFollower = wpilib.PWMSparkMax(4)

        self.leftEncoder = wpilib.Encoder(0, 1)
        self.rightEncoder = wpilib.Encoder(2, 3)

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.kFlat)

        self.leftPIDController = wpimath.PIDController(1, 0, 0)
        self.rightPIDController = wpimath.PIDController(1, 0, 0)

        self.kinematics = wpimath.DifferentialDriveKinematics(self.kTrackwidth)

        self.robotToCamera = wpimath.Transform3d(
            wpimath.Translation3d(1, 1, 1),
            wpimath.Rotation3d(0, 0, math.pi / 2),
        )

        self.defaultVal = [0.0] * 7
        self.cameraToObjectEntry = cameraToObjectTopic.getEntry(self.defaultVal)

        layout = robotpy_apriltag.AprilTagFieldLayout.loadField(
            robotpy_apriltag.AprilTagField.k2024Crescendo
        )
        self.objectInField = layout.getTagPose(0) or wpimath.Pose3d()

        self.fieldSim = wpilib.Field2d()
        self.fieldApproximation = wpilib.Field2d()

        # Here we use DifferentialDrivePoseEstimator so that we can fuse odometry readings. The
        # numbers used below are robot specific, and should be tuned.
        self.poseEstimator = wpimath.DifferentialDrivePoseEstimator(
            self.kinematics,
            self.imu.getRotation2d(),
            self.leftEncoder.getDistance(),
            self.rightEncoder.getDistance(),
            wpimath.Pose2d(),
            (0.05, 0.05, wpimath.units.degreesToRadians(5)),
            (0.5, 0.5, wpimath.units.degreesToRadians(30)),
        )

        # Gains are for example purposes only - must be determined for your own robot!
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 3)

        # Simulation classes
        self.leftEncoderSim = wpilib.simulation.EncoderSim(self.leftEncoder)
        self.rightEncoderSim = wpilib.simulation.EncoderSim(self.rightEncoder)
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

        self.imu.resetYaw()

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
            math.tau * self.kWheelRadius / self.kEncoderResolution
        )
        self.rightEncoder.setDistancePerPulse(
            math.tau * self.kWheelRadius / self.kEncoderResolution
        )

        self.leftEncoder.reset()
        self.rightEncoder.reset()

        wpilib.SmartDashboard.putData("Field", self.fieldSim)
        wpilib.SmartDashboard.putData("FieldEstimation", self.fieldApproximation)

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

    def publishCameraToObject(
        self,
        objectInField: wpimath.Pose3d,
        robotToCamera: wpimath.Transform3d,
        cameraToObjectEntry: ntcore.DoubleArrayEntry,
        drivetrainSimulator: wpilib.simulation.DifferentialDrivetrainSim,
    ) -> None:
        """Computes and publishes to a network tables topic the transformation from the
        camera's pose to the object's pose. This function exists solely for the
        purposes of simulation, and this would normally be handled by computer vision.

        The object could be a target or a fiducial marker.

        :param objectInField: The object's field-relative position.
        :param robotToCamera: The transformation from the robot's pose to the camera's pose.
        :param cameraToObjectEntry: The networktables entry publishing and querying example
            computer vision measurements.
        """
        robotInField = wpimath.Pose3d(drivetrainSimulator.getPose())
        cameraInField = robotInField.transformBy(robotToCamera)
        cameraToObject = wpimath.Transform3d(cameraInField, objectInField)

        # Publishes double array with Translation3D elements {x, y, z} and Rotation3D elements
        # {w, x, y, z} which describe the cameraToObject transformation.
        quaternion = cameraToObject.rotation().getQuaternion()
        val = [
            cameraToObject.x,
            cameraToObject.y,
            cameraToObject.z,
            quaternion.w,
            quaternion.x,
            quaternion.y,
            quaternion.z,
        ]
        cameraToObjectEntry.set(val)

    def objectToRobotPose(
        self,
        objectInField: wpimath.Pose3d,
        robotToCamera: wpimath.Transform3d,
        cameraToObjectEntry: ntcore.DoubleArrayEntry,
    ) -> wpimath.Pose3d:
        """Queries the camera-to-object transformation from networktables to compute the robot's
        field-relative pose from vision measurements.

        The object could be a target or a fiducial marker.

        :param objectInField: The object's field-relative pose.
        :param robotToCamera: The transformation from the robot's pose to the camera's pose.
        :param cameraToObjectEntry: The networktables entry publishing and querying example
            computer vision measurements.
        """
        val = cameraToObjectEntry.get()

        # Reconstruct cameraToObject Transform3d from networktables.
        translation = wpimath.Translation3d(val[0], val[1], val[2])
        rotation = wpimath.Rotation3d(
            wpimath.Quaternion(val[3], val[4], val[5], val[6])
        )
        cameraToObject = wpimath.Transform3d(translation, rotation)

        cameraInField = objectInField.transformBy(cameraToObject.inverse())
        robotInField = cameraInField.transformBy(robotToCamera.inverse())
        return robotInField

    def updateOdometry(self) -> None:
        """Updates the field-relative position."""
        self.poseEstimator.update(
            self.imu.getRotation2d(),
            self.leftEncoder.getDistance(),
            self.rightEncoder.getDistance(),
        )

        # Publish cameraToObject transformation to networktables --this would normally be handled by
        # the computer vision solution.
        self.publishCameraToObject(
            self.objectInField,
            self.robotToCamera,
            self.cameraToObjectEntry,
            self.drivetrainSimulator,
        )

        # Compute the robot's field-relative position exclusively from vision measurements.
        visionMeasurement3d = self.objectToRobotPose(
            self.objectInField, self.robotToCamera, self.cameraToObjectEntry
        )

        # Convert robot pose from Pose3d to Pose2d needed to apply vision measurements.
        visionMeasurement2d = visionMeasurement3d.toPose2d()

        # Apply vision measurements. For simulation purposes only, we don't input a latency delay --
        # on a real robot, this must be calculated based either on known latency or timestamps.
        self.poseEstimator.addVisionMeasurement(
            visionMeasurement2d,
            wpilib.Timer.getTimestamp(),
        )

    def simulationPeriodic(self) -> None:
        """This function is called periodically during simulation."""
        # To update our simulation, we set motor voltage inputs, update the
        # simulation, and write the simulated positions and velocities to our
        # simulated encoder and gyro.
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
        """This function is called periodically, no matter the mode."""
        self.updateOdometry()
        self.fieldSim.setRobotPose(self.drivetrainSimulator.getPose())
        self.fieldApproximation.setRobotPose(self.poseEstimator.getEstimatedPosition())
