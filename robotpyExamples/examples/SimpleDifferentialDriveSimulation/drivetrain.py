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
    MAX_VELOCITY = 3.0
    # 1/2 rotation per second.
    MAX_ANGULAR_VELOCITY = math.pi

    TRACKWIDTH = 0.381 * 2
    WHEEL_RADIUS = 0.0508
    ENCODER_RESOLUTION = -4096

    def __init__(self) -> None:
        self.left_leader = wpilib.PWMSparkMax(1)
        self.left_follower = wpilib.PWMSparkMax(2)
        self.right_leader = wpilib.PWMSparkMax(3)
        self.right_follower = wpilib.PWMSparkMax(4)

        self.left_encoder = wpilib.Encoder(0, 1)
        self.right_encoder = wpilib.Encoder(2, 3)

        self.left_pid_controller = wpimath.PIDController(8.5, 0, 0)
        self.right_pid_controller = wpimath.PIDController(8.5, 0, 0)

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.FLAT)

        self.kinematics = wpimath.DifferentialDriveKinematics(self.TRACKWIDTH)
        self.odometry = wpimath.DifferentialDriveOdometry(
            self.imu.get_rotation2d(),
            self.left_encoder.get_distance(),
            self.right_encoder.get_distance(),
        )

        # Gains are for example purposes only - must be determined for your own
        # robot!
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(1, 3)

        # Simulation classes help us simulate our robot
        self.left_encoder_sim = wpilib.simulation.EncoderSim(self.left_encoder)
        self.right_encoder_sim = wpilib.simulation.EncoderSim(self.right_encoder)
        self.field_sim = wpilib.Field2d()
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

        # Subsystem constructor.
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
            2 * math.pi * self.WHEEL_RADIUS / self.ENCODER_RESOLUTION
        )
        self.right_encoder.set_distance_per_pulse(
            2 * math.pi * self.WHEEL_RADIUS / self.ENCODER_RESOLUTION
        )

        self.left_encoder.reset()
        self.right_encoder.reset()

        self.right_leader.set_inverted(True)
        wpilib.SmartDashboard.put_data("Field", self.field_sim)

    def set_velocities(
        self, velocities: wpimath.DifferentialDriveWheelVelocities
    ) -> None:
        """Sets velocities to the drivetrain motors."""
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
        """Controls the robot using arcade drive.

        :param x_velocity: the velocity for the x axis
        :param rot: the rotation
        """
        self.set_velocities(
            self.kinematics.to_wheel_velocities(
                wpimath.ChassisVelocities(x_velocity, 0, rot)
            )
        )

    def update_odometry(self) -> None:
        """Update robot odometry."""
        self.odometry.update(
            self.imu.get_rotation2d(),
            self.left_encoder.get_distance(),
            self.right_encoder.get_distance(),
        )

    def reset_odometry(self, pose: wpimath.Pose2d) -> None:
        """Resets robot odometry."""
        self.drivetrain_simulator.set_pose(pose)
        self.odometry.reset_position(
            self.imu.get_rotation2d(),
            self.left_encoder.get_distance(),
            self.right_encoder.get_distance(),
            pose,
        )

    def get_pose(self) -> wpimath.Pose2d:
        """Check the current robot pose."""
        return self.odometry.get_pose()

    def simulation_periodic(self) -> None:
        """Update our simulation. This should be run every robot loop in simulation."""
        # To update our simulation, we set motor voltage inputs, update the
        # simulation, and write the simulated positions and velocities to our
        # simulated encoder and gyro. We negate the right side so that positive
        # voltages make the right side move forward.
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
        """Update odometry - this should be run every robot loop."""
        self.update_odometry()
        self.field_sim.set_robot_pose(self.odometry.get_pose())
