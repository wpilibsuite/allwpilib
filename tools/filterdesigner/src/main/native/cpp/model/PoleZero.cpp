// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/PoleZero.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <vector>

namespace wpi::filterdesigner {

namespace {

// Returns roots of p0*z^2 + p1*z + p2 = 0. Degenerate cases (leading
// coefficients zero) collapse to a linear or null polynomial and return
// fewer roots accordingly — this matches how scipy.signal.tf2zpk handles
// trailing-zero numerators when expressing a 1st-order filter as a biquad.
//
// "Zero" is judged relative to the largest coefficient, not absolutely: the
// designer folds the whole cascade gain into one section's numerator, and for
// a high-order, low-cutoff filter that gain is far below any fixed cutoff
// (an 8th-order 1 Hz low-pass at 1 kHz puts it near 1e-20) while the
// section's two zeros are still perfectly real.
std::vector<std::complex<double>> QuadraticRoots(double p0, double p1,
                                                 double p2) {
  constexpr double kRelEps = 1e-12;
  const double scale = std::max({std::abs(p0), std::abs(p1), std::abs(p2)});
  if (scale == 0.0) {
    return {};
  }
  // Roots are invariant under a common scaling, and without one the
  // discriminant underflows for a section carrying the cascade gain:
  // {1e-200, 0, -1e-200} has zeros at ±1, but 4·p0·p2 rounds to zero and
  // collapses them onto the origin. A power of two rescales exactly.
  int exponent = 0;
  std::frexp(scale, &exponent);
  p0 = std::ldexp(p0, -exponent);
  p1 = std::ldexp(p1, -exponent);
  p2 = std::ldexp(p2, -exponent);

  const double coefEps = kRelEps * std::ldexp(scale, -exponent);
  if (std::abs(p0) < coefEps) {
    if (std::abs(p1) < coefEps) {
      return {};
    }
    return {std::complex<double>{-p2 / p1, 0.0}};
  }
  double disc = p1 * p1 - 4.0 * p0 * p2;
  double twoP0 = 2.0 * p0;
  if (disc >= 0.0) {
    double sqrtDisc = std::sqrt(disc);
    return {
        std::complex<double>{(-p1 + sqrtDisc) / twoP0, 0.0},
        std::complex<double>{(-p1 - sqrtDisc) / twoP0, 0.0},
    };
  }
  double sqrtAbsDisc = std::sqrt(-disc);
  double re = -p1 / twoP0;
  double im = sqrtAbsDisc / twoP0;
  return {
      std::complex<double>{re, im},
      std::complex<double>{re, -im},
  };
}

}  // namespace

PoleZeroPlot ComputePolesZeros(const Sections& sections) {
  PoleZeroPlot out;
  for (const auto& s : sections) {
    auto zeros = QuadraticRoots(s.b0, s.b1, s.b2);
    out.zeros.insert(out.zeros.end(), zeros.begin(), zeros.end());
    // Biquad a0 is normalized to 1, so the denominator polynomial is always
    // z^2 + a1 z + a2 and always has exactly two roots (possibly a double
    // root at the origin for pure-FIR sections).
    auto poles = QuadraticRoots(1.0, s.a1, s.a2);
    out.poles.insert(out.poles.end(), poles.begin(), poles.end());
  }
  return out;
}

}  // namespace wpi::filterdesigner
