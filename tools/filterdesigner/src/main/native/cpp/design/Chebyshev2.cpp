// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <complex>
#include <numbers>

#include "Zpk.hpp"
#include "wpi/filterdesigner/design/FilterDesign.hpp"

namespace wpi::filterdesigner {

namespace detail {

Zpk Chebyshev2Prototype(int order, double stopbandAttenDb) {
  // Match scipy.signal.cheb2ap. Stopband-edge frequency is normalized to
  // 1 rad/s (the point at which the response first reaches the stopband
  // attenuation). Poles are reciprocals of the deformed unit-circle points;
  // zeros sit on the imaginary axis at j/sin(theta_k).
  Zpk out;
  const double delta =
      1.0 / std::sqrt(std::pow(10.0, 0.1 * stopbandAttenDb) - 1.0);
  const double mu = std::asinh(1.0 / delta) / order;

  cplx prodNegP{1.0, 0.0};
  for (int k = 0; k < order; ++k) {
    const double m1 = static_cast<double>(-order + 1 + 2 * k);
    const double theta = std::numbers::pi * m1 / (2.0 * order);
    const cplx pole = -1.0 / std::sinh(cplx{mu, theta});
    out.poles.push_back(pole);
    prodNegP *= -pole;
  }

  // Zeros at z_k = j / sin(theta_k). For odd order the m=0 entry would give
  // sin(0) → infinite zero; we skip it (one fewer zero than poles, matching
  // scipy's pole-pair conventions for odd-order Chebyshev II).
  cplx prodNegZ{1.0, 0.0};
  for (int k = 0; k < order; ++k) {
    const double m = static_cast<double>(-order + 1 + 2 * k);
    if (m == 0.0) {
      continue;
    }
    const cplx zero{0.0, 1.0 / std::sin(std::numbers::pi * m / (2.0 * order))};
    out.zeros.push_back(zero);
    prodNegZ *= -zero;
  }

  out.gain = (prodNegP / prodNegZ).real();
  return out;
}

}  // namespace detail

std::optional<Sections> DesignChebyshev2(FilterKind kind, int order, double fs,
                                         double f1, double f2,
                                         double stopbandAttenDb) {
  if (order < 1 || fs <= 0.0 || stopbandAttenDb <= 0.0) {
    return std::nullopt;
  }
  const double nyquist = 0.5 * fs;
  if (f1 <= 0.0 || f1 >= nyquist) {
    return std::nullopt;
  }
  if ((kind == FilterKind::BandPass || kind == FilterKind::BandStop) &&
      (f2 <= f1 || f2 >= nyquist)) {
    return std::nullopt;
  }
  return detail::DesignFromAnalogLp(
      detail::Chebyshev2Prototype(order, stopbandAttenDb), kind, fs, f1, f2);
}

}  // namespace wpi::filterdesigner
