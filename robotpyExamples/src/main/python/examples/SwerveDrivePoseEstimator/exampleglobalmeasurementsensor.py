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
    def getEstimatedGlobalPose(estimatedRobotPose: wpimath.Pose2d) -> wpimath.Pose2d:
        """Get a "noisy" fake global pose reading.

        :param estimatedRobotPose: The robot pose.
        """
        rand_x = random.gauss(0.0, 0.5)
        rand_y = random.gauss(0.0, 0.5)
        rand_rot = random.gauss(0.0, wpimath.units.degreesToRadians(30))
        return wpimath.Pose2d(
            estimatedRobotPose.x + rand_x,
            estimatedRobotPose.y + rand_y,
            estimatedRobotPose.rotation().rotateBy(wpimath.Rotation2d(rand_rot)),
        )
