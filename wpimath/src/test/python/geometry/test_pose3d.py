import pytest
import math
import numpy as np

from wpimath.geometry import (
    Pose2d,
    Pose3d,
    Rotation2d,
    Rotation3d,
    Transform2d,
    Transform3d,
    Translation2d,
    Translation3d,
    Quaternion,
)
from wpimath.units import meters, feetToMeters


def test_rotate_by():
    x = meters(1)
    y = meters(2)
    initial = Pose3d(x, y, meters(0), Rotation3d.fromDegrees(0, 0, 45))

    yaw = math.radians(5)
    rotation = Rotation3d.fromDegrees(0, 0, math.degrees(yaw))
    rotated = initial.rotateBy(rotation)

    # Translation is rotated by CCW rotation matrix
    c = math.cos(yaw)
    s = math.sin(yaw)
    assert rotated.x == pytest.approx(c * x - s * y)
    assert rotated.y == pytest.approx(s * x + c * y)
    assert rotated.z == pytest.approx(0.0)
    assert rotated.rotation().x == pytest.approx(0.0)
    assert rotated.rotation().y == pytest.approx(0.0)
    assert rotated.rotation().z == pytest.approx(
        initial.rotation().z + rotation.z
    )


def test_test_transform_by_rotations():
    k_epsilon = 1e-9

    initial_pose = Pose3d(meters(0), meters(0), meters(0), Rotation3d())
    transform1 = Transform3d(Translation3d(), Rotation3d.fromDegrees(90, 45, 0))
    transform2 = Transform3d(Translation3d(), Rotation3d.fromDegrees(-90, 0, 0))
    transform3 = Transform3d(Translation3d(), Rotation3d.fromDegrees(0, -45, 0))

    final_pose = initial_pose.transformBy(transform1) \
        .transformBy(transform2) \
        .transformBy(transform3)

    assert final_pose.rotation().x == pytest.approx(
        initial_pose.rotation().x, abs=k_epsilon
    )
    assert final_pose.rotation().y == pytest.approx(
        initial_pose.rotation().y, abs=k_epsilon
    )
    assert final_pose.rotation().z == pytest.approx(
        initial_pose.rotation().z, abs=k_epsilon
    )


def test_transform_by():
    z_axis = np.array([0.0, 0.0, 1.0])

    initial = Pose3d(meters(1), meters(2), meters(0), Rotation3d(z_axis, math.radians(45.0)))
    transform = Transform3d(Translation3d(meters(5), meters(0), meters(0)), Rotation3d(z_axis, math.radians(5)))

    transformed = initial + transform

    assert transformed.x == pytest.approx(1.0 + 5.0 / math.sqrt(2.0))
    assert transformed.y == pytest.approx(2.0 + 5.0 / math.sqrt(2.0))
    assert transformed.rotation().z == pytest.approx(math.radians(50))


def test_relative_to():
    z_axis = np.array([0.0, 0.0, 1.0])

    initial = Pose3d(meters(0), meters(0), meters(0), Rotation3d(z_axis, math.radians(45)))
    final = Pose3d(meters(5), meters(5), meters(0), Rotation3d(z_axis, math.radians(45)))

    final_relative_to_initial = final.relativeTo(initial)

    assert final_relative_to_initial.x == pytest.approx(5.0 * math.sqrt(2.0))
    assert final_relative_to_initial.y == pytest.approx(0.0)
    assert final_relative_to_initial.rotation().z == pytest.approx(0.0, abs=1e-9)


def test_rotate_around():
    initial = Pose3d(meters(5), meters(0), meters(0), Rotation3d())
    point = Translation3d(meters(0), meters(0), meters(0))

    rotated = initial.rotateAround(point, Rotation3d.fromDegrees(0, 0, 180))

    assert rotated.x == pytest.approx(-5.0, abs=1e-9)
    assert rotated.y == pytest.approx(0.0, abs=1e-9)
    assert rotated.rotation().z == pytest.approx(math.radians(180), abs=1e-9)


def test_equality():
    z_axis = np.array([0.0, 0.0, 1.0])

    a = Pose3d(meters(0), meters(5), meters(0), Rotation3d(z_axis, math.radians(43)))
    b = Pose3d(meters(0), meters(5), meters(0), Rotation3d(z_axis, math.radians(43)))
    assert a == b


def test_inequality():
    z_axis = np.array([0.0, 0.0, 1.0])

    a = Pose3d(meters(0), meters(5), meters(0), Rotation3d(z_axis, math.radians(43)))
    b = Pose3d(meters(0), feetToMeters(5), meters(0), Rotation3d(z_axis, math.radians(43)))
    assert a != b


def test_minus():
    z_axis = np.array([0.0, 0.0, 1.0])

    initial = Pose3d(meters(0), meters(0), meters(0), Rotation3d(z_axis, math.radians(45)))
    final = Pose3d(meters(5), meters(5), meters(0), Rotation3d(z_axis, math.radians(45)))

    transform = final - initial

    assert transform.x == pytest.approx(5.0 * math.sqrt(2.0))
    assert transform.y == pytest.approx(0.0)
    assert transform.rotation().z == pytest.approx(0.0, abs=1e-9)


