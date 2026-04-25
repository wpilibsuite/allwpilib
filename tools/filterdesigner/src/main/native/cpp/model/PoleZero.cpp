// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/PoleZero.hpp"

#include <cmath>
#include <complex>
#include <vector>

namespace wpi::filterdesigner {

namespace {

// Returns roots of p0*z^2 + p1*z + p2 = 0. Degenerate cases (leading
// coefficients zero) collapse to a linear or null polynomial and return
// fewer roots accordingly — this matches how scipy.signal.tf2zpk handles
// trailing-zero numerators when expressing a 1st-order filter as a biquad.
std::vector<std::complex<double>> QuadraticRoots(double p0, double p1,
                                                 double p2) {
  constexpr double kCoefEps = 1e-15;
  if (std::abs(p0) < kCoefEps) {
    if (std::abs(p1) < kCoefEps) {
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
