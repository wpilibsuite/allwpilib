import pytest
import math
import numpy as np

from wpimath.geometry import Pose3d, Rotation3d, Transform3d
from wpimath.units import meters


def test_to_matrix():
    before = Transform3d(meters(1), meters(2), meters(3), Rotation3d(math.radians(10), math.radians(20), math.radians(30)))
    after = Transform3d.fromMatrix(before.toMatrix())

    assert before == after


def test_inverse():
    z_axis = np.array([0.0, 0.0, 1.0])

    initial = Pose3d(meters(1), meters(2), meters(3), Rotation3d(z_axis, math.radians(45)))
    transform = Transform3d(meters(5), meters(4), meters(3), Rotation3d(z_axis, math.radians(5)))

    transformed = initial + transform
    untransformed = transformed + transform.inverse()

    assert initial == untransformed


def test_composition():
    z_axis = np.array([0.0, 0.0, 1.0])

    initial = Pose3d(meters(1), meters(2), meters(3), Rotation3d(z_axis, math.radians(45)))
    transform1 = Transform3d(meters(5), meters(0), meters(0), Rotation3d(z_axis, math.radians(5)))
    transform2 = Transform3d(meters(0), meters(2), meters(0), Rotation3d(z_axis, math.radians(5)))

    transformed_separate = initial + transform1 + transform2
    transformed_combined = initial + (transform1 + transform2)

    assert transformed_separate == transformed_combined