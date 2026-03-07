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
        self.leftLeader = examplesmartmotorcontroller.ExampleSmartMotorController(
            constants.DriveConstants.kLeftMotor1Port
        )

        self.leftFollower = examplesmartmotorcontroller.ExampleSmartMotorController(
            constants.DriveConstants.kLeftMotor2Port
        )

        # The motors on the right side of the drive.
        self.rightLeader = examplesmartmotorcontroller.ExampleSmartMotorController(
            constants.DriveConstants.kRightMotor1Port
        )

        self.rightFollower = examplesmartmotorcontroller.ExampleSmartMotorController(
            constants.DriveConstants.kRightMotor1Port
        )

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.rightLeader.setInverted(True)

        # You might need to not do this depending on the specific motor controller
        # that you are using -- contact the respective vendor's documentation for
        # more details.
        self.rightFollower.setInverted(True)

        self.leftFollower.follow(self.leftLeader)
        self.rightFollower.follow(self.rightLeader)

        self.leftLeader.setPID(constants.DriveConstants.kp, 0, 0)
        self.rightLeader.setPID(constants.DriveConstants.kp, 0, 0)

        # The feedforward controller (note that these are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!)
        # check DriveConstants for more information.
        self.feedforward = wpimath.SimpleMotorFeedforwardMeters(
            constants.DriveConstants.ks,
            constants.DriveConstants.kv,
            constants.DriveConstants.ka,
        )

        # The robot's drive
        self.drive = wpilib.DifferentialDrive(self.leftLeader, self.rightLeader)

        self.profile = wpimath.TrapezoidProfile(
            wpimath.TrapezoidProfile.Constraints(
                constants.DriveConstants.kMaxVelocity,
                constants.DriveConstants.kMaxAcceleration,
            )
        )
        self.timer = wpilib.Timer()
        self._initialLeftDistance = 0.0
        self._initialRightDistance = 0.0

    def arcadeDrive(self, fwd: float, rot: float):
        """
        Drives the robot using arcade controls.

        :param fwd: the commanded forward movement
        :param rot: the commanded rotation
        """
        self.drive.arcadeDrive(fwd, rot)

    def setDriveStates(
        self,
        currentLeft: wpimath.TrapezoidProfile.State,
        currentRight: wpimath.TrapezoidProfile.State,
        nextLeft: wpimath.TrapezoidProfile.State,
        nextRight: wpimath.TrapezoidProfile.State,
    ):
        """
        Attempts to follow the given drive states using offboard PID.

        :param currentLeft: The current left wheel state.
        :param currentRight: The current right wheel state.
        :param nextLeft: The next left wheel state.
        :param nextRight: The next right wheel state.
        """
        battery_voltage = wpilib.RobotController.getBatteryVoltage()
        left_feedforward = self.feedforward.calculate(
            currentLeft.velocity,
            (nextLeft.velocity - currentLeft.velocity) / constants.DriveConstants.kDt,
        )
        right_feedforward = self.feedforward.calculate(
            currentRight.velocity,
            (nextRight.velocity - currentRight.velocity) / constants.DriveConstants.kDt,
        )
        self.leftLeader.setSetPoint(
            examplesmartmotorcontroller.ExampleSmartMotorController.PIDMode.kPosition,
            currentLeft.position,
            left_feedforward / battery_voltage,
        )

        self.rightLeader.setSetPoint(
            examplesmartmotorcontroller.ExampleSmartMotorController.PIDMode.kPosition,
            currentRight.position,
            right_feedforward / battery_voltage,
        )

    def getLeftEncoderDistance(self) -> float:
        """
        Returns the left drive encoder distance.

        :returns: the left drive encoder distance
        """
        return self.leftLeader.getEncoderDistance()

    def getRightEncoderDistance(self) -> float:
        """
        Returns the right drive encoder distance.

        :returns: the right drive encoder distance
        """
        return self.rightLeader.getEncoderDistance()

    def resetEncoders(self):
        """Resets the drive encoders"""
        self.leftLeader.resetEncoder()
        self.rightLeader.resetEncoder()

    def setMaxOutput(self, maxOutput: float):
        """
        Sets the max output of the drive. Useful for scaling the drive to drive more slowly.

        :param maxOutput: the maximum output to which the drive will be constrained
        """
        self.drive.setMaxOutput(maxOutput)

    def profiledDriveDistance(self, distance: float) -> commands2.Command:
        def on_init():
            self.timer.restart()
            self.resetEncoders()

        def on_execute():
            current_time = self.timer.get()
            current_setpoint = self.profile.calculate(
                current_time,
                wpimath.TrapezoidProfile.State(),
                wpimath.TrapezoidProfile.State(distance, 0),
            )
            next_setpoint = self.profile.calculate(
                current_time + constants.DriveConstants.kDt,
                wpimath.TrapezoidProfile.State(),
                wpimath.TrapezoidProfile.State(distance, 0),
            )
            self.setDriveStates(
                current_setpoint, current_setpoint, next_setpoint, next_setpoint
            )

        def on_end(interrupted: bool):
            self.leftLeader.set(0)
            self.rightLeader.set(0)

        def is_finished() -> bool:
            return self.profile.isFinished(0)

        return commands2.FunctionalCommand(
            on_init, on_execute, on_end, is_finished, self
        )

    def dynamicProfiledDriveDistance(self, distance: float) -> commands2.Command:
        def on_init():
            self.timer.restart()
            self._initialLeftDistance = self.getLeftEncoderDistance()
            self._initialRightDistance = self.getRightEncoderDistance()

        def on_execute():
            current_time = self.timer.get()
            current_left = self.profile.calculate(
                current_time,
                wpimath.TrapezoidProfile.State(self._initialLeftDistance, 0),
                wpimath.TrapezoidProfile.State(self._initialLeftDistance + distance, 0),
            )
            current_right = self.profile.calculate(
                current_time,
                wpimath.TrapezoidProfile.State(self._initialRightDistance, 0),
                wpimath.TrapezoidProfile.State(
                    self._initialRightDistance + distance, 0
                ),
            )
            next_left = self.profile.calculate(
                current_time + constants.DriveConstants.kDt,
                wpimath.TrapezoidProfile.State(self._initialLeftDistance, 0),
                wpimath.TrapezoidProfile.State(self._initialLeftDistance + distance, 0),
            )
            next_right = self.profile.calculate(
                current_time + constants.DriveConstants.kDt,
                wpimath.TrapezoidProfile.State(self._initialRightDistance, 0),
                wpimath.TrapezoidProfile.State(
                    self._initialRightDistance + distance, 0
                ),
            )
            self.setDriveStates(current_left, current_right, next_left, next_right)

        def on_end(interrupted: bool):
            self.leftLeader.set(0)
            self.rightLeader.set(0)

        def is_finished() -> bool:
            return self.profile.isFinished(0)

        return commands2.FunctionalCommand(
            on_init, on_execute, on_end, is_finished, self
        )
