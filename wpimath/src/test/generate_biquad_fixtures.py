#!/usr/bin/env python3
"""Reproducibility script for the BiquadFilter test golden values.

Run this script (requires scipy + numpy) to print every scipy-derived
constant that's hard-coded into the BiquadFilter test suites:

  wpimath/src/test/native/cpp/filter/BiquadFilter*Test.cpp
  wpimath/src/test/java/org/wpilib/math/filter/BiquadFilter*Test.java
  wpimath/src/test/python/test_biquad_filter.py

The hard-coded test literals were generated against the scipy version
printed at the top of this script's output. If scipy ever changes the
underlying algorithm or default precision behavior the literals may need
to be regenerated. A lower-bound check is enforced below to catch the
known-incompatible historical versions.
"""

import math
import sys

import numpy as np
import scipy
import scipy.signal as sig

_MIN_SCIPY = (1, 11)
_scipy_version = tuple(int(p) for p in scipy.__version__.split(".")[:2])
if _scipy_version < _MIN_SCIPY:
    sys.exit(
        f"scipy >= {'.'.join(str(p) for p in _MIN_SCIPY)} required "
        f"(found {scipy.__version__})"
    )

print(f"# scipy {scipy.__version__}, numpy {np.__version__}")
print()


def _format_section(row):
    """scipy SOS row is [b0, b1, b2, 1.0, a1, a2]; tests store (b0,b1,b2,a1,a2)."""
    b0, b1, b2, _, a1, a2 = (float(v) for v in row)
    return f"({b0!r}, {b1!r}, {b2!r}, {a1!r}, {a2!r})"


def _print_sos(label, sos):
    print(f"# {label}")
    for i, row in enumerate(sos):
        print(f"  sections[{i}] = {_format_section(row)}")
    print()


# ---------- Butterworth -----------------------------------------------------

# Used by:
#   BiquadFilterTest.Butterworth4thOrderLowPass   (cpp / java)
#   BiquadFilterTest.ResetZerosState              (cpp / java)
#   BiquadFilterTest.ResetToSteadyState           (cpp / java)
#   BiquadFilterTest.DCGainConverges              (cpp / java)
#   BiquadFilterDesignTest.ButterworthLowPass4thOrderMatchesScipy
#   test_biquad_filter.test_butterworth_4th_order_low_pass
#   test_biquad_filter.test_butterworth_factory_matches_scipy
#   test_biquad_filter.test_reset_zeros_state / test_reset_to_steady_state
sos_butter4_lp = sig.butter(4, 50.0, btype="low", fs=1000.0, output="sos")
_print_sos(
    "scipy.signal.butter(4, 50.0, btype='low', fs=1000.0, output='sos')", sos_butter4_lp
)

# Impulse response, first 30 samples, used in:
#   BiquadFilterTest.Butterworth4thOrderLowPass (cpp / java / python)
print("# Butterworth4thOrderLowPass impulse response, first 30 samples")
print("# (sosfilt of a unit impulse — matches inline values exactly)")
x = np.zeros(30)
x[0] = 1.0
y = sig.sosfilt(sos_butter4_lp, x)
for i, v in enumerate(y):
    print(f"  y[{i:2d}] = {float(v)!r}")
print()

# Used by:
#   BiquadFilterTest.Order8ButterworthMatchesScipy        (cpp / java)
#   BiquadFilterDesignTest.ButterworthLowPass8thOrderMatchesScipy
#   test_biquad_filter.test_order_8_butterworth_matches_scipy
sos_butter8_lp = sig.butter(8, 100.0, btype="low", fs=1000.0, output="sos")
_print_sos(
    "scipy.signal.butter(8, 100.0, btype='low', fs=1000.0, output='sos')",
    sos_butter8_lp,
)

# Used by:
#   BiquadFilterDesignTest.ButterworthBandPass4thOrderMatchesScipy (cpp / java)
#   test_biquad_filter.test_butterworth_bandpass_factory_matches_scipy
sos_butter4_bp = sig.butter(4, [80.0, 120.0], btype="bandpass", fs=1000.0, output="sos")
_print_sos(
    "scipy.signal.butter(4, [80.0, 120.0], btype='bandpass', fs=1000.0, "
    "output='sos')",
    sos_butter4_bp,
)

# Chirp 1→200 Hz over 500 samples at 1 kHz, spot samples for the order-8
# filter.
print("# Order8ButterworthMatchesScipy chirp spot samples")
print("# (cos chirp 1→200 Hz, N=500, fs=1000, then sosfilt)")
N = 500
fs = 1000.0
f0 = 1.0
f1 = 200.0
t1 = (N - 1) / fs
k = (f1 - f0) / t1
n = np.arange(N)
t = n / fs
phase = 2.0 * np.pi * (f0 * t + 0.5 * k * t * t)
x = np.cos(phase)
y = sig.sosfilt(sos_butter8_lp, x)
for idx in [10, 50, 100, 250, 499]:
    print(f"  y[{idx:3d}] = {float(y[idx])!r}")
print()


# ---------- Notch -----------------------------------------------------------

