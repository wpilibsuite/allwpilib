// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/**
 * ImGui panel that draws a two-row subplot of the biquad cascade's
 * magnitude (dB) and unwrapped phase (deg) on a shared log-frequency axis.
 * The response is recomputed each frame; 512 log-spaced points is cheap.
 */
class ResponsePlotView {
 public:
  /**
   * @param sections Current cascade (nullopt or empty clears the plot).
   * @param fs       Sample rate used to compute the response.
   */
  void Display(const std::optional<Sections>& sections, double fs);
};

}  // namespace wpi::filterdesigner
