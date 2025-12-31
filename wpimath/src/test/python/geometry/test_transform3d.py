import pytest
import math
import numpy as np

from wpimath import Pose3d, Rotation3d, Transform3d


def test_to_matrix():
    before = Transform3d(
        x=1,
        y=2,
        z=3,
        rotation=Rotation3d(math.radians(10), math.radians(20), math.radians(30)),
    )
    after = Transform3d.fromMatrix(before.toMatrix())

    assert before == after


def test_inverse():
    z_axis = np.array([0.0, 0.0, 1.0])

    initial = Pose3d(x=1, y=2, z=3, rotation=Rotation3d(z_axis, math.radians(45)))
    transform = Transform3d(x=5, y=4, z=3, rotation=Rotation3d(z_axis, math.radians(5)))

    transformed = initial + transform
    untransformed = transformed + transform.inverse()

    assert initial == untransformed


def test_composition():
    z_axis = np.array([0.0, 0.0, 1.0])

    initial = Pose3d(x=1, y=2, z=3, rotation=Rotation3d(z_axis, math.radians(45)))
    transform1 = Transform3d(
        x=5, y=0, z=0, rotation=Rotation3d(z_axis, math.radians(5))
    )
    transform2 = Transform3d(
        x=0, y=2, z=0, rotation=Rotation3d(z_axis, math.radians(5))
    )

    transformed_separate = initial + transform1 + transform2
    transformed_combined = initial + (transform1 + transform2)

    assert transformed_separate == transformed_combined
