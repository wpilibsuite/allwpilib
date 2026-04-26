// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string>
#include <vector>

#include "internal/AnalogPrototypes.hpp"
#include "internal/BilinearTransform.hpp"
#include "internal/Zpk.hpp"
#include "wpi/math/filter/BiquadFilter.hpp"

// Public design factories. Each classical-IIR factory (Butterworth,
// Chebyshev I/II, Elliptic) drives the same three-step pipeline that
// scipy.signal's iirfilter does:
//
//   1. AnalogPrototypes::*Prototype     — analog LP prototype, cutoff 1 rad/s
//   2. BilinearTransform::DesignFromAnalogLp:
//        a. Zpk::AnalogLpTo{Lp,Hp,Bp,Bs} — kind-specific frequency transform
//        b. BilinearTransform            — analog → digital at sample rate fs
//        c. Zpk::ZpkToSos                — pair conjugate roots into biquads
//
// SciPy reference for the whole pipeline:
//   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py
// (functions iirfilter, butter, cheby1, cheby2, ellip)
//
// Notch and MovingAverage are closed-form and do not go through the
// prototype/bilinear path. They are documented inline below.

namespace wpi::math {

namespace {

void ValidateClassicalArgs(BiquadFilter::Kind kind, int order, double fs,
                           double f1, double f2) {
  if (order < 1) {
    throw std::invalid_argument("BiquadFilter design: order must be >= 1.");
  }
  if (fs <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter design: sample rate must be positive.");
  }
  const double nyquist = 0.5 * fs;
  if (f1 <= 0.0 || f1 >= nyquist) {
    throw std::invalid_argument(
        "BiquadFilter design: f1 must lie in (0, fs/2).");
  }
  if (kind == BiquadFilter::Kind::BandPass ||
      kind == BiquadFilter::Kind::BandStop) {
    if (f2 <= f1 || f2 >= nyquist) {
      throw std::invalid_argument(
          "BiquadFilter design: BandPass/BandStop requires f1 < f2 < fs/2.");
    }
  }
}

void RejectBandKindForLpHpOverload(const char* factoryName,
                                   BiquadFilter::Kind kind) {
  if (kind == BiquadFilter::Kind::BandPass ||
      kind == BiquadFilter::Kind::BandStop) {
    throw std::invalid_argument(
        std::string{"BiquadFilter::"} + factoryName +
        ": BandPass/BandStop requires f2; use the overload that takes it.");
  }
}

}  // namespace

BiquadFilter BiquadFilter::Butterworth(Kind kind, int order, double fs,
                                       double f1, double f2) {
  ValidateClassicalArgs(kind, order, fs, f1, f2);
  return BiquadFilter{filter::internal::DesignFromAnalogLp(
      filter::internal::ButterworthPrototype(order), kind, fs, f1, f2)};
}

BiquadFilter BiquadFilter::ChebyshevI(Kind kind, int order, double fs,
                                      double f1, double f2, double rippleDb) {
  ValidateClassicalArgs(kind, order, fs, f1, f2);
  if (rippleDb <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter design: ChebyshevI passband ripple must be positive.");
  }
  return BiquadFilter{filter::internal::DesignFromAnalogLp(
      filter::internal::ChebyshevIPrototype(order, rippleDb), kind, fs, f1,
      f2)};
}

BiquadFilter BiquadFilter::ChebyshevI(Kind kind, int order, double fs,
                                      double f1, double rippleDb) {
  RejectBandKindForLpHpOverload("ChebyshevI", kind);
  return ChebyshevI(kind, order, fs, f1, 0.0, rippleDb);
}

BiquadFilter BiquadFilter::ChebyshevII(Kind kind, int order, double fs,
                                       double f1, double f2,
                                       double stopAttenDb) {
  ValidateClassicalArgs(kind, order, fs, f1, f2);
  if (stopAttenDb <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter design: ChebyshevII stopband attenuation must be "
        "positive.");
  }
  return BiquadFilter{filter::internal::DesignFromAnalogLp(
      filter::internal::ChebyshevIIPrototype(order, stopAttenDb), kind, fs, f1,
      f2)};
}

BiquadFilter BiquadFilter::ChebyshevII(Kind kind, int order, double fs,
                                       double f1, double stopAttenDb) {
  RejectBandKindForLpHpOverload("ChebyshevII", kind);
  return ChebyshevII(kind, order, fs, f1, 0.0, stopAttenDb);
}

