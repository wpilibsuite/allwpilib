#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import commands2
import constants
import examplesmartmotorcontroller
import wpimath


class DriveSubsystem(commands2.Subsystem):
    def __init__(self) -> None:
        """Creates a new DriveSubsystem"""
        super().__init__()

        # The motors on the left side of the drive.
        self.left_leader = examplesmartmotorcontroller.ExampleSmartMotorController(
            constants.DriveConstants.LEFT_MOTOR1_PORT
        )

        self.left_follower = examplesmartmotorcontroller.ExampleSmartMotorController(
            constants.DriveConstants.LEFT_MOTOR2_PORT
        )

        # The motors on the right side of the drive.
        self.right_leader = examplesmartmotorcontroller.ExampleSmartMotorController(
            constants.DriveConstants.RIGHT_MOTOR1_PORT
        )

        self.right_follower = examplesmartmotorcontroller.ExampleSmartMotorController(
            constants.DriveConstants.RIGHT_MOTOR1_PORT
        )

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.right_leader.set_inverted(True)

        # You might need to not do this depending on the specific motor controller
        # that you are using -- contact the respective vendor's documentation for
        # more details.
        self.right_follower.set_inverted(True)

        self.left_follower.follow(self.left_leader)
        self.right_follower.follow(self.right_leader)

        self.left_leader.set_pid(constants.DriveConstants.kp, 0, 0)
        self.right_leader.set_pid(constants.DriveConstants.kp, 0, 0)

        # The feedforward controller (note that these are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!)
        # check DriveConstants for more information.
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(
            constants.DriveConstants.ks,
            constants.DriveConstants.kv,
            constants.DriveConstants.ka,
        )

        # The robot's drive
        self.drive = wpilib.DifferentialDrive(
            self.left_leader.set_throttle, self.right_leader.set_throttle
        )

        self.constraints = wpimath.TrapezoidProfile.Constraints(
            constants.DriveConstants.MAX_VELOCITY,
            constants.DriveConstants.MAX_ACCELERATION,
        )
        self.left_profile = wpimath.TrapezoidProfile.generate(
            self.constraints,
            wpimath.TrapezoidProfile.State(),
            wpimath.TrapezoidProfile.State(),
        )
        self.right_profile = self.left_profile
        self.timer = wpilib.Timer()
        self._initial_left_distance = 0.0
        self._initial_right_distance = 0.0

    def arcade_drive(self, fwd: float, rot: float):
        """
        Drives the robot using arcade controls.

        :param fwd: the commanded forward movement
        :param rot: the commanded rotation
        """
        self.drive.arcade_drive(fwd, rot)

    def set_drive_states(
        self,
        current_left: wpimath.TrapezoidProfileSample,
        current_right: wpimath.TrapezoidProfileSample,
        next_left: wpimath.TrapezoidProfileSample,
        next_right: wpimath.TrapezoidProfileSample,
    ):
        """
        Attempts to follow the given drive states using offboard PID.

        :param currentLeft: The current left wheel state.
        :param currentRight: The current right wheel state.
        :param nextLeft: The next left wheel state.
        :param nextRight: The next right wheel state.
        """
        battery_voltage = wpilib.RobotController.get_battery_voltage()
        left_feedforward = self.feedforward.calculate(
            current_left.velocity,
            (next_left.velocity - current_left.velocity) / constants.DriveConstants.DT,
        )
        right_feedforward = self.feedforward.calculate(
            current_right.velocity,
            (next_right.velocity - current_right.velocity)
            / constants.DriveConstants.DT,
        )
        self.left_leader.set_setpoint(
            examplesmartmotorcontroller.ExampleSmartMotorController.PIDMode.POSITION,
            current_left.position,
            left_feedforward / battery_voltage,
        )

        self.right_leader.set_setpoint(
            examplesmartmotorcontroller.ExampleSmartMotorController.PIDMode.POSITION,
            current_right.position,
            right_feedforward / battery_voltage,
        )

    def get_left_encoder_distance(self) -> float:
        """
        Returns the left drive encoder distance.

        :returns: the left drive encoder distance
        """
        return self.left_leader.get_encoder_distance()

    def get_right_encoder_distance(self) -> float:
        """
        Returns the right drive encoder distance.

        :returns: the right drive encoder distance
        """
        return self.right_leader.get_encoder_distance()

    def reset_encoders(self):
        """Resets the drive encoders"""
        self.left_leader.reset_encoder()
        self.right_leader.reset_encoder()

    def set_max_output(self, max_output: float):
        """
        Sets the max output of the drive. Useful for scaling the drive to drive more slowly.

        :param maxOutput: the maximum output to which the drive will be constrained
        """
        self.drive.set_max_output(max_output)

    def profiled_drive_distance(self, distance: float) -> commands2.Command:
        def on_init():
            # Restart timer so profile setpoints start at the beginning
            self.timer.restart()
            self.reset_encoders()
            # Both encoders start at zero, so they can share a profile
            self.left_profile = wpimath.TrapezoidProfile.generate(
                self.constraints,
                wpimath.TrapezoidProfile.State(),
                wpimath.TrapezoidProfile.State(distance, 0),
            )

        def on_execute():
            # Current state never changes, so we need to use a timer to get the
            # setpoints we need to be at
            current_time = self.timer.get()
            current_setpoint = self.left_profile.sample_at(current_time)
            next_setpoint = self.left_profile.sample_at(
                current_time + constants.DriveConstants.DT
            )
            self.set_drive_states(
                current_setpoint, current_setpoint, next_setpoint, next_setpoint
            )

        def on_end(interrupted: bool):
            self.left_leader.set_throttle(0)
            self.right_leader.set_throttle(0)

        def is_finished() -> bool:
            return self.timer.get() >= self.left_profile.duration()

        return commands2.FunctionalCommand(
            on_init, on_execute, on_end, is_finished, self
        )

    def dynamic_profiled_drive_distance(self, distance: float) -> commands2.Command:
        def on_init():
            # Restart timer so profile setpoints start at the beginning
            self.timer.restart()
            # Store distance so we know the target distance for each encoder
            self._initial_left_distance = self.get_left_encoder_distance()
            self._initial_right_distance = self.get_right_encoder_distance()
            self.left_profile = wpimath.TrapezoidProfile.generate(
                self.constraints,
                wpimath.TrapezoidProfile.State(self._initial_left_distance, 0),
                wpimath.TrapezoidProfile.State(
                    self._initial_left_distance + distance, 0
                ),
            )
            self.right_profile = wpimath.TrapezoidProfile.generate(
                self.constraints,
                wpimath.TrapezoidProfile.State(self._initial_right_distance, 0),
                wpimath.TrapezoidProfile.State(
                    self._initial_right_distance + distance, 0
                ),
            )

        def on_execute():
            # Current state never changes for the duration of the command, so we
            # need to use a timer to get the setpoints we need to be at
            current_time = self.timer.get()
            next_time = current_time + constants.DriveConstants.DT
            current_left = self.left_profile.sample_at(current_time)
            current_right = self.right_profile.sample_at(current_time)
            next_left = self.left_profile.sample_at(next_time)
            next_right = self.right_profile.sample_at(next_time)
            self.set_drive_states(current_left, current_right, next_left, next_right)

        def on_end(interrupted: bool):
            self.left_leader.set_throttle(0)
            self.right_leader.set_throttle(0)

        def is_finished() -> bool:
            return (
                self.timer.get() >= self.left_profile.duration()
                and self.timer.get() >= self.right_profile.duration()
            )

        return commands2.FunctionalCommand(
            on_init, on_execute, on_end, is_finished, self
        )
