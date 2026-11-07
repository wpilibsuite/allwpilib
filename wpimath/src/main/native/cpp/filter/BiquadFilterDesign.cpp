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

void ValidateClassicalArgs(BiquadFilter::Kind kind, int order,
                           double sampleRate, double lowCutoff,
                           double highCutoff) {
  if (order < 1) {
    throw std::invalid_argument("BiquadFilter design: order must be >= 1.");
  }
  if (sampleRate <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter design: sample rate must be positive.");
  }
  const double nyquist = 0.5 * sampleRate;
  if (lowCutoff <= 0.0 || lowCutoff >= nyquist) {
    throw std::invalid_argument(
        "BiquadFilter design: cutoff must lie in (0, sampleRate/2).");
  }
  if (kind == BiquadFilter::Kind::BandPass ||
      kind == BiquadFilter::Kind::BandStop) {
    if (highCutoff <= lowCutoff || highCutoff >= nyquist) {
      throw std::invalid_argument(
          "BiquadFilter design: BandPass/BandStop requires "
          "lowCutoff < highCutoff < sampleRate/2.");
    }
  }
}

void RejectBandKindForLpHpOverload(const char* factoryName,
                                   BiquadFilter::Kind kind) {
  if (kind == BiquadFilter::Kind::BandPass ||
      kind == BiquadFilter::Kind::BandStop) {
    throw std::invalid_argument(
        std::string{"BiquadFilter::"} + factoryName +
        ": BandPass/BandStop requires the overload that takes both "
        "lowCutoff and highCutoff.");
  }
}

void RejectLpHpKindForBandOverload(const char* factoryName,
                                   BiquadFilter::Kind kind) {
  if (kind == BiquadFilter::Kind::LowPass ||
      kind == BiquadFilter::Kind::HighPass) {
    throw std::invalid_argument(
        std::string{"BiquadFilter::"} + factoryName +
        ": LowPass/HighPass requires the single-cutoff overload.");
  }
}

}  // namespace

BiquadFilter BiquadFilter::Butterworth(Kind kind, int order,
                                       wpi::units::hertz_t sampleRate,
                                       wpi::units::hertz_t cutoff) {
  RejectBandKindForLpHpOverload("Butterworth", kind);
  ValidateClassicalArgs(kind, order, sampleRate.value(), cutoff.value(), 0.0);
  return BiquadFilter{filter::internal::DesignFromAnalogLp(
      filter::internal::ButterworthPrototype(order), kind, sampleRate.value(),
      cutoff.value(), 0.0)};
}

BiquadFilter BiquadFilter::Butterworth(Kind kind, int order,
                                       wpi::units::hertz_t sampleRate,
                                       wpi::units::hertz_t lowCutoff,
                                       wpi::units::hertz_t highCutoff) {
  RejectLpHpKindForBandOverload("Butterworth", kind);
  ValidateClassicalArgs(kind, order, sampleRate.value(), lowCutoff.value(),
                        highCutoff.value());
  return BiquadFilter{filter::internal::DesignFromAnalogLp(
      filter::internal::ButterworthPrototype(order), kind, sampleRate.value(),
      lowCutoff.value(), highCutoff.value())};
}

BiquadFilter BiquadFilter::ChebyshevI(Kind kind, int order,
                                      wpi::units::hertz_t sampleRate,
                                      wpi::units::hertz_t lowCutoff,
                                      wpi::units::hertz_t highCutoff,
                                      double rippleDb) {
  RejectLpHpKindForBandOverload("ChebyshevI", kind);
  ValidateClassicalArgs(kind, order, sampleRate.value(), lowCutoff.value(),
                        highCutoff.value());
  if (rippleDb <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter design: ChebyshevI passband ripple must be positive.");
  }
  return BiquadFilter{filter::internal::DesignFromAnalogLp(
      filter::internal::ChebyshevIPrototype(order, rippleDb), kind,
      sampleRate.value(), lowCutoff.value(), highCutoff.value())};
}

