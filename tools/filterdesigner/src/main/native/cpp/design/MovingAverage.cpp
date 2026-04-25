// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>

#include "wpi/filterdesigner/design/FilterDesign.hpp"

namespace wpi::filterdesigner {

// A length-N moving average has H(z) = (1/N)·(1 - z⁻ᴺ)/(1 - z⁻¹), whose
// non-trivial zeros are the Nth roots of unity except z = 1:
//   z_k = exp(i·2π·k/N), k = 1..N-1
// Pair each (z_k, z_{N-k}) into one all-zero biquad
//   (b0, b1, b2, a1, a2) = (1, -2·cos(2πk/N), 1, 0, 0)
// and, if N is even, emit a single-zero first-order biquad for the unpaired
// root at z = -1:
//   (1, 1, 0, 0, 0)
// The overall 1/N gain is folded into the first section.
std::optional<Sections> DesignMovingAverage(int taps) {
  if (taps < 1) {
    return std::nullopt;
  }
  Sections out;
  if (taps == 1) {
    out.push_back({1.0, 0.0, 0.0, 0.0, 0.0});
    return out;
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
  return out;
}

}  // namespace wpi::filterdesigner