# Used by:
#   BiquadFilterTest.Notch60Hz                  (cpp / java)
#   BiquadFilterDesignTest.Notch60HzMatchesScipy
#   test_biquad_filter.test_notch_60hz / test_notch_factory_matches_scipy
b, a = sig.iirnotch(60.0, Q=10.0, fs=1000.0)
sos_notch = sig.tf2sos(b, a)
_print_sos("scipy.signal.iirnotch(60.0, Q=10.0, fs=1000.0) via tf2sos", sos_notch)

# Notch time-series spot samples. NOTE: ULP-drift from inline values.
print("# Notch60Hz time-series spot samples")
print("# (sin(2π·10·t) + sin(2π·60·t), N=1000, fs=1000, then sosfilt)")
N = 1000
fs = 1000.0
n = np.arange(N)
t = n / fs
x = np.sin(2.0 * np.pi * 10.0 * t) + np.sin(2.0 * np.pi * 60.0 * t)
y = sig.sosfilt(sos_notch, x)
print(f"  y[500] = {float(y[500])!r}")
print(f"  y[999] = {float(y[999])!r}")
print()


# ---------- Chebyshev type I ------------------------------------------------

# Used by:
#   BiquadFilterChebyshevTest.Cheby1LowPass4thOrderMatchesScipy (cpp / java)
#   test_biquad_filter.test_chebyshev1_factory_matches_scipy
sos_cheby1_lp = sig.cheby1(4, 1.0, 50.0, btype="low", fs=1000.0, output="sos")
_print_sos(
    "scipy.signal.cheby1(4, 1.0, 50.0, btype='low', fs=1000.0, output='sos')",
    sos_cheby1_lp,
)

# Used by:
#   BiquadFilterChebyshevTest.Cheby1HighPass4thOrderMatchesScipy (cpp / java)
sos_cheby1_hp = sig.cheby1(4, 1.0, 100.0, btype="high", fs=1000.0, output="sos")
_print_sos(
    "scipy.signal.cheby1(4, 1.0, 100.0, btype='high', fs=1000.0, output='sos')",
    sos_cheby1_hp,
)

# Used by:
#   BiquadFilterChebyshevTest.Cheby1BandPass4thOrderMatchesScipy (cpp / java)
sos_cheby1_bp = sig.cheby1(
    4, 1.0, [80.0, 120.0], btype="bandpass", fs=1000.0, output="sos"
)
_print_sos(
    "scipy.signal.cheby1(4, 1.0, [80.0, 120.0], btype='bandpass', "
    "fs=1000.0, output='sos')",
    sos_cheby1_bp,
)


# ---------- Chebyshev type II -----------------------------------------------

# Used by:
#   BiquadFilterChebyshevTest.Cheby2LowPass4thOrderMatchesScipy (cpp / java)
#   test_biquad_filter.test_chebyshev2_factory_matches_scipy
sos_cheby2_lp = sig.cheby2(4, 40.0, 50.0, btype="low", fs=1000.0, output="sos")
_print_sos(
    "scipy.signal.cheby2(4, 40.0, 50.0, btype='low', fs=1000.0, output='sos')",
    sos_cheby2_lp,
)


# ---------- Elliptic --------------------------------------------------------

# Used by:
#   BiquadFilterEllipticTest.LowPass4thOrderMatchesScipy (cpp / java)
#   test_biquad_filter.test_elliptic_factory_matches_scipy
sos_ellip_lp = sig.ellip(4, 1.0, 40.0, 50.0, btype="low", fs=1000.0, output="sos")
_print_sos(
    "scipy.signal.ellip(4, 1.0, 40.0, 50.0, btype='low', fs=1000.0, output='sos')",
    sos_ellip_lp,
)

# Used by:
#   BiquadFilterEllipticTest.BandPass4thOrderMatchesScipy (cpp / java)
#   test_biquad_filter.test_elliptic_bandpass_factory_matches_scipy
sos_ellip_bp = sig.ellip(
    4, 1.0, 40.0, [80.0, 120.0], btype="bandpass", fs=1000.0, output="sos"
)
_print_sos(
    "scipy.signal.ellip(4, 1.0, 40.0, [80.0, 120.0], btype='bandpass', "
    "fs=1000.0, output='sos')",
    sos_ellip_bp,
)


# ---------- First-order-as-biquad cross-check -------------------------------

# Used by:
#   BiquadFilterTest.FirstOrderMatchesSinglePoleIIR (cpp / java)
# Not a scipy value — just the analytic conversion of SinglePoleIIR's pole
# coefficient. Documented here so the conversion is in one place.
print("# FirstOrderMatchesSinglePoleIIR: SinglePoleIIR(T=0.015915, dt=0.005)")
print("#   y[n] = (1-g) x[n] + g y[n-1],  g = exp(-dt/T)")
print("# Equivalent biquad section: (1-g, 0, 0, -g, 0)")
T = 0.015915
dt = 0.005
g = math.exp(-dt / T)
print(f"  g = {g!r}")
print(f"  section = ({1.0 - g!r}, 0.0, 0.0, {-g!r}, 0.0)")