BiquadFilter BiquadFilter::ChebyshevI(Kind kind, int order,
                                      wpi::units::hertz_t sampleRate,
                                      wpi::units::hertz_t cutoff,
                                      double rippleDb) {
  RejectBandKindForLpHpOverload("ChebyshevI", kind);
  ValidateClassicalArgs(kind, order, sampleRate.value(), cutoff.value(), 0.0);
  if (rippleDb <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter design: ChebyshevI passband ripple must be positive.");
  }
  return BiquadFilter{filter::internal::DesignFromAnalogLp(
      filter::internal::ChebyshevIPrototype(order, rippleDb), kind,
      sampleRate.value(), cutoff.value(), 0.0)};
}

BiquadFilter BiquadFilter::ChebyshevII(Kind kind, int order,
                                       wpi::units::hertz_t sampleRate,
                                       wpi::units::hertz_t lowCutoff,
                                       wpi::units::hertz_t highCutoff,
                                       double stopAttenDb) {
  RejectLpHpKindForBandOverload("ChebyshevII", kind);
  ValidateClassicalArgs(kind, order, sampleRate.value(), lowCutoff.value(),
                        highCutoff.value());
  if (stopAttenDb <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter design: ChebyshevII stopband attenuation must be "
        "positive.");
  }
  return BiquadFilter{filter::internal::DesignFromAnalogLp(
      filter::internal::ChebyshevIIPrototype(order, stopAttenDb), kind,
      sampleRate.value(), lowCutoff.value(), highCutoff.value())};
}

BiquadFilter BiquadFilter::ChebyshevII(Kind kind, int order,
                                       wpi::units::hertz_t sampleRate,
                                       wpi::units::hertz_t cutoff,
                                       double stopAttenDb) {
  RejectBandKindForLpHpOverload("ChebyshevII", kind);
  ValidateClassicalArgs(kind, order, sampleRate.value(), cutoff.value(), 0.0);
  if (stopAttenDb <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter design: ChebyshevII stopband attenuation must be "
        "positive.");
  }
  return BiquadFilter{filter::internal::DesignFromAnalogLp(
      filter::internal::ChebyshevIIPrototype(order, stopAttenDb), kind,
      sampleRate.value(), cutoff.value(), 0.0)};
}

BiquadFilter BiquadFilter::Elliptic(Kind kind, int order,
                                    wpi::units::hertz_t sampleRate,
                                    wpi::units::hertz_t lowCutoff,
                                    wpi::units::hertz_t highCutoff,
                                    double rippleDb, double stopAttenDb) {
  RejectLpHpKindForBandOverload("Elliptic", kind);
  ValidateClassicalArgs(kind, order, sampleRate.value(), lowCutoff.value(),
                        highCutoff.value());
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
      sampleRate.value(), lowCutoff.value(), highCutoff.value())};
}

BiquadFilter BiquadFilter::Elliptic(Kind kind, int order,
                                    wpi::units::hertz_t sampleRate,
                                    wpi::units::hertz_t cutoff, double rippleDb,
                                    double stopAttenDb) {
  RejectBandKindForLpHpOverload("Elliptic", kind);
  ValidateClassicalArgs(kind, order, sampleRate.value(), cutoff.value(), 0.0);
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
      sampleRate.value(), cutoff.value(), 0.0)};
}

BiquadFilter BiquadFilter::Notch(wpi::units::hertz_t sampleRate,
                                 wpi::units::hertz_t centerFrequency,
                                 double qualityFactor) {
  const double fs = sampleRate.value();
  const double f0 = centerFrequency.value();
  if (fs <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter::Notch: sample rate must be positive.");
  }
  if (qualityFactor <= 0.0) {
    throw std::invalid_argument(
        "BiquadFilter::Notch: quality factor must be positive.");
  }
  const double nyquist = 0.5 * fs;
  if (f0 <= 0.0 || f0 >= nyquist) {
    throw std::invalid_argument(
        "BiquadFilter::Notch: center frequency must lie in (0, "
        "sampleRate/2).");
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
  const double bw = w0 / qualityFactor;
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
