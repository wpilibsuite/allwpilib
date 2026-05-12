// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <vector>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/**
 * Frequency response of a biquad cascade, sampled on a log-spaced grid from
 * @c fs/numPoints to the Nyquist frequency.
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
