import pytest
import math
import numpy as np

from wpimath.geometry import Quaternion, Rotation3d


def test_init():
    # Identity
    q1 = Quaternion()
    assert q1.w == pytest.approx(1.0)
    assert q1.x == pytest.approx(0.0)
    assert q1.y == pytest.approx(0.0)
    assert q1.z == pytest.approx(0.0)

    # Normalized
    q2 = Quaternion(0.5, 0.5, 0.5, 0.5)
    assert q2.w == pytest.approx(0.5)
    assert q2.x == pytest.approx(0.5)
    assert q2.y == pytest.approx(0.5)
    assert q2.z == pytest.approx(0.5)

    # Unnormalized
    q3 = Quaternion(0.75, 0.3, 0.4, 0.5)
    assert q3.w == pytest.approx(0.75)
    assert q3.x == pytest.approx(0.3)
    assert q3.y == pytest.approx(0.4)
    assert q3.z == pytest.approx(0.5)

    q3 = q3.normalize()
    norm = math.sqrt(0.75 * 0.75 + 0.3 * 0.3 + 0.4 * 0.4 + 0.5 * 0.5)
    assert q3.w == pytest.approx(0.75 / norm)
    assert q3.x == pytest.approx(0.3 / norm)
    assert q3.y == pytest.approx(0.4 / norm)
    assert q3.z == pytest.approx(0.5 / norm)
    assert q3.w * q3.w + q3.x * q3.x + q3.y * q3.y + q3.z * q3.z == pytest.approx(1.0)


def test_addition():
    q = Quaternion(0.1, 0.2, 0.3, 0.4)
    p = Quaternion(0.5, 0.6, 0.7, 0.8)

    sum = q + p

    assert sum.w == pytest.approx(q.w + p.w)
    assert sum.x == pytest.approx(q.x + p.x)
    assert sum.y == pytest.approx(q.y + p.y)
    assert sum.z == pytest.approx(q.z + p.z)


def test_subtraction():
    q = Quaternion(0.1, 0.2, 0.3, 0.4)
    p = Quaternion(0.5, 0.6, 0.7, 0.8)

    difference = q - p

    assert difference.w == pytest.approx(q.w - p.w)
    assert difference.x == pytest.approx(q.x - p.x)
    assert difference.y == pytest.approx(q.y - p.y)
    assert difference.z == pytest.approx(q.z - p.z)


def test_scalar_multiplication():
    q = Quaternion(0.1, 0.2, 0.3, 0.4)
    scalar = 2

    product = q * scalar

    assert product.w == pytest.approx(q.w * scalar)
    assert product.x == pytest.approx(q.x * scalar)
    assert product.y == pytest.approx(q.y * scalar)
    assert product.z == pytest.approx(q.z * scalar)


def test_scalar_division():
    q = Quaternion(0.1, 0.2, 0.3, 0.4)
    scalar = 2

    product = q / scalar

    assert product.w == pytest.approx(q.w / scalar)
    assert product.x == pytest.approx(q.x / scalar)
    assert product.y == pytest.approx(q.y / scalar)
    assert product.z == pytest.approx(q.z / scalar)


def test_multiply():
    # 90° CCW rotations around each axis
    c = math.cos(math.radians(90) / 2.0)
    s = math.sin(math.radians(90) / 2.0)
    x_rot = Quaternion(c, s, 0.0, 0.0)
    y_rot = Quaternion(c, 0.0, s, 0.0)
    z_rot = Quaternion(c, 0.0, 0.0, s)

    # 90° CCW X rotation, 90° CCW Y rotation, and 90° CCW Z rotation should
    # produce a 90° CCW Y rotation
    expected = y_rot
    actual = z_rot * y_rot * x_rot
    assert actual.w == pytest.approx(expected.w, abs=1e-9)
    assert actual.x == pytest.approx(expected.x, abs=1e-9)
    assert actual.y == pytest.approx(expected.y, abs=1e-9)
    assert actual.z == pytest.approx(expected.z, abs=1e-9)

    # Identity
    q = Quaternion(0.72760687510899891, 0.29104275004359953, 0.38805700005813276,
                   0.48507125007266594)
    actual = q * q.inverse()
    assert actual.w == pytest.approx(1.0, abs=1e-9)
    assert actual.x == pytest.approx(0.0, abs=1e-9)
    assert actual.y == pytest.approx(0.0, abs=1e-9)
    assert actual.z == pytest.approx(0.0, abs=1e-9)


