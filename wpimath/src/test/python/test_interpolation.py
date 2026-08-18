import math

from wpimath import (
    Pose2d,
    Rotation2d,
    TimeInterpolatableFloatBuffer,
    TimeInterpolatablePose2dBuffer,
    TimeInterpolatableRotation2dBuffer,
)


def test_float():
    buffer = TimeInterpolatableFloatBuffer(10)

    buffer.add_sample(0, 0)
    assert buffer.sample(0) == 0
    buffer.add_sample(1, 1)
    assert buffer.sample(0.5) == 0.5
    assert buffer.sample(1) == 1
    buffer.add_sample(3, 2)
    assert buffer.sample(2) == 1.5

    buffer.add_sample(10.5, 2)
    assert buffer.sample(0) == 1


def test_rotation2d():
    buffer = TimeInterpolatableRotation2dBuffer(10)

    buffer.add_sample(0, Rotation2d(0))
    assert buffer.sample(0) == Rotation2d(0)
    buffer.add_sample(1, Rotation2d(1))
    assert buffer.sample(0.5) == Rotation2d(0.5)
    assert buffer.sample(1) == Rotation2d(1)
    buffer.add_sample(3, Rotation2d(2))
    assert buffer.sample(2) == Rotation2d(1.5)

    buffer.add_sample(10.5, Rotation2d(2))
    assert buffer.sample(0) == Rotation2d(1)


def test_pose2d():
    buffer = TimeInterpolatablePose2dBuffer(10)
    # We expect to be at (1 - 1/sqrt(2), 1/sqrt(2), 45deg) at t=0.5
    buffer.add_sample(0, Pose2d(0, 0, Rotation2d.from_degrees(90)))
    buffer.add_sample(1, Pose2d(1, 1, Rotation2d(0)))
    sample = buffer.sample(0.5)
    assert sample is not None
    assert math.isclose(sample.x, 1 - 1 / 2**0.5)
    assert math.isclose(sample.y, 1 / 2**0.5)
    assert math.isclose(sample.rotation().degrees(), 45)
