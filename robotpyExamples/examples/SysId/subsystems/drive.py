#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import Callable

from commands2 import Command, Subsystem
from commands2.sysid import SysIdRoutine
from wpilib import DifferentialDrive, Encoder, PWMSparkMax, RobotController
from wpilib.sysid import SysIdRoutineLog

from constants import DriveConstants


class Drive(Subsystem):
    """Represents the drive subsystem."""

    def __init__(self) -> None:
        """Creates a new Drive subsystem."""
        super().__init__()

        # The motors on the left side of the drive.
        self.left_motor = PWMSparkMax(DriveConstants.LEFT_MOTOR1_PORT)

        # The motors on the right side of the drive.
        self.right_motor = PWMSparkMax(DriveConstants.RIGHT_MOTOR1_PORT)

        # The robot's drive
        self.drive = DifferentialDrive(self.left_motor, self.right_motor)

        # The left-side drive encoder
        self.left_encoder = Encoder(
            DriveConstants.LEFT_ENCODER_PORTS[0],
            DriveConstants.LEFT_ENCODER_PORTS[1],
            DriveConstants.LEFT_ENCODER_REVERSED,
        )

        # The right-side drive encoder
        self.right_encoder = Encoder(
            DriveConstants.RIGHT_ENCODER_PORTS[0],
            DriveConstants.RIGHT_ENCODER_PORTS[1],
            DriveConstants.RIGHT_ENCODER_REVERSED,
        )

        # Create a new SysId routine for characterizing the drive.
        self.sys_id_routine = SysIdRoutine(
            # Empty config defaults to 1 volt/second ramp rate and 7 volt step voltage.
            SysIdRoutine.Config(),
            SysIdRoutine.Mechanism(
                # Tell SysId how to plumb the driving voltage to the motors.
                self._drive_voltage,
                # Tell SysId how to record a frame of data for each motor on the mechanism being
                # characterized.
                self._log,
                # Tell SysId to make generated commands require this subsystem, suffix test state in
                # WPILog with this subsystem's name ("drive")
                self,
            ),
        )

        # Add the second motors on each side of the drivetrain
        self.left_motor.add_follower(PWMSparkMax(DriveConstants.LEFT_MOTOR2_PORT))
        self.right_motor.add_follower(PWMSparkMax(DriveConstants.RIGHT_MOTOR2_PORT))

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.right_motor.set_inverted(True)

        # Sets the distance per pulse for the encoders
        self.left_encoder.set_distance_per_pulse(
            DriveConstants.ENCODER_DISTANCE_PER_PULSE
        )
        self.right_encoder.set_distance_per_pulse(
            DriveConstants.ENCODER_DISTANCE_PER_PULSE
        )

    def _drive_voltage(self, voltage: float) -> None:
        self.left_motor.set_voltage(voltage)
        self.right_motor.set_voltage(voltage)

    def _log(self, sys_id_routine: SysIdRoutineLog) -> None:
        # Record a frame for the left motors. Since these share an encoder, we consider
        # the entire group to be one motor.
        sys_id_routine.motor("drive-left").voltage(
            self.left_motor.get_throttle() * RobotController.get_battery_voltage()
        ).position(self.left_encoder.get_distance()).velocity(
            self.left_encoder.get_rate()
        )
        # Record a frame for the right motors. Since these share an encoder, we consider
        # the entire group to be one motor.
        sys_id_routine.motor("drive-right").voltage(
            self.right_motor.get_throttle() * RobotController.get_battery_voltage()
        ).position(self.right_encoder.get_distance()).velocity(
            self.right_encoder.get_rate()
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

    def sys_id_quasistatic(self, direction: SysIdRoutine.Direction) -> Command:
        """Returns a command that will execute a quasistatic test in the given direction.

        :param direction: The direction (forward or reverse) to run the test in
        """

        return self.sys_id_routine.quasistatic(direction)

    def sys_id_dynamic(self, direction: SysIdRoutine.Direction) -> Command:
        """Returns a command that will execute a dynamic test in the given direction.

        :param direction: The direction (forward or reverse) to run the test in
        """

        return self.sys_id_routine.dynamic(direction)
