import pytest
import math
import numpy as np

from wpimath.geometry import Rotation2d, Transform2d, Translation2d, Pose2d


def test_to_matrix():
    before = Transform2d(x=1, y=2, rotation=Rotation2d.fromDegrees(20))
    after = Transform2d.fromMatrix(before.toMatrix())
    assert before == after


def test_inverse():
    initial = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(45))
    transform = Transform2d(Translation2d(x=5, y=0), Rotation2d.fromDegrees(5))

    transformed = initial + transform
    untransformed = transformed + transform.inverse()
    assert initial == untransformed


def test_composition():
    initial = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(45))
    transform1 = Transform2d(
        Translation2d(x=5, y=0), rotation=Rotation2d.fromDegrees(5)
    )
    transform2 = Transform2d(
        Translation2d(x=0, y=2), rotation=Rotation2d.fromDegrees(5)
    )

    transformed_separate = initial + transform1 + transform2
    transformed_combined = initial + (transform1 + transform2)

    assert transformed_separate == transformed_combined
