# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

from typing import TYPE_CHECKING, List, Tuple
import math

from wpilib import Timer

from wpimath.geometry import Pose2d, Rotation2d, Translation2d
from wpimath.kinematics import (
    SwerveModuleState,
    SwerveModulePosition,
    SwerveDrive4Kinematics,
    SwerveDrive4Odometry,
)
from wpimath.controller import (
    ProfiledPIDControllerRadians,
    PIDController,
    HolonomicDriveController,
)
from wpimath.trajectory import (
    TrapezoidProfileRadians,
    Trajectory,
    TrajectoryConfig,
    TrajectoryGenerator,
)


from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *

import pytest

import commands2


def test_swervecontrollercommand():
    timer = Timer()
    angle = Rotation2d(0)

    swerve_module_states = (
        SwerveModuleState(0, Rotation2d(0)),
        SwerveModuleState(0, Rotation2d(0)),
        SwerveModuleState(0, Rotation2d(0)),
        SwerveModuleState(0, Rotation2d(0)),
    )

    swerve_module_positions = (
        SwerveModulePosition(0, Rotation2d(0)),
        SwerveModulePosition(0, Rotation2d(0)),
        SwerveModulePosition(0, Rotation2d(0)),
        SwerveModulePosition(0, Rotation2d(0)),
    )

    rot_controller = ProfiledPIDControllerRadians(
        1,
        0,
        0,
        TrapezoidProfileRadians.Constraints(3 * math.pi, math.pi),
    )

    x_tolerance = 1 / 12.0
    y_tolerance = 1 / 12.0
    angular_tolerance = 1 / 12.0

    wheel_base = 0.5
    track_width = 0.5

    kinematics = SwerveDrive4Kinematics(
        Translation2d(wheel_base / 2, track_width / 2),
        Translation2d(wheel_base / 2, -track_width / 2),
        Translation2d(-wheel_base / 2, track_width / 2),
        Translation2d(-wheel_base / 2, -track_width / 2),
    )

    odometry = SwerveDrive4Odometry(
        kinematics,
        Rotation2d(0),
        swerve_module_positions,
        Pose2d(0, 0, Rotation2d(0)),
    )

    def set_module_states(states):
        nonlocal swerve_module_states
        swerve_module_states = states

    def get_robot_pose() -> Pose2d:
        odometry.update(angle, swerve_module_positions)
        return odometry.getPose()

    with ManualSimTime() as sim:
        subsystem = commands2.Subsystem()
        waypoints: List[Pose2d] = []
        waypoints.append(Pose2d(0, 0, Rotation2d(0)))
        waypoints.append(Pose2d(1, 5, Rotation2d(3)))
        config = TrajectoryConfig(8.8, 0.1)
        trajectory = TrajectoryGenerator.generateTrajectory(waypoints, config)

        end_state = trajectory.sample(trajectory.totalTime())

        command = commands2.SwerveControllerCommand(
            trajectory=trajectory,
            pose=get_robot_pose,
            kinematics=kinematics,
            controller=HolonomicDriveController(
                PIDController(0.6, 0, 0),
                PIDController(0.6, 0, 0),
                rot_controller,
            ),
            outputModuleStates=set_module_states,
            requirements=(subsystem,),
        )

        timer.restart()

        command.initialize()
        while not command.isFinished():
            command.execute()
            angle = trajectory.sample(timer.get()).pose.rotation()

            for i in range(0, len(swerve_module_positions)):
                swerve_module_positions[i].distance += (
                    swerve_module_states[i].speed * 0.005
                )
                swerve_module_positions[i].angle = swerve_module_states[i].angle

            sim.step(0.005)

        timer.stop()
        command.end(True)

        assert end_state.pose.X() == pytest.approx(get_robot_pose().X(), x_tolerance)

        assert end_state.pose.Y() == pytest.approx(get_robot_pose().Y(), y_tolerance)

        assert end_state.pose.rotation().radians() == pytest.approx(
            get_robot_pose().rotation().radians(),
            angular_tolerance,
        )
