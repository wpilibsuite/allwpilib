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

Zpk Chebyshev1Prototype(int order, double rippleDb) {
  // Match scipy.signal.cheb1ap. Poles lie on an ellipse in the LHP at:
  //     p_k = -sinh(mu + j*theta_k)
  // where mu = (1/N) * asinh(1/eps), eps = sqrt(10^(rp/10) - 1), and
  // theta_k = pi*(2k - N + 1) / (2N) for k = 0..N-1.
  Zpk out;
  const double eps = std::sqrt(std::pow(10.0, 0.1 * rippleDb) - 1.0);
  const double mu = std::asinh(1.0 / eps) / order;

  cplx prodNegP{1.0, 0.0};
  for (int k = 0; k < order; ++k) {
    const double m = static_cast<double>(-order + 1 + 2 * k);
    const double theta = std::numbers::pi * m / (2.0 * order);
    const cplx pole = -std::sinh(cplx{mu, theta});
    out.poles.push_back(pole);
    prodNegP *= -pole;
  }

  // Gain: forces |H(j0)| = 1 for odd N, 1/sqrt(1+eps^2) for even N (the
  // ripple trough at DC).
  double k = prodNegP.real();
  if (order % 2 == 0) {
    k /= std::sqrt(1.0 + eps * eps);
  }
  out.gain = k;
  return out;
}

}  // namespace detail

std::optional<Sections> DesignChebyshev1(FilterKind kind, int order, double fs,
                                         double f1, double f2,
                                         double rippleDb) {
  if (order < 1 || fs <= 0.0 || rippleDb <= 0.0) {
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
      detail::Chebyshev1Prototype(order, rippleDb), kind, fs, f1, f2);
}

}  // namespace wpi::filterdesigner
