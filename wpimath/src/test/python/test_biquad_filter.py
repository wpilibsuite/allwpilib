"""Tests for wpimath.BiquadFilter."""

import math
import random

import pytest

from wpimath import BiquadFilter


def test_pass_through():
    filter = BiquadFilter([BiquadFilter.Section(1.0, 0.0, 0.0, 0.0, 0.0)])
    rng = random.Random(42)
    for _ in range(200):
        x = rng.uniform(-100.0, 100.0)
        assert filter.calculate(x) == x


def test_butterworth_4th_order_low_pass():
    # scipy.signal.butter(4, 50.0, btype='low', fs=1000.0, output='sos')
    filter = BiquadFilter(
        [
            BiquadFilter.Section(
                0.00041659920440659937,
                0.0008331984088131987,
                0.00041659920440659937,
                -1.4796742169311934,
                0.5558215432824889,
            ),
            BiquadFilter.Section(
                1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979
            ),
        ]
    )
    expected = [
        0.00041659920440659937,
        0.0029914483065925663,
        0.010405740533503665,
        0.024092655231875183,
        0.04300386328531425,
        0.06442081415630327,
        0.08518000836484753,
        0.10245740377665029,
        0.1142030744642985,
        0.11931076610150239,
        0.11759868177262096,
        0.10966797135549569,
        0.09669445862739445,
        0.08019770689053446,
        0.06182021082200691,
        0.04313888252371942,
        0.025521549440937964,
        0.01003324447867498,
        -0.002609160074363505,
        -0.01203989315971688,
        -0.018213508615082776,
        -0.021350392922805498,
        -0.02186860712506684,
        -0.020311212598085788,
        -0.01727668431352673,
        -0.013358111475758645,
        -0.009094876704322833,
        -0.004938595712161598,
        -0.0012334395430879353,
        0.0017903545884787877,
    ]
    for i, e in enumerate(expected):
        x = 1.0 if i == 0 else 0.0
        y = filter.calculate(x)
        assert math.isclose(y, e, rel_tol=0, abs_tol=1e-10), f"sample {i}"


def test_notch_60hz():
    # scipy.signal.iirnotch(60, Q=10, fs=1000) via tf2sos
    filter = BiquadFilter(
        [
            BiquadFilter.Section(
                0.9814970254751076,
                -1.8251457105120343,
                0.9814970254751076,
                -1.8251457105120341,
                0.9629940509502151,
            )
        ]
    )
    fs = 1000.0
    samples = 1000
    output = []
    for n in range(samples):
        t = n / fs
        x = math.sin(2.0 * math.pi * 10.0 * t) + math.sin(2.0 * math.pi * 60.0 * t)
        output.append(filter.calculate(x))

    assert math.isclose(output[500], -0.017355123579818322, abs_tol=1e-10)
    assert math.isclose(output[999], -0.08007594066581347, abs_tol=1e-10)


def test_order_8_butterworth_matches_scipy():
    # scipy.signal.butter(8, 100.0, btype='low', fs=1000.0, output='sos')
    filter = BiquadFilter(
        [
            BiquadFilter.Section(
                2.3959644103776166e-05,
                4.791928820755233e-05,
                2.3959644103776166e-05,
                -1.0263514742610553,
                0.26864019099379005,
            ),
            BiquadFilter.Section(
                1.0, 2.0, 1.0, -1.0868584613628944, 0.343430940165366
            ),
            BiquadFilter.Section(
                1.0, 2.0, 1.0, -1.2197253651240232, 0.5076634651740437
            ),
            BiquadFilter.Section(
                1.0, 2.0, 1.0, -1.4515795942478362, 0.794251053241888
            ),
        ]
    )
    N = 500
    fs = 1000.0
    f0 = 1.0
    f1 = 200.0
    t1 = (N - 1) / fs
    k = (f1 - f0) / t1

    spot_idx = [10, 50, 100, 250, 499]
    expected = [
        0.8950675041062186,
        -0.7902247252134351,
        0.1716891991372734,
        0.05240058121316523,
        -0.0016952227415119995,
    ]
    got = []
    j = 0
    for n in range(N):
        t = n / fs
        phase = 2.0 * math.pi * (f0 * t + 0.5 * k * t * t)
        x = math.cos(phase)
        y = filter.calculate(x)
        if j < len(spot_idx) and n == spot_idx[j]:
            got.append(y)
            j += 1

    for i, (g, e) in enumerate(zip(got, expected)):
        assert math.isclose(g, e, abs_tol=1e-10), f"index {spot_idx[i]}"


def test_reset_zeros_state():
    filter = BiquadFilter(
        [
            BiquadFilter.Section(
                0.00041659920440659937,
                0.0008331984088131987,
                0.00041659920440659937,
                -1.4796742169311934,
                0.5558215432824889,
            ),
            BiquadFilter.Section(
                1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979
            ),
        ]
    )
    for _ in range(50):
        filter.calculate(1.0)
    assert filter.lastValue() != 0.0

    filter.reset()
    assert filter.lastValue() == 0.0
    y = filter.calculate(1.0)
    assert math.isclose(y, 0.00041659920440659937, abs_tol=1e-12)


def test_reset_to_steady_state():
    filter = BiquadFilter(
        [
            BiquadFilter.Section(
                0.00041659920440659937,
                0.0008331984088131987,
                0.00041659920440659937,
                -1.4796742169311934,
                0.5558215432824889,
            ),
            BiquadFilter.Section(
                1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979
            ),
        ]
    )
    input_val = 3.0
    filter.reset(input_val)

    assert math.isclose(filter.lastValue(), input_val, abs_tol=1e-12)
    assert math.isclose(filter.calculate(input_val), input_val, abs_tol=1e-12)
    for _ in range(20):
        assert math.isclose(filter.calculate(input_val), input_val, abs_tol=1e-12)


def test_num_sections():
    filter = BiquadFilter(
        [
            BiquadFilter.Section(1.0, 0.0, 0.0, 0.0, 0.0),
            BiquadFilter.Section(1.0, 0.0, 0.0, 0.0, 0.0),
            BiquadFilter.Section(1.0, 0.0, 0.0, 0.0, 0.0),
        ]
    )
    assert filter.numSections() == 3


def test_section_repr():
    s = BiquadFilter.Section(1.0, 2.0, 3.0, 4.0, 5.0)
    assert "b0" in repr(s)
    assert "1.0" in repr(s)


def test_empty_cascade_throws():
    with pytest.raises(RuntimeError):
        BiquadFilter([])
