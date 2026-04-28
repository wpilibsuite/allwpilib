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


# ---------- Design factories ------------------------------------------------


def _expect_section_near(got, want, tol):
    assert got.b0 == pytest.approx(want.b0, abs=tol)
    assert got.b1 == pytest.approx(want.b1, abs=tol)
    assert got.b2 == pytest.approx(want.b2, abs=tol)
    assert got.a1 == pytest.approx(want.a1, abs=tol)
    assert got.a2 == pytest.approx(want.a2, abs=tol)


def _cascade_magnitude(sections, f, fs):
    import cmath

    w = 2.0 * math.pi * f / fs
    z1 = cmath.exp(-1j * w)
    z2 = cmath.exp(-2j * w)
    h = 1.0 + 0j
    for s in sections:
        num = s.b0 + s.b1 * z1 + s.b2 * z2
        den = 1.0 + s.a1 * z1 + s.a2 * z2
        h *= num / den
    return abs(h)


def test_butterworth_factory_matches_scipy():
    # scipy.signal.butter(4, 50.0, btype='low', fs=1000.0, output='sos')
    f = BiquadFilter.butterworth(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0)
    sections = list(f.sections())
    assert len(sections) == 2
    _expect_section_near(
        sections[0],
        BiquadFilter.Section(
            0.00041659920440659937,
            0.0008331984088131987,
            0.00041659920440659937,
            -1.4796742169311934,
            0.5558215432824889,
        ),
        1e-10,
    )
    _expect_section_near(
        sections[1],
        BiquadFilter.Section(
            1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979
        ),
        1e-10,
    )


def test_butterworth_bandpass_factory_matches_scipy():
    # scipy.signal.butter(4, [80.0, 120.0], btype='bandpass', fs=1000.0)
    f = BiquadFilter.butterworth(BiquadFilter.Kind.BandPass, 4, 1000.0, 80.0, 120.0)
    sections = list(f.sections())
    assert len(sections) == 4
    _expect_section_near(
        sections[0],
        BiquadFilter.Section(
            0.0001832160233696091,
            0.0003664320467392182,
            0.0001832160233696091,
            -1.395944592254935,
            0.7785762494967928,
        ),
        1e-10,
    )
    _expect_section_near(
        sections[1],
        BiquadFilter.Section(1.0, 2.0, 1.0, -1.5194742571654707, 0.8044610397041421),
        1e-10,
    )
    _expect_section_near(
        sections[2],
        BiquadFilter.Section(1.0, -2.0, 1.0, -1.395095159020637, 0.8950130915917338),
        1e-10,
    )
    _expect_section_near(
        sections[3],
        BiquadFilter.Section(1.0, -2.0, 1.0, -1.678184355447092, 0.9231164780821922),
        1e-10,
    )


def test_butterworth_factory_cutoff_at_minus_three_db():
    f = BiquadFilter.butterworth(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0)
    sections = list(f.sections())
    assert _cascade_magnitude(sections, 0.0, 1000.0) == pytest.approx(1.0, abs=1e-10)
    db_at_fc = 20.0 * math.log10(_cascade_magnitude(sections, 50.0, 1000.0))
    assert db_at_fc == pytest.approx(-3.0, abs=0.05)


def test_chebyshev1_factory_matches_scipy():
    # scipy.signal.cheby1(4, 1.0, 50.0, btype='low', fs=1000.0, output='sos')
    f = BiquadFilter.chebyshevI(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, 1.0)
    sections = list(f.sections())
    assert len(sections) == 2
    _expect_section_near(
        sections[0],
        BiquadFilter.Section(
            0.00012984963538691335,
            0.0002596992707738267,
            0.00012984963538691335,
            -1.7831991339963722,
            0.8083720161261031,
        ),
        1e-10,
    )


def test_chebyshev2_factory_matches_scipy():
    # scipy.signal.cheby2(4, 40.0, 50.0, btype='low', fs=1000.0, output='sos')
    f = BiquadFilter.chebyshevII(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, 40.0)
    sections = list(f.sections())
    assert len(sections) == 2
    _expect_section_near(
        sections[0],
        BiquadFilter.Section(
            0.009735570656077937,
            -0.01377605024474192,
            0.009735570656077937,
            -1.6993957730842835,
            0.7262535657383176,
        ),
        1e-10,
    )


def test_elliptic_factory_matches_scipy():
    # scipy.signal.ellip(4, 1.0, 40.0, 50.0, btype='low', fs=1000.0)
    f = BiquadFilter.elliptic(
        BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, 1.0, 40.0
    )
    sections = list(f.sections())
    assert len(sections) == 2
    _expect_section_near(
        sections[0],
        BiquadFilter.Section(
            0.011738158079014929,
            -0.01231742214386518,
            0.011738158079014929,
            -1.7624726990429698,
            0.7947551993829407,
        ),
        1e-10,
    )


def test_elliptic_bandpass_factory_matches_scipy():
    # scipy.signal.ellip(4, 1.0, 40.0, [80.0, 120.0], btype='bandpass', fs=1000.0)
    f = BiquadFilter.elliptic(
        BiquadFilter.Kind.BandPass, 4, 1000.0, 80.0, 120.0, 1.0, 40.0
    )
    sections = list(f.sections())
    assert len(sections) == 4
    _expect_section_near(
        sections[0],
        BiquadFilter.Section(
            0.010903156756394984,
            -0.008920205787636758,
            0.010903156756394982,
            -1.4809043488404827,
            0.9052184223450329,
        ),
        1e-10,
    )
    _expect_section_near(
        sections[1],
        BiquadFilter.Section(
            1.0,
            -1.9038045463534676,
            0.9999999999999999,
            -1.62699499510272,
            0.9194678402475894,
        ),
        1e-10,
    )
    _expect_section_near(
        sections[2],
        BiquadFilter.Section(
            1.0, -1.3265553048553793, 1.0, -1.4370735618061194, 0.9697500844409095
        ),
        1e-10,
    )
    _expect_section_near(
        sections[3],
        BiquadFilter.Section(
            1.0,
            -1.8057300347135379,
            0.9999999999999998,
            -1.733243724674222,
            0.978571861817194,
        ),
        1e-10,
    )


def test_notch_factory_matches_scipy():
    # scipy.signal.iirnotch(60.0, Q=10.0, fs=1000.0)
    f = BiquadFilter.notch(1000.0, 60.0, 10.0)
    sections = list(f.sections())
    assert len(sections) == 1
    _expect_section_near(
        sections[0],
        BiquadFilter.Section(
            0.9814970254751076,
            -1.8251457105120343,
            0.9814970254751076,
            -1.8251457105120341,
            0.9629940509502151,
        ),
        1e-12,
    )


def test_moving_average_factory_dc_gain():
    f = BiquadFilter.movingAverage(4)
    sections = list(f.sections())
    assert _cascade_magnitude(sections, 0.0, 1000.0) == pytest.approx(1.0, abs=1e-12)
    assert _cascade_magnitude(sections, 500.0, 1000.0) < 1e-12


def test_factory_invalid_args_throw():
    with pytest.raises((ValueError, RuntimeError)):
        BiquadFilter.butterworth(BiquadFilter.Kind.LowPass, 0, 1000.0, 50.0)
    with pytest.raises((ValueError, RuntimeError)):
        BiquadFilter.notch(1000.0, 60.0, 0.0)
    with pytest.raises((ValueError, RuntimeError)):
        BiquadFilter.movingAverage(0)
