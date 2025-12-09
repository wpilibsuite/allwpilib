import pytest
import math
import numpy as np

from wpimath.geometry import Rotation3d, Rotation2d, Quaternion
from wpimath.units import radians


def test_gimbal_lock_accuracy():
    rot1 = Rotation3d(0, 0, radians(math.pi / 2))
    rot2 = Rotation3d(radians(math.pi), 0, 0)
    rot3 = Rotation3d(radians(-math.pi / 2), 0, 0)
    result1 = rot1 + rot2 + rot3
    expected1 = Rotation3d(0, -radians(math.pi / 2), radians(math.pi / 2))
    assert expected1 == result1
    assert (result1.x + result1.z) == pytest.approx(math.pi / 2)
    assert result1.y == pytest.approx(-math.pi / 2, abs=1e-7)

    rot1 = Rotation3d(0, 0, radians(math.pi / 2))
    rot2 = Rotation3d(radians(-math.pi), 0, 0)
    rot3 = Rotation3d(radians(math.pi / 2), 0, 0)
    result2 = rot1 + rot2 + rot3
    expected2 = Rotation3d(0, radians(math.pi / 2), radians(math.pi / 2))
    assert expected2 == result2
    assert (result2.z - result2.x) == pytest.approx(math.pi / 2)
    assert result2.y == pytest.approx(math.pi / 2, abs=1e-7)

    rot1 = Rotation3d(0, 0, radians(math.pi / 2))
    rot2 = Rotation3d(0, radians(math.pi / 3), 0)
    rot3 = Rotation3d(radians(-math.pi / 2), 0, 0)
    result3 = rot1 + rot2 + rot3
    expected3 = Rotation3d(0, radians(math.pi / 2), radians(math.pi / 6))
    assert expected3 == result3
    assert (result3.z - result3.x) == pytest.approx(math.pi / 6)
    assert result3.y == pytest.approx(math.pi / 2)


def test_init_axis_angle_and_roll_pitch_yaw():
    x_axis = np.array([1.0, 0.0, 0.0])
    rot1 = Rotation3d(x_axis, radians(math.pi / 3))
    rot2 = Rotation3d(radians(math.pi / 3), 0, 0)
    rvec1 = Rotation3d(x_axis * (math.pi / 3))
    assert rot1 == rot2
    assert rot1 == rvec1

    y_axis = np.array([0.0, 1.0, 0.0])
    rot3 = Rotation3d(y_axis, radians(math.pi / 3))
    rot4 = Rotation3d(0, radians(math.pi / 3), 0)
    rvec2 = Rotation3d(y_axis * (math.pi / 3))
    assert rot3 == rot4
    assert rot3 == rvec2

    z_axis = np.array([0.0, 0.0, 1.0])
    rot5 = Rotation3d(z_axis, radians(math.pi / 3))
    rot6 = Rotation3d(0, 0, radians(math.pi / 3))
    rvec3 = Rotation3d(z_axis * (math.pi / 3))
    assert rot5 == rot6
    assert rot5 == rvec3


def test_init_rotation_matrix():
    # No rotation
    R1 = np.identity(3)
    rot1 = Rotation3d(R1)
    assert Rotation3d() == rot1

    # 90 degree CCW rotation around z-axis
    R2 = np.zeros((3, 3))
    R2[:, 0] = [0.0, 1.0, 0.0]
    R2[:, 1] = [-1.0, 0.0, 0.0]
    R2[:, 2] = [0.0, 0.0, 1.0]
    rot2 = Rotation3d(R2)
    expected2 = Rotation3d(math.radians(0), math.radians(0), math.radians(90))
    assert expected2 == rot2

    # Matrix that isn't orthogonal
    R3 = np.array([[1.0, 0.0, 0.0], [1.0, 0.0, 0.0], [1.0, 0.0, 0.0]])
    with pytest.raises(ValueError):
        Rotation3d(R3)

    # Matrix that's orthogonal but not special orthogonal
    R4 = np.identity(3) * 2.0
    with pytest.raises(ValueError):
        Rotation3d(R4)


