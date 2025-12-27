import pytest
import math

from wpimath.geometry import Pose2d, Rotation2d, Translation2d, Transform2d
from wpimath.units import feetToMeters


def test_rotate_by():
    x = 1
    y = 2
    initial = Pose2d(x, y, math.radians(45))

    rotation = Rotation2d(math.radians(5))
    rotated = initial.rotateBy(rotation)

    # Translation is rotated by CCW rotation matrix
    c = rotation.cos()
    s = rotation.sin()
    assert rotated.x == pytest.approx(c * x - s * y)
    assert rotated.y == pytest.approx(s * x + c * y)
    assert rotated.rotation().degrees() == pytest.approx(
        initial.rotation().degrees() + rotation.degrees()
    )


def test_transform_by():
    initial = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(45))
    transform = Transform2d(Translation2d(x=5, y=0), Rotation2d.fromDegrees(5))

    transformed = initial + transform

    assert transformed.x == pytest.approx(1.0 + 5.0 / math.sqrt(2.0))
    assert transformed.y == pytest.approx(2.0 + 5.0 / math.sqrt(2.0))
    assert transformed.rotation().degrees() == pytest.approx(50.0)


def test_relative_to():
    initial = Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(45))
    final = Pose2d(x=5, y=5, rotation=Rotation2d.fromDegrees(45.0))

    final_relative_to_initial = final.relativeTo(initial)

    assert final_relative_to_initial.x == pytest.approx(5.0 * math.sqrt(2.0), abs=1e-9)
    assert final_relative_to_initial.y == pytest.approx(0.0, abs=1e-9)
    assert final_relative_to_initial.rotation().degrees() == pytest.approx(
        0.0, abs=1e-9
    )


def test_rotate_around():
    initial = Pose2d(x=5, y=0, rotation=Rotation2d.fromDegrees(0))
    point = Translation2d(x=0, y=0)

    rotated = initial.rotateAround(point, Rotation2d.fromDegrees(180))

    assert rotated.x == pytest.approx(-5.0, abs=1e-9)
    assert rotated.y == pytest.approx(0.0, abs=1e-9)
    assert rotated.rotation().degrees() == pytest.approx(180.0, abs=1e-9)


def test_equality():
    a = Pose2d(x=0, y=5, rotation=Rotation2d.fromDegrees(43))
    b = Pose2d(x=0, y=5, rotation=Rotation2d.fromDegrees(43))
    assert a == b


def test_inequality():
    a = Pose2d(x=0, y=5, rotation=Rotation2d.fromDegrees(43))
    b = Pose2d(x=0, y=feetToMeters(5), rotation=Rotation2d.fromDegrees(43))
    assert a != b


def test_minus():
    initial = Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(45))
    final = Pose2d(x=5, y=5, rotation=Rotation2d.fromDegrees(45))

    transform = final - initial

    assert transform.x == pytest.approx(5.0 * math.sqrt(2.0), abs=1e-9)
    assert transform.y == pytest.approx(0.0, abs=1e-9)
    assert transform.rotation().degrees() == pytest.approx(0.0, abs=1e-9)


def test_nearest():
    origin = Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(0))

    pose1 = Pose2d(
        Translation2d(distance=1.0, angle=Rotation2d.fromDegrees((45))),
        Rotation2d.fromDegrees(0),
    )
    pose2 = Pose2d(
        Translation2d(distance=2.0, angle=Rotation2d.fromDegrees((90))),
        Rotation2d.fromDegrees(0),
    )
    pose3 = Pose2d(
        Translation2d(distance=3.0, angle=Rotation2d.fromDegrees((135))),
        Rotation2d.fromDegrees(0),
    )
    pose4 = Pose2d(
        Translation2d(distance=4.0, angle=Rotation2d.fromDegrees((180))),
        Rotation2d.fromDegrees(0),
    )
    pose5 = Pose2d(
        Translation2d(distance=5.0, angle=Rotation2d.fromDegrees((270))),
        Rotation2d.fromDegrees(0),
    )

    assert origin.nearest([pose5, pose3, pose4]).x == pytest.approx(pose3.x)
    assert origin.nearest([pose5, pose3, pose4]).y == pytest.approx(pose3.y)

    assert origin.nearest([pose1, pose2, pose3]).x == pytest.approx(pose1.x)
    assert origin.nearest([pose1, pose2, pose3]).y == pytest.approx(pose1.y)

    assert origin.nearest([pose4, pose2, pose3]).x == pytest.approx(pose2.x)
    assert origin.nearest([pose4, pose2, pose3]).y == pytest.approx(pose2.y)

    # Rotation component sort (when distance is the same)
    # Use the same translation because using different angles at the same
    # distance can cause rounding error.
    translation = Translation2d(distance=1.0, angle=Rotation2d.fromDegrees(0))

    pose_a = Pose2d(translation, Rotation2d.fromDegrees(0))
    pose_b = Pose2d(translation, Rotation2d.fromDegrees(30))
    pose_c = Pose2d(translation, Rotation2d.fromDegrees(120))
    pose_d = Pose2d(translation, Rotation2d.fromDegrees(90))
    pose_e = Pose2d(translation, Rotation2d.fromDegrees(-180))

    assert Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(360)).nearest(
        [pose_a, pose_b, pose_d]
    ).rotation().degrees() == pytest.approx(pose_a.rotation().degrees())
    assert Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(-335)).nearest(
        [pose_b, pose_c, pose_d]
    ).rotation().degrees() == pytest.approx(pose_b.rotation().degrees())
    assert Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(-120)).nearest(
        [pose_b, pose_c, pose_d]
    ).rotation().degrees() == pytest.approx(pose_c.rotation().degrees())
    assert Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(85)).nearest(
        [pose_a, pose_c, pose_d]
    ).rotation().degrees() == pytest.approx(pose_d.rotation().degrees())
    assert Pose2d(x=0, y=0, rotation=Rotation2d.fromDegrees(170)).nearest(
        [pose_a, pose_d, pose_e]
    ).rotation().degrees() == pytest.approx(pose_e.rotation().degrees())


def test_to_matrix():
    before = Pose2d(x=1, y=2, rotation=Rotation2d.fromDegrees(20))
    after = Pose2d.fromMatrix(before.toMatrix())

    assert before == after
