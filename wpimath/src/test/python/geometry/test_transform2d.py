import pytest
import math
import numpy as np

from wpimath.geometry import (
    Rotation2d,
    Transform2d,
    Translation2d,
    Pose2d
)
from wpimath.units import meters


def test_to_matrix():
    before = Transform2d(x=1, y=2, rotation=Rotation2d.fromDegrees(20))
    after = Transform2d.fromMatrix(before.toMatrix())
    assert before == after


def test_inverse():
    initial = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(45))
    transform = Transform2d(Translation2d(meters(5), meters(0)), Rotation2d.fromDegrees(5))

    transformed = initial + transform
    untransformed = transformed + transform.inverse()
    assert initial == untransformed


def test_composition():
    initial = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(45))
    transform1 = Transform2d(Translation2d(x=5, y=0), rotation=Rotation2d.fromDegrees(5))
    transform2 = Transform2d(Translation2d(x=0, y=2), rotation=Rotation2d.fromDegrees(5))

    transformed_separate = initial + transform1 + transform2
    transformed_combined = initial + (transform1 + transform2)

    assert transformed_separate == transformed_combined


def test_constexpr():
    default_ctor = Transform2d()
    translation_rotation_ctor = Transform2d(Translation2d(), Rotation2d.fromDegrees(10))
    multiplied = translation_rotation_ctor * 5
    divided = translation_rotation_ctor / 2

    assert default_ctor.x == pytest.approx(0.0)
    assert translation_rotation_ctor.x == pytest.approx(0.0)
    assert translation_rotation_ctor.y == pytest.approx(0.0)
    assert multiplied.rotation().degrees() == pytest.approx(50.0)
    assert translation_rotation_ctor.inverse().rotation().degrees() == pytest.approx(-10.0)
    assert translation_rotation_ctor.inverse().x == pytest.approx(0.0)
    assert translation_rotation_ctor.inverse().y == pytest.approx(0.0)
    assert divided.rotation().degrees() == pytest.approx(5.0)