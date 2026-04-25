// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/FilterResponse.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <numbers>

namespace wpi::filterdesigner {

std::optional<FrequencyResponse> FrequencyResponse::Compute(
    const Sections& sections, double fs, int numPoints) {
  if (sections.empty() || fs <= 0.0 || numPoints < 2) {
    return std::nullopt;
  }

  const double nyquist = 0.5 * fs;
  const double fLow = std::max(fs / static_cast<double>(numPoints), 1e-6);
  const double logLow = std::log10(fLow);
  const double logHigh = std::log10(nyquist);

  FrequencyResponse out;
  out.frequencies.reserve(numPoints);
  out.magnitudesDb.reserve(numPoints);
  out.phasesDegrees.reserve(numPoints);

  double prevPhase = 0.0;
  double offset = 0.0;
  for (int i = 0; i < numPoints; ++i) {
    double alpha = static_cast<double>(i) / (numPoints - 1);
    double f = std::pow(10.0, logLow + alpha * (logHigh - logLow));
    double w = 2.0 * std::numbers::pi * f / fs;
    std::complex<double> z1 = std::polar(1.0, -w);
    std::complex<double> z2 = std::polar(1.0, -2.0 * w);
    std::complex<double> h{1.0, 0.0};
    for (const Section& s : sections) {
      std::complex<double> num = s.b0 + s.b1 * z1 + s.b2 * z2;
      std::complex<double> den = 1.0 + s.a1 * z1 + s.a2 * z2;
      h *= num / den;
    }
    double mag = std::abs(h);
    double magDb = 20.0 * std::log10(std::max(mag, 1e-12));
    double phase = std::arg(h);
    if (i > 0) {
      // `while` (not `if`) handles sparse grids where phase jumps by more than
      // a single 2π between samples (high-order filters, wide log spans).
      double delta = phase - prevPhase;
      while (delta > std::numbers::pi) {
        offset -= 2.0 * std::numbers::pi;
        delta -= 2.0 * std::numbers::pi;
      }
      while (delta < -std::numbers::pi) {
        offset += 2.0 * std::numbers::pi;
        delta += 2.0 * std::numbers::pi;
      }
    }
    prevPhase = phase;
    out.frequencies.push_back(f);
    out.magnitudesDb.push_back(magDb);
    out.phasesDegrees.push_back((phase + offset) * 180.0 / std::numbers::pi);
  }
  return out;
}

}  // namespace wpi::filterdesigner