def test_init_two_vector():
    x_axis = np.array([1.0, 0.0, 0.0])
    y_axis = np.array([0.0, 1.0, 0.0])
    z_axis = np.array([0.0, 0.0, 1.0])

    # 90 degree CW rotation around y-axis
    rot1 = Rotation3d(x_axis, z_axis)
    expected1 = Rotation3d(y_axis, radians(-math.pi / 2.0))
    assert expected1 == rot1

    # 45 degree CCW rotation around z-axis
    rot2 = Rotation3d(x_axis, np.array([1.0, 1.0, 0.0]))
    expected2 = Rotation3d(z_axis, radians(math.pi / 4.0))
    assert expected2 == rot2

    # 0 degree rotation of x-axes
    rot3 = Rotation3d(x_axis, x_axis)
    assert Rotation3d() == rot3

    # 0 degree rotation of y-axes
    rot4 = Rotation3d(y_axis, y_axis)
    assert Rotation3d() == rot4

    # 0 degree rotation of z-axes
    rot5 = Rotation3d(z_axis, z_axis)
    assert Rotation3d() == rot5

    # 180 degree rotation tests. For 180 degree rotations, any quaternion with an
    # orthogonal rotation axis is acceptable. The rotation axis and initial
    # vector are orthogonal if their dot product is zero.

    # 180 degree rotation of x-axes
    rot6 = Rotation3d(x_axis, -x_axis)
    q6 = rot6.getQuaternion()
    assert q6.w == pytest.approx(0.0)
    assert q6.x * x_axis[0] + q6.y * x_axis[1] + q6.z * x_axis[2] == pytest.approx(0.0)

    # 180 degree rotation of y-axes
    rot7 = Rotation3d(y_axis, -y_axis)
    q7 = rot7.getQuaternion()
    assert q7.w == pytest.approx(0.0)
    assert q7.x * y_axis[0] + q7.y * y_axis[1] + q7.z * y_axis[2] == pytest.approx(0.0)

    # 180 degree rotation of z-axes
    rot8 = Rotation3d(z_axis, -z_axis)
    q8 = rot8.getQuaternion()
    assert q8.w == pytest.approx(0.0)
    assert q8.x * z_axis[0] + q8.y * z_axis[1] + q8.z * z_axis[2] == pytest.approx(0.0)


def test_radians_to_degrees():
    z_axis = np.array([0.0, 0.0, 1.0])

    rot1 = Rotation3d(z_axis, radians(math.pi / 3))
    assert rot1.x == pytest.approx(0.0)
    assert rot1.y == pytest.approx(0.0)
    assert rot1.z == pytest.approx(math.radians(60))

    rot2 = Rotation3d(z_axis, radians(math.pi / 4))
    assert rot2.x == pytest.approx(0.0)
    assert rot2.y == pytest.approx(0.0)
    assert rot2.z == pytest.approx(math.radians(45))


def test_degrees_to_radians():
    z_axis = np.array([0.0, 0.0, 1.0])

    rot1 = Rotation3d(z_axis, math.radians(45))
    assert rot1.x == pytest.approx(0.0)
    assert rot1.y == pytest.approx(0.0)
    assert rot1.z == pytest.approx(math.pi / 4.0)

    rot2 = Rotation3d(z_axis, math.radians(30))
    assert rot2.x == pytest.approx(0.0)
    assert rot2.y == pytest.approx(0.0)
    assert rot2.z == pytest.approx(math.pi / 6.0)


def test_rotation_loop():
    rot = Rotation3d()

    rot = rot + Rotation3d(math.radians(90), 0, 0)
    expected = Rotation3d(math.radians(90), 0, 0)
    assert expected == rot

    rot = rot + Rotation3d(0, math.radians(90), 0)
    expected = Rotation3d(
        np.array([1.0 / math.sqrt(3), 1.0 / math.sqrt(3), -1.0 / math.sqrt(3)]),
        math.radians(120),
    )
    assert expected == rot

    rot = rot + Rotation3d(0, 0, math.radians(90))
    expected = Rotation3d(0, math.radians(90), 0)
    assert expected == rot

    rot = rot + Rotation3d(0, math.radians(-90), 0)
    assert Rotation3d() == rot