BiquadFilter BiquadFilter::Elliptic(Kind kind, int order, double fs, double f1,
                                    double f2, double rippleDb,
                                    double stopAttenDb) {
  ValidateClassicalArgs(kind, order, fs, f1, f2);
  if (rippleDb <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter design: Elliptic passband ripple must be positive.");
  }
  if (stopAttenDb <= rippleDb) {
    throw std::invalid_argument(
        "BiquadFilter design: Elliptic stopband attenuation must exceed "
        "passband ripple.");
  }
  return BiquadFilter{filter::internal::DesignFromAnalogLp(
      filter::internal::EllipticPrototype(order, rippleDb, stopAttenDb), kind,
      fs, f1, f2)};
}

BiquadFilter BiquadFilter::Elliptic(Kind kind, int order, double fs, double f1,
                                    double rippleDb, double stopAttenDb) {
  RejectBandKindForLpHpOverload("Elliptic", kind);
  return Elliptic(kind, order, fs, f1, 0.0, rippleDb, stopAttenDb);
}

BiquadFilter BiquadFilter::Notch(double fs, double f0, double q) {
  if (fs <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter::Notch: sample rate must be positive.");
  }
  if (q <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter::Notch: quality factor must be positive.");
  }
  const double nyquist = 0.5 * fs;
  if (f0 <= 0.0 || f0 >= nyquist) {
    throw std::invalid_argument(
        "BiquadFilter::Notch: f0 must lie in (0, fs/2).");
  }

  // Standard second-order IIR notch (zero pair on the unit circle at ±w0,
  // pole pair just inside on the same radial line). Matches
  // scipy.signal.iirnotch(f0, Q, fs) exactly:
  //   w0 = 2π·f0/fs,  bw = w0/Q,  β = tan(bw/2),  g = 1/(1 + β)
  //   b = g · [1, -2cos(w0), 1],  a = [1, -2g·cos(w0), 2g - 1]
  // SciPy reference (function iirnotch):
  //   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py
  // Background: Sophocles Orfanidis, "Introduction to Signal Processing"
  // §11.3.2 ("Parametric resonators and notch filters").
  const double w0 = 2.0 * std::numbers::pi * f0 / fs;
  const double bw = w0 / q;
  const double beta = std::tan(0.5 * bw);
  const double gain = 1.0 / (1.0 + beta);
  const double cosW0 = std::cos(w0);

  Section s{
      .b0 = gain,
      .b1 = -2.0 * gain * cosW0,
      .b2 = gain,
      .a1 = -2.0 * gain * cosW0,
      .a2 = 2.0 * gain - 1.0,
  };
  return BiquadFilter{{s}};
}

BiquadFilter BiquadFilter::MovingAverage(int taps) {
  if (taps < 1) {
    throw std::invalid_argument(
        "BiquadFilter::MovingAverage: taps must be >= 1.");
  }

  // A length-N moving average has H(z) = (1/N)·(1 - z⁻ᴺ)/(1 - z⁻¹), whose
  // non-trivial zeros are the Nth roots of unity except z = 1:
  //   z_k = exp(i·2π·k/N), k = 1..N-1
  // Pair each (z_k, z_{N-k}) into one all-zero biquad
  //   (b0, b1, b2, a1, a2) = (1, -2·cos(2πk/N), 1, 0, 0)
  // and, if N is even, emit a single-zero first-order biquad for the unpaired
  // root at z = -1:
  //   (1, 1, 0, 0, 0)
  // The overall 1/N gain is folded into the first section.
  //
  // The factorization of (1 - z⁻ᴺ) into roots of unity is textbook:
  //   https://en.wikipedia.org/wiki/Root_of_unity#Polynomial_form
  // Equivalent to scipy.signal.tf2sos applied to b = [1/N, ..., 1/N], a = [1].
  std::vector<Section> out;
  if (taps == 1) {
    out.push_back({1.0, 0.0, 0.0, 0.0, 0.0});
    return BiquadFilter{out};
  }
  const double N = static_cast<double>(taps);
  const int pairs = (taps - 1) / 2;
  for (int k = 1; k <= pairs; ++k) {
    double c = std::cos(2.0 * std::numbers::pi * k / N);
    out.push_back({1.0, -2.0 * c, 1.0, 0.0, 0.0});
  }
  if (taps % 2 == 0) {
    out.push_back({1.0, 1.0, 0.0, 0.0, 0.0});
  }
  const double g = 1.0 / N;
  out[0].b0 *= g;
  out[0].b1 *= g;
  out[0].b2 *= g;
  return BiquadFilter{out};
}

}  // namespace wpi::math
