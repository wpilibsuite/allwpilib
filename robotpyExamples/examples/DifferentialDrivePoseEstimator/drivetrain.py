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

    MAX_VELOCITY = 3.0  # meters per second
    MAX_ANGULAR_VELOCITY = math.tau  # one rotation per second

    TRACKWIDTH = 0.381 * 2  # meters
    WHEEL_RADIUS = 0.0508  # meters
    ENCODER_RESOLUTION = 4096

    def __init__(self, camera_to_object_topic: ntcore.DoubleArrayTopic) -> None:
        self.left_leader = wpilib.PWMSparkMax(1)
        self.left_follower = wpilib.PWMSparkMax(2)
        self.right_leader = wpilib.PWMSparkMax(3)
        self.right_follower = wpilib.PWMSparkMax(4)

        self.left_encoder = wpilib.Encoder(0, 1)
        self.right_encoder = wpilib.Encoder(2, 3)

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.FLAT)

        self.left_pid_controller = wpimath.PIDController(1, 0, 0)
        self.right_pid_controller = wpimath.PIDController(1, 0, 0)

        self.kinematics = wpimath.DifferentialDriveKinematics(self.TRACKWIDTH)

        self.robot_to_camera = wpimath.Transform3d(
            wpimath.Translation3d(1, 1, 1),
            wpimath.Rotation3d(0, 0, math.pi / 2),
        )

        self.default_val = [0.0] * 7
        self.camera_to_object_entry = camera_to_object_topic.get_entry(self.default_val)

        layout = robotpy_apriltag.AprilTagFieldLayout.load_field(
            robotpy_apriltag.AprilTagField.K2024_CRESCENDO
        )
        self.object_in_field = layout.get_tag_pose(0) or wpimath.Pose3d()

        self.field_sim = wpilib.Field2d()
        self.field_approximation = wpilib.Field2d()

        # Here we use DifferentialDrivePoseEstimator so that we can fuse odometry readings. The
        # numbers used below are robot specific, and should be tuned.
        self.pose_estimator = wpimath.DifferentialDrivePoseEstimator(
            self.kinematics,
            self.imu.get_rotation2d(),
            self.left_encoder.get_distance(),
            self.right_encoder.get_distance(),
            wpimath.Pose2d(),
            (0.05, 0.05, wpimath.units.degrees_to_radians(5)),
            (0.5, 0.5, wpimath.units.degrees_to_radians(30)),
        )

        # Gains are for example purposes only - must be determined for your own robot!
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 3)

        # Simulation classes
        self.left_encoder_sim = wpilib.simulation.EncoderSim(self.left_encoder)
        self.right_encoder_sim = wpilib.simulation.EncoderSim(self.right_encoder)
        self.drivetrain_system = wpimath.Models.differential_drive_from_sys_id(
            1.98, 0.2, 1.5, 0.3
        )
        self.drivetrain_simulator = wpilib.simulation.DifferentialDrivetrainSim(
            self.drivetrain_system,
            self.TRACKWIDTH,
            wpimath.DCMotor.cim(2),
            8,
            self.WHEEL_RADIUS,
        )

        self.imu.reset_yaw()

        self.left_leader.add_follower(self.left_follower)
        self.right_leader.add_follower(self.right_follower)

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.right_leader.set_inverted(True)

        # Set the distance per pulse for the drive encoders. We can simply use the
        # distance traveled for one rotation of the wheel divided by the encoder
        # resolution.
        self.left_encoder.set_distance_per_pulse(
            math.tau * self.WHEEL_RADIUS / self.ENCODER_RESOLUTION
        )
        self.right_encoder.set_distance_per_pulse(
            math.tau * self.WHEEL_RADIUS / self.ENCODER_RESOLUTION
        )

        self.left_encoder.reset()
        self.right_encoder.reset()

        wpilib.Telemetry.log("Field", self.field_sim)
        wpilib.Telemetry.log("FieldEstimation", self.field_approximation)

    def set_velocities(
        self, velocities: wpimath.DifferentialDriveWheelVelocities
    ) -> None:
        """Sets the desired wheel velocities.

        :param velocities: The desired wheel velocities.
        """
        left_feedforward = self.feedforward.calculate(velocities.left)
        right_feedforward = self.feedforward.calculate(velocities.right)

        left_output = self.left_pid_controller.calculate(
            self.left_encoder.get_rate(), velocities.left
        )
        right_output = self.right_pid_controller.calculate(
            self.right_encoder.get_rate(), velocities.right
        )
        self.left_leader.set_voltage(left_output + left_feedforward)
        self.right_leader.set_voltage(right_output + right_feedforward)

    def drive(self, x_velocity: float, rot: float) -> None:
        """Drives the robot with the given linear velocity and angular velocity.

        :param x_velocity: Linear velocity in m/s.
        :param rot: Angular velocity in rad/s.
        """
        wheel_velocities = self.kinematics.to_wheel_velocities(
            wpimath.ChassisVelocities(x_velocity, 0.0, rot)
        )
        self.set_velocities(wheel_velocities)

    def publish_camera_to_object(
        self,
        object_in_field: wpimath.Pose3d,
        robot_to_camera: wpimath.Transform3d,
        camera_to_object_entry: ntcore.DoubleArrayEntry,
        drivetrain_simulator: wpilib.simulation.DifferentialDrivetrainSim,
    ) -> None:
        """Computes and publishes to a network tables topic the transformation from the
        camera's pose to the object's pose. This function exists solely for the
        purposes of simulation, and this would normally be handled by computer vision.

        The object could be a target or a fiducial marker.

        :param object_in_field: The object's field-relative position.
        :param robot_to_camera: The transformation from the robot's pose to the camera's pose.
        :param camera_to_object_entry: The networktables entry publishing and querying example
            computer vision measurements.
        """
        robot_in_field = wpimath.Pose3d(drivetrain_simulator.get_pose())
        camera_in_field = robot_in_field.transform_by(robot_to_camera)
        camera_to_object = wpimath.Transform3d(camera_in_field, object_in_field)

        # Publishes double array with Translation3d elements {x, y, z} and Rotation3d elements
        # {w, x, y, z} which describe the camera_to_object transformation.
        quaternion = camera_to_object.rotation().get_quaternion()
        val = [
            camera_to_object.x,
            camera_to_object.y,
            camera_to_object.z,
            quaternion.w,
            quaternion.x,
            quaternion.y,
            quaternion.z,
        ]
        camera_to_object_entry.set(val)

    def object_to_robot_pose(
        self,
        object_in_field: wpimath.Pose3d,
        robot_to_camera: wpimath.Transform3d,
        camera_to_object_entry: ntcore.DoubleArrayEntry,
    ) -> wpimath.Pose3d:
        """Queries the camera-to-object transformation from networktables to compute the robot's
        field-relative pose from vision measurements.

        The object could be a target or a fiducial marker.

        :param object_in_field: The object's field-relative pose.
        :param robot_to_camera: The transformation from the robot's pose to the camera's pose.
        :param camera_to_object_entry: The networktables entry publishing and querying example
            computer vision measurements.
        """
        val = camera_to_object_entry.get()

        # Reconstruct camera_to_object Transform3d from networktables.
        translation = wpimath.Translation3d(val[0], val[1], val[2])
        rotation = wpimath.Rotation3d(
            wpimath.Quaternion(val[3], val[4], val[5], val[6])
        )
        camera_to_object = wpimath.Transform3d(translation, rotation)

        camera_in_field = object_in_field.transform_by(camera_to_object.inverse())
        robot_in_field = camera_in_field.transform_by(robot_to_camera.inverse())
        return robot_in_field

    def update_odometry(self) -> None:
        """Updates the field-relative position."""
        self.pose_estimator.update(
            self.imu.get_rotation2d(),
            self.left_encoder.get_distance(),
            self.right_encoder.get_distance(),
        )

        # Publish camera_to_object transformation to networktables --this would normally be handled by
        # the computer vision solution.
        self.publish_camera_to_object(
            self.object_in_field,
            self.robot_to_camera,
            self.camera_to_object_entry,
            self.drivetrain_simulator,
        )

        # Compute the robot's field-relative position exclusively from vision measurements.
        vision_measurement3d = self.object_to_robot_pose(
            self.object_in_field, self.robot_to_camera, self.camera_to_object_entry
        )

        # Convert robot pose from Pose3d to Pose2d needed to apply vision measurements.
        vision_measurement2d = vision_measurement3d.to_pose2d()

        # Apply vision measurements. For simulation purposes only, we don't input a latency delay --
        # on a real robot, this must be calculated based either on known latency or timestamps.
        self.pose_estimator.add_vision_measurement(
            vision_measurement2d,
            wpilib.Timer.get_timestamp(),
        )

    def simulation_periodic(self) -> None:
        """This function is called periodically during simulation."""
        # To update our simulation, we set motor voltage inputs, update the
        # simulation, and write the simulated positions and velocities to our
        # simulated encoder and gyro.
        self.drivetrain_simulator.set_inputs(
            self.left_leader.get_throttle()
            * wpilib.RobotController.get_input_voltage(),
            self.right_leader.get_throttle()
            * wpilib.RobotController.get_input_voltage(),
        )
        self.drivetrain_simulator.update(0.02)

        self.left_encoder_sim.set_distance(
            self.drivetrain_simulator.get_left_position()
        )
        self.left_encoder_sim.set_rate(self.drivetrain_simulator.get_left_velocity())
        self.right_encoder_sim.set_distance(
            self.drivetrain_simulator.get_right_position()
        )
        self.right_encoder_sim.set_rate(self.drivetrain_simulator.get_right_velocity())
        # self.gyro_sim.set_angle(-self.drivetrain_simulator.get_heading().degrees())

    def periodic(self) -> None:
        """This function is called periodically, no matter the mode."""
        self.update_odometry()
        self.field_sim.set_robot_pose(self.drivetrain_simulator.get_pose())
        self.field_approximation.set_robot_pose(
            self.pose_estimator.get_estimated_position()
        )
