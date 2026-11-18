import pytest
import math
import numpy as np

from wpimath import DCMotorBiasEstimator


def test_dc_motor_bias_estimator_constant_bias():
    ks = 0.5
    kg = 1.0
    kv = 1.0
    ka = 0.1
    dt = 0.005  # s
    duration = 2  # s

    # Continuous DC motor model
    A_c = -kv / ka
    B_c = 1 / ka

    # Discrete DC motor model
    A_d = math.exp(A_c * dt)
    B_d = 1 / A_c * (A_d - 1) * B_c

    estimator = DCMotorBiasEstimator(ks, kg, kv, ka, 0.05, dt)
    v = 0.0
    u_bias_estimate = 0.0

    estimator.reset(v)

    def simulate(u, u_bias):
        nonlocal v, u_bias_estimate
        for _ in np.arange(0, duration, dt):
            u_bias_estimate = estimator.calculate(v, u)
            v = A_d * v + B_d * (u - ks * np.sign(v) - kg + u_bias)

    # Positive bias
    simulate(5.0, 2.0)
    assert v == pytest.approx((5.0 - ks - kg + 2.0) / kv, abs=1e-6)
    assert u_bias_estimate == pytest.approx(2.0, abs=1e-6)

    # Negative bias
    simulate(5.0, -2.0)
    assert v == pytest.approx((5.0 - ks - kg - 2.0) / kv, abs=1e-6)
    assert u_bias_estimate == pytest.approx(-2.0, abs=1e-6)

    # Zero bias
    simulate(5.0, 0.0)
    assert v == pytest.approx((5.0 - ks - kg) / kv, abs=1e-6)
    assert u_bias_estimate == pytest.approx(0.0, abs=1e-6)

    # Reverse direction with positive bias
    simulate(-5.0, 1.0)
    assert v == pytest.approx((-5.0 + ks - kg + 1.0) / kv, abs=1e-6)
    assert u_bias_estimate == pytest.approx(1.0, abs=1e-6)