def test_conjugate():
    q = Quaternion(0.72760687510899891, 0.29104275004359953, 0.38805700005813276,
                   0.48507125007266594)
    conj = q.conjugate()

    assert conj.w == pytest.approx(q.w)
    assert conj.x == pytest.approx(-q.x)
    assert conj.y == pytest.approx(-q.y)
    assert conj.z == pytest.approx(-q.z)


def test_inverse():
    q = Quaternion(0.72760687510899891, 0.29104275004359953, 0.38805700005813276,
                   0.48507125007266594)
    norm = q.norm()

    inv = q.inverse()

    assert inv.w == pytest.approx(q.w / (norm * norm))
    assert inv.x == pytest.approx(-q.x / (norm * norm))
    assert inv.y == pytest.approx(-q.y / (norm * norm))
    assert inv.z == pytest.approx(-q.z / (norm * norm))


def test_norm():
    q = Quaternion(3, 4, 12, 84)
    norm = q.norm()

    assert norm == pytest.approx(85, abs=1e-9)


def test_exponential():
    q = Quaternion(1.1, 2.2, 3.3, 4.4)
    expect = Quaternion(2.81211398529184, -0.392521193481878, -0.588781790222817,
                        -0.785042386963756)

    q_exp = q.exp()

    assert q_exp == expect


def test_logarithm():
    q = Quaternion(1.1, 2.2, 3.3, 4.4)
    expect = Quaternion(1.7959088706354, 0.515190292664085, 0.772785438996128,
                        1.03038058532817)

    q_log = q.log()

    assert q_log == expect

    zero = Quaternion(0, 0, 0, 0)
    one = Quaternion(1, 0, 0, 0)
    i = Quaternion(0, 1, 0, 0)
    j = Quaternion(0, 0, 1, 0)
    k = Quaternion(0, 0, 0, 1)
    ln_half = Quaternion(math.log(0.5), -math.pi, 0, 0)

    assert one.log() == zero
    assert i.log() == Quaternion(0, math.pi / 2, 0, 0)
    assert j.log() == Quaternion(0, 0, math.pi / 2, 0)
    assert k.log() == Quaternion(0, 0, 0, math.pi / 2)

    assert (one * -1).log() == Quaternion(0, -math.pi, 0, 0)
    assert (one * -0.5).log() == ln_half


def test_logarithm_and_exponential_inverse():
    q = Quaternion(1.1, 2.2, 3.3, 4.4)

    # These operations are order-dependent: ln(exp(q)) is congruent but not
    # necessarily equal to exp(ln(q)) due to the multi-valued nature of the
    # complex logarithm.

    q_log_exp = q.log().exp()

    assert q_log_exp == q

    start = Quaternion(1, 2, 3, 4)
    expect = Quaternion(5, 6, 7, 8)

    twist = start.log(expect)
    actual = start.exp(twist)

    assert actual == expect


def test_dot_product():
    q = Quaternion(1.1, 2.2, 3.3, 4.4)
    p = Quaternion(5.5, 6.6, 7.7, 8.8)

    assert q.dot(p) == pytest.approx(q.w * p.w + q.x * p.x + q.y * p.y + q.z * p.z, abs=1e-9)


def test_dot_product_as_equality():
    q = Quaternion(1.1, 2.2, 3.3, 4.4)
    q_conj = q.conjugate()

    assert q.dot(q) == pytest.approx(q.norm() * q.norm(), abs=1e-9)
    assert abs(q.dot(q_conj) - q.norm() * q_conj.norm()) > 1e-9