def test_to_matrix():
    before = Pose3d(meters(1), meters(2), meters(3), Rotation3d.fromDegrees(10, 20, 30))
    after = Pose3d.fromMatrix(before.toMatrix())

    assert before == after


def test_to_pose2d():
    pose = Pose3d(meters(1), meters(2), meters(3), Rotation3d.fromDegrees(20, 30, 40))
    expected = Pose2d(meters(1), meters(2), math.radians(40))

    assert expected == pose.toPose2d()


def test_complex_twists():
    initial_poses = [
        Pose3d(
            meters(0.698303),
            meters(-0.959096),
            meters(0.271076),
            Rotation3d(Quaternion(0.86403, -0.076866, 0.147234, 0.475254)),
        ),
        Pose3d(
            meters(0.634892),
            meters(-0.765209),
            meters(0.117543),
            Rotation3d(Quaternion(0.84987, -0.070829, 0.162097, 0.496415)),
        ),
        Pose3d(
            meters(0.584827),
            meters(-0.590303),
            meters(-0.02557),
            Rotation3d(Quaternion(0.832743, -0.041991, 0.202188, 0.513708)),
        ),
        Pose3d(
            meters(0.505038),
            meters(-0.451479),
            meters(-0.112835),
            Rotation3d(Quaternion(0.816515, -0.002673, 0.226182, 0.531166)),
        ),
        Pose3d(
            meters(0.428178),
            meters(-0.329692),
            meters(-0.189707),
            Rotation3d(Quaternion(0.807886, 0.029298, 0.257788, 0.529157)),
        ),
    ]

    final_poses = [
        Pose3d(
            meters(-0.230448),
            meters(-0.511957),
            meters(0.198406),
            Rotation3d(Quaternion(0.753984, 0.347016, 0.409105, 0.379106)),
        ),
        Pose3d(
            meters(-0.088932),
            meters(-0.343253),
            meters(0.095018),
            Rotation3d(Quaternion(0.638738, 0.413016, 0.536281, 0.365833)),
        ),
        Pose3d(
            meters(-0.107908),
            meters(-0.317552),
            meters(0.133946),
            Rotation3d(Quaternion(0.653444, 0.417069, 0.465505, 0.427046)),
        ),
        Pose3d(
            meters(-0.123383),
            meters(-0.156411),
            meters(-0.047435),
            Rotation3d(Quaternion(0.652983, 0.40644, 0.431566, 0.47135)),
        ),
        Pose3d(
            meters(-0.084654),
            meters(-0.019305),
            meters(-0.030022),
            Rotation3d(Quaternion(0.620243, 0.429104, 0.479384, 0.44873)),
        ),
    ]

    for i in range(len(initial_poses)):
        start = initial_poses[i]
        end = final_poses[i]

        twist = (end - start).log()
        start_exp = start + twist.exp()

        eps = 1e-5

        assert start_exp.x == pytest.approx(end.x, abs=eps)
        assert start_exp.y == pytest.approx(end.y, abs=eps)
        assert start_exp.z == pytest.approx(end.z, abs=eps)
        assert start_exp.rotation().getQuaternion().w == pytest.approx(
            end.rotation().getQuaternion().w, abs=eps
        )
        assert start_exp.rotation().getQuaternion().x == pytest.approx(
            end.rotation().getQuaternion().x, abs=eps
        )
        assert start_exp.rotation().getQuaternion().y == pytest.approx(
            end.rotation().getQuaternion().y, abs=eps
        )
        assert start_exp.rotation().getQuaternion().z == pytest.approx(
            end.rotation().getQuaternion().z, abs=eps
        )


def test_twist_nan():
    initial_poses = [
        Pose3d(
            meters(6.32),
            meters(4.12),
            meters(0.00),
            Rotation3d(
                Quaternion(
                    -0.9999999999999999, 0.0, 0.0, 1.9208309264993548e-08
                )
            ),
        ),
        Pose3d(
            meters(3.75),
            meters(2.95),
            meters(0.00),
            Rotation3d(
                Quaternion(
                    0.9999999999999793, 0.0, 0.0, 2.0352360299846772e-07
                )
            ),
        ),
    ]

    final_poses = [
        Pose3d(
            meters(6.33),
            meters(4.15),
            meters(0.00),
            Rotation3d(
                Quaternion(
                    -0.9999999999999999, 0.0, 0.0, 2.416890209039172e-08
                )
            ),
        ),
        Pose3d(
            meters(3.66),
            meters(2.93),
            meters(0.00),
            Rotation3d(
                Quaternion(
                    0.9999999999999782, 0.0, 0.0, 2.0859477994905617e-07
                )
            ),
        ),
    ]

    for i in range(len(initial_poses)):
        start = initial_poses[i]
        end = final_poses[i]
        twist = (end - start).log()

        assert not math.isnan(twist.dx)
        assert not math.isnan(twist.dy)
        assert not math.isnan(twist.dz)
        assert not math.isnan(twist.rx)
        assert not math.isnan(twist.ry)
        assert not math.isnan(twist.rz)