def test_rotate_by_from_zero_x():
    x_axis = np.array([1.0, 0.0, 0.0])

    zero = Rotation3d()
    rotated = zero + Rotation3d(x_axis, math.radians(90))

    expected = Rotation3d(x_axis, math.radians(90))
    assert expected == rotated


def test_rotate_by_from_zero_y():
    y_axis = np.array([0.0, 1.0, 0.0])

    zero = Rotation3d()
    rotated = zero + Rotation3d(y_axis, math.radians(90))

    expected = Rotation3d(y_axis, math.radians(90))
    assert expected == rotated


def test_rotate_by_from_zero_z():
    z_axis = np.array([0.0, 0.0, 1.0])

    zero = Rotation3d()
    rotated = zero + Rotation3d(z_axis, math.radians(90))

    expected = Rotation3d(z_axis, math.radians(90))
    assert expected == rotated


def test_rotate_by_non_zero_x():
    x_axis = np.array([1.0, 0.0, 0.0])

    rot = Rotation3d(x_axis, math.radians(90))
    rot = rot + Rotation3d(x_axis, math.radians(30))

    expected = Rotation3d(x_axis, math.radians(120))
    assert expected == rot


def test_rotate_by_non_zero_y():
    y_axis = np.array([0.0, 1.0, 0.0])

    rot = Rotation3d(y_axis, math.radians(90))
    rot = rot + Rotation3d(y_axis, math.radians(30))

    expected = Rotation3d(y_axis, math.radians(120))
    assert expected == rot


def test_rotate_by_non_zero_z():
    z_axis = np.array([0.0, 0.0, 1.0])

    rot = Rotation3d(z_axis, math.radians(90))
    rot = rot + Rotation3d(z_axis, math.radians(30))

    expected = Rotation3d(z_axis, math.radians(120))
    assert expected == rot


def test_minus():
    z_axis = np.array([0.0, 0.0, 1.0])

    rot1 = Rotation3d(z_axis, math.radians(70))
    rot2 = Rotation3d(z_axis, math.radians(30))

    assert math.degrees((rot1 - rot2).z) == pytest.approx(40.0)


def test_axis_angle():
    x_axis = np.array([1.0, 0.0, 0.0])
    y_axis = np.array([0.0, 1.0, 0.0])
    z_axis = np.array([0.0, 0.0, 1.0])

    rot1 = Rotation3d(x_axis, math.radians(90))
    assert np.allclose(x_axis, rot1.axis())
    assert rot1.angle == pytest.approx(math.pi / 2.0)

    rot2 = Rotation3d(y_axis, math.radians(45))
    assert np.allclose(y_axis, rot2.axis())
    assert rot2.angle == pytest.approx(math.pi / 4.0)

    rot3 = Rotation3d(z_axis, math.radians(60))
    assert np.allclose(z_axis, rot3.axis())
    assert rot3.angle == pytest.approx(math.pi / 3.0)


def test_to_rotation2d():
    rotation = Rotation3d(math.radians(20), math.radians(30), math.radians(40))
    expected = Rotation2d(math.radians(40))

    assert expected == rotation.toRotation2d()


def test_equality():
    z_axis = np.array([0.0, 0.0, 1.0])

    rot1 = Rotation3d(z_axis, math.radians(43))
    rot2 = Rotation3d(z_axis, math.radians(43))
    assert rot1 == rot2

    rot3 = Rotation3d(z_axis, math.radians(-180))
    rot4 = Rotation3d(z_axis, math.radians(180))
    assert rot3 == rot4


def test_inequality():
    z_axis = np.array([0.0, 0.0, 1.0])

    rot1 = Rotation3d(z_axis, math.radians(43))
    rot2 = Rotation3d(z_axis, math.radians(43.5))
    assert rot1 != rot2


def test_to_matrix():
    before = Rotation3d(math.radians(10), math.radians(20), math.radians(30))
    after = Rotation3d(before.toMatrix())

    assert before == after
