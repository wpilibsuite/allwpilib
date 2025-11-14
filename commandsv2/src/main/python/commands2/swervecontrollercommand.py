# validated: 2024-01-20 DS 192a28af4731 SwerveControllerCommand.java
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
from __future__ import annotations
from typing import Callable, Optional, Union, Tuple, Sequence

from wpimath.controller import (
    HolonomicDriveController,
)
from wpimath.geometry import Pose2d, Rotation2d
from wpimath.kinematics import (
    SwerveDrive2Kinematics,
    SwerveDrive3Kinematics,
    SwerveDrive4Kinematics,
    SwerveDrive6Kinematics,
    SwerveModuleState,
)
from wpimath.trajectory import Trajectory
from wpilib import Timer

from .command import Command
from .subsystem import Subsystem


class SwerveControllerCommand(Command):
    """
    A command that uses two PID controllers (:class:`wpimath.controller.PIDController`)
    and a HolonomicDriveController (:class:`wpimath.controller.HolonomicDriveController`)
    to follow a trajectory (:class:`wpimath.trajectory.Trajectory`) with a swerve drive.

    This command outputs the raw desired Swerve Module States (:class:`wpimath.kinematics.SwerveModuleState`) in an
    array. The desired wheel and module rotation velocities should be taken from those and used in
    velocity PIDs.

    The robot angle controller does not follow the angle given by the trajectory but rather goes
    to the angle given in the final state of the trajectory.
    """

    def __init__(
        self,
        trajectory: Trajectory,
        pose: Callable[[], Pose2d],
        kinematics: Union[
            SwerveDrive2Kinematics,
            SwerveDrive3Kinematics,
            SwerveDrive4Kinematics,
            SwerveDrive6Kinematics,
        ],
        controller: HolonomicDriveController,
        outputModuleStates: Callable[[Sequence[SwerveModuleState]], None],
        requirements: Tuple[Subsystem],
        desiredRotation: Optional[Callable[[], Rotation2d]] = None,
    ) -> None:
        """
        Constructs a new SwerveControllerCommand that when executed will follow the
        provided trajectory. This command will not return output voltages but
        rather raw module states from the position controllers which need to be put
        into a velocity PID.

        Note: The controllers will *not* set the outputVolts to zero upon
        completion of the path- this is left to the user, since it is not
        appropriate for paths with nonstationary endstates.

        :param trajectory:         The trajectory to follow.
        :param pose:               A function that supplies the robot pose - use one of the odometry classes to
                                   provide this.
        :param kinematics:         The kinematics for the robot drivetrain. Can be kinematics for 2/3/4/6
                                   SwerveKinematics.
        :param controller:         The HolonomicDriveController for the drivetrain.
                                   If you have x, y, and theta controllers, pass them into
                                   HolonomicPIDController.
        :param outputModuleStates: The raw output module states from the position controllers.
        :param requirements:       The subsystems to require.
        :param desiredRotation:    (optional) The angle that the drivetrain should be
                                   facing. This is sampled at each time step. If not specified, that rotation of
                                   the final pose in the trajectory is used.
        """
        super().__init__()
        self._trajectory = trajectory
        self._pose = pose
        self._kinematics = kinematics
        self._outputModuleStates = outputModuleStates
        self._controller = controller
        if desiredRotation is None:
            self._desiredRotation = trajectory.states()[-1].pose.rotation
        else:
            self._desiredRotation = desiredRotation

        self._timer = Timer()
        self.addRequirements(*requirements)

    def initialize(self):
        self._timer.restart()

    def execute(self):
        curTime = self._timer.get()
        desiredState = self._trajectory.sample(curTime)

        targetChassisSpeeds = self._controller.calculate(
            self._pose(), desiredState, self._desiredRotation()
        )
        targetModuleStates = self._kinematics.toSwerveModuleStates(targetChassisSpeeds)

        self._outputModuleStates(targetModuleStates)

    def end(self, interrupted):
        self._timer.stop()

    def isFinished(self):
        return self._timer.hasElapsed(self._trajectory.totalTime())