def test_nearest():
    origin = Pose3d(meters(0), meters(0), meters(0), Rotation3d())

    # Distance sort
    # poses are in order of closest to farthest away from the origin at
    # various positions in 3D space.
    pose1 = Pose3d(meters(1), meters(0), meters(0), Rotation3d())
    pose2 = Pose3d(meters(0), meters(2), meters(0), Rotation3d())
    pose3 = Pose3d(meters(0), meters(0), meters(3), Rotation3d())
    pose4 = Pose3d(meters(2), meters(2), meters(2), Rotation3d())
    pose5 = Pose3d(meters(3), meters(3), meters(3), Rotation3d())

    assert origin.nearest([pose5, pose3, pose4]).x == pytest.approx(pose3.x)
    assert origin.nearest([pose5, pose3, pose4]).y == pytest.approx(pose3.y)
    assert origin.nearest([pose5, pose3, pose4]).z == pytest.approx(pose3.z)

    assert origin.nearest([pose1, pose2, pose3]).x == pytest.approx(pose1.x)
    assert origin.nearest([pose1, pose2, pose3]).y == pytest.approx(pose1.y)
    assert origin.nearest([pose1, pose2, pose3]).z == pytest.approx(pose1.z)

    assert origin.nearest([pose4, pose2, pose3]).x == pytest.approx(pose2.x)
    assert origin.nearest([pose4, pose2, pose3]).y == pytest.approx(pose2.y)
    assert origin.nearest([pose4, pose2, pose3]).z == pytest.approx(pose2.z)

    # Rotation component sort (when distance is the same)
    # Use the same translation to avoid distance differences
    translation = Translation3d(meters(1), meters(0), meters(0))

    pose_a = Pose3d(translation, Rotation3d())  # No rotation
    pose_b = Pose3d(translation, Rotation3d.fromDegrees(30, 0, 0))
    pose_c = Pose3d(translation, Rotation3d.fromDegrees(0, 45, 0))
    pose_d = Pose3d(translation, Rotation3d.fromDegrees(0, 0, 90))
    pose_e = Pose3d(translation, Rotation3d.fromDegrees(180, 0, 0))

    result1 = Pose3d(meters(0), meters(0), meters(0), Rotation3d()).nearest(
        [pose_a, pose_b, pose_d]
    )
    assert result1.rotation().x == pytest.approx(pose_a.rotation().x)
    assert result1.rotation().y == pytest.approx(pose_a.rotation().y)
    assert result1.rotation().z == pytest.approx(pose_a.rotation().z)

    result2 = Pose3d(meters(0), meters(0), meters(0), Rotation3d.fromDegrees(25, 0, 0)).nearest(
        [pose_b, pose_c, pose_d]
    )
    assert result2.rotation().x == pytest.approx(pose_b.rotation().x)
    assert result2.rotation().y == pytest.approx(pose_b.rotation().y)
    assert result2.rotation().z == pytest.approx(pose_b.rotation().z)

    result3 = Pose3d(meters(0), meters(0), meters(0), Rotation3d.fromDegrees(0, 50, 0)).nearest(
        [pose_b, pose_c, pose_d]
    )
    assert result3.rotation().x == pytest.approx(pose_c.rotation().x)
    assert result3.rotation().y == pytest.approx(pose_c.rotation().y)
    assert result3.rotation().z == pytest.approx(pose_c.rotation().z)

    result4 = Pose3d(meters(0), meters(0), meters(0), Rotation3d.fromDegrees(0, 0, 85)).nearest(
        [pose_a, pose_c, pose_d]
    )
    assert result4.rotation().x == pytest.approx(pose_d.rotation().x)
    assert result4.rotation().y == pytest.approx(pose_d.rotation().y)
    assert result4.rotation().z == pytest.approx(pose_d.rotation().z)

    result5 = Pose3d(meters(0), meters(0), meters(0), Rotation3d.fromDegrees(170, 0, 0)).nearest(
        [pose_a, pose_d, pose_e]
    )
    assert result5.rotation().x == pytest.approx(pose_e.rotation().x)
    assert result5.rotation().y == pytest.approx(pose_e.rotation().y)
    assert result5.rotation().z == pytest.approx(pose_e.rotation().z)

    # Test with complex 3D rotations (combining roll, pitch, yaw)
    complex_pose1 = Pose3d(translation, Rotation3d.fromDegrees(45, 30, 60))
    complex_pose2 = Pose3d(translation, Rotation3d.fromDegrees(90, 45, 90))
    complex_pose3 = Pose3d(translation, Rotation3d.fromDegrees(10, 15, 20))

    complex_result = Pose3d(meters(0), meters(0), meters(0), Rotation3d.fromDegrees(5, 10, 15)).nearest(
        [complex_pose1, complex_pose2, complex_pose3]
    )
    assert complex_result.rotation().x == pytest.approx(complex_pose3.rotation().x)
    assert complex_result.rotation().y == pytest.approx(complex_pose3.rotation().y)
    assert complex_result.rotation().z == pytest.approx(complex_pose3.rotation().z)