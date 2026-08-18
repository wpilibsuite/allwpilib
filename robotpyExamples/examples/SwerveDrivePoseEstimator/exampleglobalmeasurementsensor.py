#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import random

import wpimath


class ExampleGlobalMeasurementSensor:
    """This dummy class represents a global measurement sensor, such as a computer vision
    solution.
    """

    def __init__(self) -> None:
        raise RuntimeError("Utility class")

    @staticmethod
    def get_estimated_global_pose(
        estimated_robot_pose: wpimath.Pose2d,
    ) -> wpimath.Pose2d:
        """Get a "noisy" fake global pose reading.

        :param estimatedRobotPose: The robot pose.
        """
        rand_x = random.gauss(0.0, 0.5)
        rand_y = random.gauss(0.0, 0.5)
        rand_rot = random.gauss(0.0, wpimath.units.degrees_to_radians(30))
        return wpimath.Pose2d(
            estimated_robot_pose.x + rand_x,
            estimated_robot_pose.y + rand_y,
            estimated_robot_pose.rotation().rotate_by(wpimath.Rotation2d(rand_rot)),
        )
