// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>

#include "wpi/filterdesigner/design/FilterDesign.hpp"

namespace wpi::filterdesigner {

// Matches scipy.signal.iirnotch(f0, Q, fs):
//   w0 = 2π·f0/fs,  bw = w0/Q,  β = tan(bw/2),  g = 1/(1 + β)
//   b = g · [1, -2cos(w0), 1],  a = [1, -2g·cos(w0), 2g - 1]
std::optional<Sections> DesignNotch(double fs, double f0, double q) {
  if (fs <= 0.0 || q <= 0.0) {
    return std::nullopt;
  }
  const double nyquist = 0.5 * fs;
  if (f0 <= 0.0 || f0 >= nyquist) {
    return std::nullopt;
  }

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
  return Sections{s};
}

}  // namespace wpi::filterdesigner
