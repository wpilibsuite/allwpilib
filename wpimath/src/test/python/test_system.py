import math

import wpimath

import pytest
import numpy as np


def test_rk4_exponential():
    """Test that integrating dx/dt = eˣ works"""
    y0 = np.array([[0.0]])

    y1 = wpimath.RK4(lambda x: np.array([[math.exp(x[0, 0])]]), y0, 0.1)

    assert math.isclose(y1[0, 0], math.exp(0.1) - math.exp(0.0), abs_tol=1e-3)


def test_rk4_exponential_with_u():
    """Test that integrating dx/dt = eˣ works when we provide a u"""
    y0 = np.array([[0.0]])

    y1 = wpimath.RK4(
        lambda x, u: np.array([[math.exp(u[0, 0] * x[0, 0])]]),
        y0,
        np.array([[1.0]]),
        0.1,
    )

    assert math.isclose(y1[0, 0], math.exp(0.1) - math.exp(0.0), abs_tol=1e-3)


def test_rk4_time_varying():
    """
    Tests RK4 with a time varying solution. From
    http://www2.hawaii.edu/~jmcfatri/math407/RungeKuttaTest.html:

        dx/dt = x (2 / (eᵗ + 1) - 1)

    The true (analytical) solution is:

        x(t) = 12eᵗ/(eᵗ + 1)²
    """
    y0 = np.array([[12.0 * math.exp(5.0) / math.pow(math.exp(5.0) + 1.0, 2.0)]])

    y1 = wpimath.RK4(
        lambda t, x: np.array([[x[0, 0] * (2.0 / (math.exp(t) + 1.0) - 1.0)]]),
        5.0,
        y0,
        1.0,
    )

    expected = 12.0 * math.exp(6.0) / math.pow(math.exp(6.0) + 1.0, 2.0)
    assert math.isclose(y1[0, 0], expected, abs_tol=1e-3)


def test_rkdp_zero():
    """Tests that integrating dx/dt = 0 works with RKDP"""
    y1 = wpimath.RKDP(
        lambda x, u: np.zeros((1, 1)),
        np.array([[0.0]]),
        np.array([[0.0]]),
        0.1,
    )

    assert math.isclose(y1[0, 0], 0.0, abs_tol=1e-3)


def test_rkdp_exponential():
    """Tests that integrating dx/dt = eˣ works with RKDP"""
    y0 = np.array([[0.0]])

    y1 = wpimath.RKDP(
        lambda x, u: np.array([[math.exp(x[0, 0])]]),
        y0,
        np.array([[0.0]]),
        0.1,
    )

    assert math.isclose(y1[0, 0], math.exp(0.1) - math.exp(0.0), abs_tol=1e-3)


def test_rkdp_time_varying():
    """
    Tests RKDP with a time varying solution. From
    http://www2.hawaii.edu/~jmcfatri/math407/RungeKuttaTest.html:

        dx/dt = x(2/(eᵗ + 1) - 1)

    The true (analytical) solution is:

        x(t) = 12eᵗ/(eᵗ + 1)²
    """
    y0 = np.array([[12.0 * math.exp(5.0) / math.pow(math.exp(5.0) + 1.0, 2.0)]])

    y1 = wpimath.RKDP(
        lambda t, x: np.array([[x[0, 0] * (2.0 / (math.exp(t) + 1.0) - 1.0)]]),
        5.0,
        y0,
        1.0,
        1e-12,
    )

    expected = 12.0 * math.exp(6.0) / math.pow(math.exp(6.0) + 1.0, 2.0)
    assert math.isclose(y1[0, 0], expected, abs_tol=1e-3)


def test_numerical_jacobian():
    """Test that we can recover A from ax_fn() pretty accurately"""
    a = np.array(
        [
            [1.0, 2.0, 4.0, 1.0],
            [5.0, 2.0, 3.0, 4.0],
            [5.0, 1.0, 3.0, 2.0],
            [1.0, 1.0, 3.0, 7.0],
        ]
    )

    def ax_fn(x):
        return a @ x

    new_a = wpimath.numericalJacobian(ax_fn, np.zeros((4, 1)))
    np.testing.assert_allclose(new_a, a, rtol=1e-6, atol=1e-5)


def test_numerical_jacobian_x_u_square():
    """Test that we can recover B from axbu_fn() pretty accurately"""
    a = np.array(
        [
            [1.0, 2.0, 4.0, 1.0],
            [5.0, 2.0, 3.0, 4.0],
            [5.0, 1.0, 3.0, 2.0],
            [1.0, 1.0, 3.0, 7.0],
        ]
    )
    b = np.array([[1.0, 1.0], [2.0, 1.0], [3.0, 2.0], [3.0, 7.0]])

    def axbu_fn(x, u):
        return a @ x + b @ u

    x0 = np.zeros((4, 1))
    u0 = np.zeros((2, 1))
    new_a = wpimath.numericalJacobianX(axbu_fn, x0, u0)
    new_b = wpimath.numericalJacobianU(axbu_fn, x0, u0)
    np.testing.assert_allclose(new_a, a, rtol=1e-6, atol=1e-5)
    np.testing.assert_allclose(new_b, b, rtol=1e-6, atol=1e-5)


def test_numerical_jacobian_x_u_rectangular():
    c = np.array(
        [
            [1.0, 2.0, 4.0, 1.0],
            [5.0, 2.0, 3.0, 4.0],
            [5.0, 1.0, 3.0, 2.0],
        ]
    )
    d = np.array([[1.0, 1.0], [2.0, 1.0], [3.0, 2.0]])

    def cxdu_fn(x, u):
        return c @ x + d @ u

    x0 = np.zeros((4, 1))
    u0 = np.zeros((2, 1))
    new_c = wpimath.numericalJacobianX(cxdu_fn, x0, u0)
    new_d = wpimath.numericalJacobianU(cxdu_fn, x0, u0)
    np.testing.assert_allclose(new_c, c, rtol=1e-6, atol=1e-5)
    np.testing.assert_allclose(new_d, d, rtol=1e-6, atol=1e-5)


def test_numerical_jacobian_x_passes_extra_args():
    a = np.array([[2.0, -1.0], [0.5, 3.0]])
    b = np.array([[1.0], [4.0]])
    x0 = np.zeros((2, 1))
    u0 = np.zeros((1, 1))

    seen = {}

    def axbu_fn(x, u, scale, bias):
        seen["args"] = (scale, bias)
        return scale * (a @ x) + bias * (b @ u)

    new_a = wpimath.numericalJacobianX(axbu_fn, x0, u0, 2.5, -3.0)

    assert seen["args"] == (2.5, -3.0)
    np.testing.assert_allclose(new_a, 2.5 * a, rtol=1e-6, atol=1e-5)


def test_numerical_jacobian_u_passes_extra_args():
    a = np.array([[1.0, 0.0], [0.0, -2.0]])
    b = np.array([[1.5], [-0.5]])
    x0 = np.zeros((2, 1))
    u0 = np.zeros((1, 1))

    seen = {}

    def axbu_fn(x, u, scale, bias):
        seen["args"] = (scale, bias)
        return scale * (a @ x) + bias * (b @ u)

    new_b = wpimath.numericalJacobianU(axbu_fn, x0, u0, 4.0, 0.25)

    assert seen["args"] == (4.0, 0.25)
    np.testing.assert_allclose(new_b, 0.25 * b, rtol=1e-6, atol=1e-5)
