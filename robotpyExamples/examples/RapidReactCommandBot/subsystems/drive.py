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
        self.left_leader = wpilib.PWMSparkMax(DriveConstants.LEFT_MOTOR1_PORT)
        self.left_follower = wpilib.PWMSparkMax(DriveConstants.LEFT_MOTOR2_PORT)

        # The motors on the right side of the drive.
        self.right_leader = wpilib.PWMSparkMax(DriveConstants.RIGHT_MOTOR1_PORT)
        self.right_follower = wpilib.PWMSparkMax(DriveConstants.RIGHT_MOTOR2_PORT)

        # The robot's drive
        self.drive = wpilib.DifferentialDrive(self.left_leader, self.right_leader)

        # The left-side drive encoder
        self.left_encoder = wpilib.Encoder(
            DriveConstants.LEFT_ENCODER_PORTS[0],
            DriveConstants.LEFT_ENCODER_PORTS[1],
            DriveConstants.LEFT_ENCODER_REVERSED,
        )

        # The right-side drive encoder
        self.right_encoder = wpilib.Encoder(
            DriveConstants.RIGHT_ENCODER_PORTS[0],
            DriveConstants.RIGHT_ENCODER_PORTS[1],
            DriveConstants.RIGHT_ENCODER_REVERSED,
        )

        self.imu = wpilib.OnboardIMU(wpilib.OnboardIMU.MountOrientation.FLAT)
        self.controller = wpimath.ProfiledPIDController(
            DriveConstants.TURN_P,
            DriveConstants.TURN_I,
            DriveConstants.TURN_D,
            wpimath.TrapezoidProfile.Constraints(
                DriveConstants.MAX_TURN_RATE_DEG_PER_S,
                DriveConstants.MAX_TURN_ACCELERATION_DEG_PER_S_SQUARED,
            ),
        )
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(
            DriveConstants.S, DriveConstants.V, DriveConstants.A
        )

        self.left_leader.add_follower(self.left_follower)
        self.right_leader.add_follower(self.right_follower)

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.right_leader.set_inverted(True)

        # Sets the distance per pulse for the encoders
        self.left_encoder.set_distance_per_pulse(
            DriveConstants.ENCODER_DISTANCE_PER_PULSE
        )
        self.right_encoder.set_distance_per_pulse(
            DriveConstants.ENCODER_DISTANCE_PER_PULSE
        )

        # Set the controller to be continuous (because it is an angle controller)
        self.controller.enable_continuous_input(-180, 180)
        # Set the controller tolerance - the delta tolerance ensures the robot is stationary at the
        # setpoint before it is considered as having reached the reference
        self.controller.set_tolerance(
            DriveConstants.TURN_TOLERANCE_DEG,
            DriveConstants.TURN_RATE_TOLERANCE_DEG_PER_S,
        )

    def arcade_drive_command(
        self, fwd: Callable[[], float], rot: Callable[[], float]
    ) -> Command:
        """Returns a command that drives the robot with arcade controls.

        :param fwd: the commanded forward movement
        :param rot: the commanded rotation
        """
        # A split-stick arcade command, with forward/backward controlled by the left
        # hand, and turning controlled by the right.
        return self.run(lambda: self.drive.arcade_drive(fwd(), rot())).with_name(
            "arcadeDrive"
        )

    def drive_distance_command(self, distance: float, velocity: float) -> Command:
        """Returns a command that drives the robot forward a specified distance at a specified
        velocity.

        :param distance: The distance to drive forward in meters
        :param velocity: The fraction of max velocity at which to drive
        """
        return (
            self.run_once(
                lambda: (
                    self.left_encoder.reset(),
                    self.right_encoder.reset(),
                )
            )
            .and_then(self.run(lambda: self.drive.arcade_drive(velocity, 0)))
            .until(
                lambda: max(
                    self.left_encoder.get_distance(), self.right_encoder.get_distance()
                )
                >= distance
            )
            .finally_do(lambda interrupted: self.drive.stop_motor())
        )

    def turn_to_angle_command(self, angle_deg: float) -> Command:
        """Returns a command that turns to robot to the specified angle using a motion profile and
        PID controller.

        :param angleDeg: The angle to turn to
        """

        def _reset() -> None:
            self.controller.reset(self.imu.get_rotation2d().degrees())

        def _run() -> None:
            rotation_output = self.controller.calculate(
                self.imu.get_rotation2d().degrees(), angle_deg
            )
            feedforward = self.feedforward.calculate(
                self.controller.get_setpoint().velocity
            )
            self.drive.arcade_drive(
                0,
                rotation_output
                + feedforward / wpilib.RobotController.get_battery_voltage(),
            )

        return (
            self.start_run(_reset, _run)
            .until(self.controller.at_goal)
            .finally_do(lambda interrupted: self.drive.arcade_drive(0, 0))
        )
