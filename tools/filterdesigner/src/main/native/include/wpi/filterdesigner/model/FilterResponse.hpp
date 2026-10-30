// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <vector>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/**
 * Frequency response of a biquad cascade, sampled on a log-spaced grid that
 * ends at the Nyquist frequency and starts low enough to show the filter's
 * passband: a decade below the cascade's lowest corner frequency (its slowest
 * pole, or for a pure-FIR section its first null), or @c fs/numPoints,
 * whichever is lower.
 */
struct FrequencyResponse {
  std::vector<double> frequencies;  ///< Hz, log-spaced.
  std::vector<double> magnitudesDb;
  std::vector<double> phasesDegrees;  ///< Unwrapped.

  /**
   * Computes the response of @a sections at @a fs sampled on a log-spaced
   * grid of @a numPoints points.
   *
   * @return Empty on invalid input (@a sections empty, @a fs non-positive,
   *         @a numPoints < 2).
   */
  static std::optional<FrequencyResponse> Compute(const Sections& sections,
                                                  double fs,
                                                  int numPoints = 512);
};

}  // namespace wpi::filterdesigner
